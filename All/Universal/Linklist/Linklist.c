#include "Linklist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 声明全局api接口
static LinklistApi _Linklist_Global_Api;

typedef struct _Iterator
{
  UniversalData *dataPtr;
  void (*next)(Iterator iter);
  lboolean (*hasNext)(Iterator iter);
  lboolean (*equals)(Iterator iterA, Iterator iterB);
  void (*destroy)(Iterator iter);
  Node *currentNode; // 当前节点指针
  Node *previousNode; // 当前节点的上一个节点指针
} *_Iterator;
typedef struct _Linklist
{
  Head *head;
  LinklistApi *functions;
  _Iterator iteratorEnd; // 尾部的迭代器
} _Linklist;

/**
 * 内部函数
 * 迭代器前进到下一个元素
 */
void _Linklist_Iterator_Next(Iterator iter)
{
  if (!iter)
    return;
  _Iterator _iter = (_Iterator)iter;
  if (!_iter->currentNode)
  {
    _iter->dataPtr = NULL;
    return;
  }
  _iter->previousNode = _iter->currentNode;
  _iter->currentNode = _iter->currentNode->next;
  _iter->dataPtr = &_iter->currentNode->data;
}
/**
 * 内部函数
 * 判断是否还有下一个元素
 * @param iter 迭代器
 * @return true 有下一个元素，false 没有下一个元素
 */
lboolean _Linklist_Iterator_HasNext(Iterator iter)
{
  if (!iter)
    return false;
  _Iterator _iter = (_Iterator)iter;
  return _iter->currentNode != NULL && _iter->currentNode->next != NULL;
}
/**
 * 内部函数
 * 比较两个迭代器是否相等
 */
lboolean _Linklist_Iterator_Equals(Iterator iterA, Iterator iterB)
{
  if (!iterA || !iterB)
    return false;
  _Iterator _iterA = (_Iterator)iterA;
  _Iterator _iterB = (_Iterator)iterB;
  return _iterA->currentNode == _iterB->currentNode;
}
/**
 * 内部函数
 * 销毁迭代器
 * @param iter 迭代器
 */
void _Linklist_Iterator_Destroy(Iterator iter)
{
  if (!iter)
    return;
  free(iter);
}
/**
 * 内部函数
 * 创建一个新的迭代器
 */
_Iterator _Linklist_Iterator_Create(Node *curNode, Node* prevNode)
{
  _Iterator iter = (_Iterator)malloc(sizeof(struct _Iterator));
  iter->dataPtr = curNode ? &curNode->data : NULL;
  iter->next = _Linklist_Iterator_Next;
  iter->hasNext = _Linklist_Iterator_HasNext;
  iter->equals = _Linklist_Iterator_Equals;
  iter->destroy = _Linklist_Iterator_Destroy;
  iter->currentNode = curNode;
  iter->previousNode = prevNode;
  return iter;
}
/**
 * 内部函数
 * 创建一个指向第一个元素的迭代器
 */
_Iterator _Linklist_Iterator_Create_Begin(Linklist *list)
{
  if (!list)
    return NULL;
  return _Linklist_Iterator_Create(list->head->first, NULL);
}
/**
 * 内部函数
 * 创建一个指向结束的迭代器
 * @warning 该迭代器的dataPtr为NULL，表示迭代结束
 */
_Iterator _Linklist_Iterator_Create_End(Linklist* list)
{
  return _Linklist_Iterator_Create(NULL, list->head->last);
}

lboolean Linklist_Destroy(Linklist *list)
{
  if (!list)
    return false;
  while (list->head->first)
  {
    Node *temp = list->head->first;
    list->head->first = list->head->first->next;
    free(temp);
  }
  free(list->head);
  free(list);
  return true;
}

