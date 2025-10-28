#include "LinklistSet.h"
#include <stdlib.h>
#include "../Linklist/Linklist.h"
// 一些声明

typedef struct _SetIterator *_SetIterator;
SetApi _LinklistSet_Global_Api;

// 一些定义

// 内部Set结构体，包含链表和比较函数
typedef struct _Set
{
  SetApi *functions;
  Comparator cmp;            // 比较函数，用于判断元素是否相等
  Linklist *linklist;        // 使用链表作为底层数据结构
  // _SetIterator iteratorEnd;   // 尾部的迭代器
} _Set;

// typedef struct _SetIterator
// {
//   UniversalData *dataPtr;
//   void (*next)(SetIterator iter);
//   lboolean (*hasNext)(SetIterator iter);
//   lboolean (*equals)(SetIterator iterA, SetIterator iterB);
//   void (*destroy)(SetIterator iter);
//   Node *currentNode; // 当前节点指针
// } *_SetIterator;

// Iterator整合进入底层数据结构，如Linklist、Array等
// /**
//  * 内部函数
//  * 迭代器前进到下一个元素
//  */
// void _LinklistSet_Iterator_Next(SetIterator iter)
// {
//   if (!iter)
//     return;
//   _SetIterator _iter = (_SetIterator)iter;
//   if (!_iter->currentNode)
//   {
//     _iter->dataPtr = NULL;
//     return;
//   }
//   _iter->currentNode = _iter->currentNode->next;
//   _iter->dataPtr = &_iter->currentNode->data;
// }
// /**
//  * 内部函数
//  * 判断是否还有下一个元素
//  * @param iter 迭代器
//  * @return true 有下一个元素，false 没有下一个元素
//  */
// lboolean _LinklistSet_Iterator_HasNext(SetIterator iter)
// {
//   if (!iter)
//     return false;
//   _SetIterator _iter = (_SetIterator)iter;
//   return _iter->currentNode != NULL && _iter->currentNode->next != NULL;
// }
// /**
//  * 内部函数
//  * 比较两个迭代器是否相等
//  */
// lboolean _LinklistSet_Iterator_Equals(SetIterator iterA, SetIterator iterB)
// {
//   if (!iterA || !iterB)
//     return false;
//   _SetIterator _iterA = (_SetIterator)iterA;
//   _SetIterator _iterB = (_SetIterator)iterB;
//   return _iterA->currentNode == _iterB->currentNode;
// }
// /**
//  * 内部函数
//  * 销毁迭代器
//  * @param iter 迭代器
//  */
// void _LinklistSet_Iterator_Destroy(SetIterator iter)
// {
//   if (!iter)
//     return;
//   free(iter);
// }
// /**
//  * 内部函数
//  * 创建一个新的迭代器
//  */
// _SetIterator _LinklistSet_Iterator_Create(Node *node)
// {
//   _SetIterator iter = (_SetIterator)malloc(sizeof(struct _SetIterator));
//   iter->dataPtr = node ? &node->data : NULL;
//   iter->next = _LinklistSet_Iterator_Next;
//   iter->hasNext = _LinklistSet_Iterator_HasNext;
//   iter->equals = _LinklistSet_Iterator_Equals;
//   iter->destroy = _LinklistSet_Iterator_Destroy;
//   iter->currentNode = node;
//   return iter;
// }
// /**
//  * 内部函数
//  * 创建一个指向第一个元素的迭代器
//  */
// _SetIterator _LinklistSet_Iterator_Create_Begin(Set *set)
// {
//   if (!set)
//     return NULL;
//   _Set *_set = (_Set *)set;
//   return _LinklistSet_Iterator_Create(_set->linklist->head->first);
// }
// /**
//  * 内部函数
//  * 创建一个指向结束的迭代器
//  * @warning 该迭代器的dataPtr为NULL，表示迭代结束
//  */
// _SetIterator _LinklistSet_Iterator_Create_End()
// {
//   return _LinklistSet_Iterator_Create(NULL);
// }