lboolean Linklist_Insert(Linklist *list, ull index, UniversalData data)
{
  if (!list)
    return false; // 如果链表不存在，则直接返回

  if (index > list->head->length)
    return false; // 如果索引值超出链表原有长度，则直接返回

  Node *newNode = malloc(sizeof(Node));
  newNode->data = data;    // 赋值参数传递的数据
  newNode->next = 0;       // 先设置为0
  if (!list->head->length) // 如果原来链表中一个元素都没有
  {
    list->head->first = list->head->last = newNode; // 赋值链表第一项和最后一项
    list->head->length++;
    return true;
  }
  if (index == list->head->length) // 最后一个元素后插入
  {
    list->head->last = list->head->last->next = newNode; // 如果新元素插在链表末尾，则修改末尾元素
    list->head->length++;
    return true;
  }
  if (index == 0) // 第一个元素前插入，这里不可能出现链表长度为0的情况，因此不需要修改last节点
  {
    newNode->next = list->head->first;
    list->head->first = newNode;
    list->head->length++;
    return true;
  }
  Node *current = list->head->first; // 初始化为第0项
  for (ull i = 0; i < index - 1; i++)
    current = current->next;     // 拿到第index个元素
  newNode->next = current->next; // 将新元素的下一个元素设置为原有的元素
  current->next = newNode;       // 新元素成为上一个元素的下一个元素
  list->head->length++;
  return true;
}

lboolean Linklist_InsertBeforeIterator(Linklist *list, Iterator iterator, UniversalData data)
{
  if (!list || !iterator)
    return false; // 如果链表不存在，则直接返回
  _Iterator iter = (_Iterator)iterator;
  _Linklist* l = (_Linklist*)list;
  // 创建新节点
  Node *newNode = malloc(sizeof(Node));
  if (!newNode)
    return false;
  newNode->data = data; // 赋值参数传递的数据
  newNode->next = iter->currentNode;
  if (list->head->first == iter->currentNode)
    list->head->first = newNode;
  else
    iter->previousNode->next = newNode;
  if (iter->equals(iterator, (Iterator)l->iteratorEnd))
    list->head->last = newNode;
  iter->previousNode = newNode;
  return true;
}

lboolean Linklist_Append(Linklist *list, UniversalData data)
{
  return Linklist_Insert(list, list->head->length, data);
}

lboolean Linklist_Remove(Linklist *list, ull index)
{
  if (!list)
    return false;

  if (index >= list->head->length)
    return false;

  Node *current = list->head->first;
  Node *previous = NULL;

  for (ull i = 0; i < index; i++)
  {
    previous = current;
    current = current->next;
  }

  if (previous) // 不是第一个节点
    previous->next = current->next;
  else
    list->head->first = current->next;

  if (current == list->head->last) // 是最后一个节点
    list->head->last = previous;

  free(current);
  list->head->length--;
  return true;
}

lboolean Linklist_RemoveByIterator(Linklist *list, Iterator iterator)
{
  if (!list || !iterator)
    return false; // 如果链表不存在，则直接返回
  _Iterator iter = (_Iterator)iterator;
  _Linklist *l = (_Linklist *)list;
  if (iter->equals(iterator, (Iterator)l->iteratorEnd))
    return false;
  if (list->head->first == iter->currentNode)
    list->head->first = iter->currentNode->next;
  else
    iter->previousNode->next = iter->currentNode->next;
  if (iter->currentNode == list->head->last)
    list->head->last = iter->previousNode;
  free(iter->currentNode); // 释放当前节点
  list->head->length--;
  // 将迭代器向下迭代
  iter->currentNode = iter->currentNode->next;
  iter->dataPtr = &iter->currentNode->data;
  return true;
}

lboolean Linklist_Replace(Linklist *list, ull index, UniversalData data)
{
  if (!list)
    return false;

  if (index >= list->head->length)
    return false;

  Node *current = list->head->first;
  for (ull i = 0; i < index; i++)
    current = current->next;

  current->data = data;
  return true;
}

lboolean Linklist_ReplaceByIterator(Linklist *list, Iterator iterator, UniversalData data)
{
  if (!list || !iterator)
    return false;
  _Iterator iter = (_Iterator)iterator;
  iter->currentNode->data = data;
  return true;
}

UniversalData Linklist_Get(Linklist *list, ull index)
{
  if (!list || index >= list->head->length)
    return NULL;

  Node *current = list->head->first;
  for (ull i = 0; i < index; i++)
    current = current->next;

  return current->data;
}