lboolean LinklistSet_Destroy(Set *set)
{
  if (!set)
    return false;
  _Set *_set = (_Set *)set;
  Linklist_Destroy(_set->linklist);
  // if (_set->iteratorEnd)
  //   free(_set->iteratorEnd);
  free(_set);
  return true;
}

lboolean LinklistSet_Add(Set *set, UniversalData data)
{
  if (!set || !data)
    return false;
  if (LinklistSet_Exist(set, data))
    return true; // 如果集合中已经存在该元素，则直接返回
  _Set *_set = (_Set *)set;
  return Linklist_Append(_set->linklist, data);
}

lboolean LinklistSet_Remove(Set *set, UniversalData data)
{
  if (!set || !data)
    return false;
  _Set *_set = (_Set *)set;
  for (Node *current = _set->linklist->head->first, *prev = NULL; current != NULL; prev = current, current = current->next)
  {
    if (!_set->cmp)
      return false;
    if (_set->cmp(&current->data, &data) == 0)
    {
      if (prev)
        prev->next = current->next;
      else {
        // 如果删除的是第一个元素，更新头指针
        _set->linklist->head->first = current->next;
      }
      if (current == _set->linklist->head->last)
        _set->linklist->head->last = prev;

      free(current);
      _set->linklist->head->length--;
      return true;
    }
  }
  return false;
}

lboolean LinklistSet_Exist(Set *set, UniversalData data)
{
  if (!set || !data)
    return false;
  _Set *_set = (_Set *)set;
  if (!_set->cmp)
    return false;
  for (Node *current = _set->linklist->head->first; current != NULL; current = current->next)
  {
    if (_set->cmp(&current->data, &data) == 0)
      return true;
  }
  return false;
}

ull LinklistSet_Length(Set *set)
{
  if (!set)
    return 0;
  _Set *_set = (_Set *)set;
  return Linklist_Length(_set->linklist);
}

lboolean LinklistSet_IsEmpty(Set *set)
{
  if (!set)
    return true;
  _Set *_set = (_Set *)set;
  return Linklist_IsEmpty(_set->linklist);
}

lboolean LinklistSet_Clear(Set *set)
{
  if (!set)
    return false;
  _Set *_set = (_Set *)set;
  return Linklist_Clear(_set->linklist);
}

UniversalData LinklistSet_Get(Set *set, ull index)
{
  if (!set)
    return NULL;
  _Set *_set = (_Set *)set;
  return Linklist_Get(_set->linklist, index);
}

UniversalData *LinklistSet_At(Set *set, ull index)
{
  if (!set)
    return NULL;
  _Set *_set = (_Set *)set;
  return Linklist_At(_set->linklist, index);
}



SetIterator LinklistSet_Iterator(Set *set, ull *index)
{
  if (!set || !index)
    return NULL;
  _Set *_set = (_Set *)set;
  return Linklist_Iterator(_set->linklist, index);
}

SetIterator LinklistSet_Begin(Set *set)
{
  if (!set)
    return NULL;
  _Set *_set = (_Set *)set;
  return Linklist_Begin(_set->linklist);
}

SetIterator LinklistSet_End(Set *set)
{
  if (!set)
    return NULL;
  _Set *_set = (_Set *)set;
  return Linklist_End(_set->linklist);
}

Set *LinklistSet_Create(UniversalData dataSet[], ull length, Comparator cmp)
{
  Linklist *api = Linklist_Create(dataSet, length);
  _Set *set = malloc(sizeof(_Set));
  set->linklist = api;
  set->functions = &_LinklistSet_Global_Api;
  set->cmp = cmp;
  // 保存结束的迭代器
  // set->iteratorEnd = _LinklistSet_Iterator_Create_End();
  return (Set *)set;
}

/**
 * 并集计算
 * @warning 两个集合的比较函数必须相同
 * @param setA 集合A
 * @param setB 集合B
 * @return 并集
 */
Set *LinklistSet_Union(Set *setA, Set *setB)
{
  _Set *_setA = (_Set *)setA;
  _Set *_setB = (_Set *)setB;
  if (!_setA || !_setB || _setA->cmp != _setB->cmp)
    return NULL;
  Set *resultSet = LinklistSet_Create(NULL, 0, _setA->cmp);
  for (Node *current = _setA->linklist->head->first; current != NULL; current = current->next)
    LinklistSet_Add(resultSet, current->data);
  for (Node *current = _setB->linklist->head->first; current != NULL; current = current->next)
    LinklistSet_Add(resultSet, current->data);
  return resultSet;
}

/**
 * 交集计算
 * @warning 两个集合的比较函数必须相同
 * @param setA 集合A
 * @param setB 集合B
 * @return 交集
 */
Set *LinklistSet_Intersection(Set *setA, Set *setB)
{
  _Set *_setA = (_Set *)setA;
  _Set *_setB = (_Set *)setB;
  if (!_setA || !_setB || _setA->cmp != _setB->cmp)
    return NULL;
  Set *resultSet = LinklistSet_Create(NULL, 0, _setA->cmp);
  for (Node *current = _setA->linklist->head->first; current != NULL; current = current->next)
  {
    if (LinklistSet_Exist(setB, current->data))
      LinklistSet_Add(resultSet, current->data);
  }
  return resultSet;
}

/**
 * 差集计算
 * @warning 两个集合的比较函数必须相同
 * @param setA 被减集合
 * @param setB 减去的集合
 * @return 差集
 */
Set *LinklistSet_Difference(Set *setA, Set *setB)
{
  _Set *_setA = (_Set *)setA;
  _Set *_setB = (_Set *)setB;
  if (!_setA || !_setB)
    return NULL;
  Set *resultSet = LinklistSet_Create(NULL, 0, _setA->cmp);
  for (Node *current = _setA->linklist->head->first; current != NULL; current = current->next)
  {
    if (!LinklistSet_Exist(setB, current->data))
      LinklistSet_Add(resultSet, current->data);
  }
  return resultSet;
}

/**
 * 补集计算
 * @warning 两个集合的比较函数必须相同
 * @param universalSet 全集
 * @param subset 子集
 * @return 补集
 */
Set *LinklistSet_Complement(Set *universalSet, Set *subset)
{
  _Set *_universalSet = (_Set *)universalSet;
  // _Set *_subset = (_Set *)subset;
  // if (!_universalSet || !_subset)
  //   return NULL;
  if (!_universalSet || !subset)
    return NULL;
  Set *resultSet = LinklistSet_Create(NULL, 0, _universalSet->cmp);
  for (Node *current = _universalSet->linklist->head->first; current != NULL; current = current->next)
  {
    if (!LinklistSet_Exist(subset, current->data))
      LinklistSet_Add(resultSet, current->data);
  }
  return resultSet;
}

/**
 * 子集判断
 * @warning 两个集合的比较函数必须相同
 * @param subset 可能的子集
 * @param set 可能的全集
 */
lboolean LinklistSet_IsSubset(Set *subset, Set *set)
{
  _Set *_subset = (_Set *)subset;
  if (!_subset || !set)
    return false;
  for (Node *current = _subset->linklist->head->first; current != NULL; current = current->next)
  {
    if (!LinklistSet_Exist(set, current->data))
      return false;
  }
  return true;
}

SetApi _LinklistSet_Global_Api = {
  .add = LinklistSet_Add,
  .clear = LinklistSet_Clear,
  .destroy = LinklistSet_Destroy,
  .empty = LinklistSet_IsEmpty,
  .exist = LinklistSet_Exist,
  .length = LinklistSet_Length,
  .remove = LinklistSet_Remove,
  .opComplement = LinklistSet_Complement,
  .opDifference = LinklistSet_Difference,
  .opIntersection = LinklistSet_Intersection,
  .opUnion = LinklistSet_Union,
  .isSubset = LinklistSet_IsSubset,
  .get = LinklistSet_Get,
  .at = LinklistSet_At,
  .iterator = LinklistSet_Iterator,
  .begin = LinklistSet_Begin,
  .end = LinklistSet_End
}; // 定义一个静态的api变量，避免每次创建集合都要分配一块内存