UniversalData* Linklist_At(Linklist* list, ull index)
{
  if (!list || index >= list->head->length)
    return NULL;

  Node *current = list->head->first;
  for (ull i = 0; i < index; i++)
    current = current->next;

  return &current->data;
}

ull Linklist_Length(Linklist *list)
{
  if (!list)
    return 0;
  return list->head->length;
}
ull Linklist_Capacity(Linklist *list)
{
  if (!list)
    return 0;
  return list->head->length;
}
lboolean Linklist_IsEmpty(Linklist *list)
{
  if (!list)
    return true;
  return (list->head->length == 0 ? true : false);
}
lboolean Linklist_Clear(Linklist *list)
{
  if (!list)
    return false;
  while (list->head->first)
  {
    Node *temp = list->head->first;
    list->head->first = list->head->first->next;
    free(temp);
  }
  list->head->last = NULL;
  list->head->length = 0;
  return true;
}
UniversalData Linklist_Back(Linklist *list)
{
  if (!list || !list->head->last)
    return NULL;
  return list->head->last->data;
}
Iterator Linklist_Iterator(Linklist *list, ull index)
{
  if (!list)
    return NULL;
  if (index >= list->head->length) // 如果索引超出范围，返回尾部迭代器
    return (Iterator)((_Linklist *)list)->iteratorEnd; // 返回尾部迭代器
  Node *current = list->head->first;
  Node *prev = NULL;
  for (ull i = 0; i < index; i++)
  {
    prev = current;
    current = current->next;
  }
  _Iterator iter = _Linklist_Iterator_Create(current, prev);
  return (Iterator)iter;
}
Iterator Linklist_Begin(Linklist *list)
{
  if (!list)
    return NULL;
  return (Iterator)_Linklist_Iterator_Create_Begin(list);
}
Iterator Linklist_End(Linklist *list)
{
  if (!list)
    return NULL;
  return (Iterator)((_Linklist *)list)->iteratorEnd; // 返回尾部迭代器
}

Linklist *Linklist_Create(UniversalData dataSet[], ull length)
{
  Head *head = (Head *)malloc(sizeof(Head));
  head->first = NULL;
  head->last = NULL;
  head->length = length;
  // 初始化链表节点
  for (ull i = 0; i < length; i++)
  {
    Node *node = (Node *)malloc(sizeof(Node));
    if (dataSet)
      node->data = dataSet[i];
    node->next = 0;
    // 为头部赋值
    if (!head->first)
      head->first = node;
    else
      head->last->next = node; // 将上一个节点的next指向当前节点
    head->last = node;
  }
  // LinklistApi *api = malloc(sizeof(LinklistApi));
  // api->destroy = Linklist_Destroy;
  // api->insert = Linklist_Insert;
  // api->append = Linklist_Append;
  // api->remove = Linklist_Remove;
  // api->replace = Linklist_Replace;
  // api->get = Linklist_Get;
  // api->length = Linklist_Length;
  // api->isEmpty = Linklist_IsEmpty;
  // api->clear = Linklist_Clear;
  // api->back = Linklist_Back;

  _Linklist *list = malloc(sizeof(_Linklist));
  list->head = head;
  list->functions = &_Linklist_Global_Api;
  list->iteratorEnd = _Linklist_Iterator_Create_End((Linklist *)list);
  return (Linklist *)list;
}


static LinklistApi _Linklist_Global_Api = {
  .append = Linklist_Append,
  .back = Linklist_Back,
  .clear = Linklist_Clear,
  .destroy = Linklist_Destroy,
  .isEmpty = Linklist_IsEmpty,
  .get = Linklist_Get,
  .at = Linklist_At,
  .insert = Linklist_Insert,
  .length = Linklist_Length,
  .remove = Linklist_Remove,
  .replace = Linklist_Replace,

  .iterator = Linklist_Iterator,
  .begin = Linklist_Begin,
  .end = Linklist_End,

  .insertBeforeIterator = Linklist_InsertBeforeIterator,
  .removeByIterator = Linklist_RemoveByIterator,
  .replaceByIterator = Linklist_ReplaceByIterator
}; // 定义一个静态的api变量，避免每次创建链表都要分配一块内存
