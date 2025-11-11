#ifndef LINKLIST_H
#define LINKLIST_H
#include "../types.h"
typedef struct Linklist Linklist;

typedef struct LinklistApi
{
  lboolean (*destroy)(Linklist *list);
  lboolean (*insert)(Linklist *list, ull index, UniversalData data);
  lboolean (*insertBeforeIterator)(Linklist *list, Iterator iterator, UniversalData data);
  lboolean (*append)(Linklist *list, UniversalData data);
  lboolean (*remove)(Linklist *list, ull index);
  lboolean (*removeByIterator)(Linklist *list, Iterator iterator);
  lboolean (*replace)(Linklist *list, ull index, UniversalData data);
  lboolean (*replaceByIterator)(Linklist *list, Iterator iterator, UniversalData data);
  UniversalData (*get)(Linklist *list, ull index);
  UniversalData* (*at)(Linklist *list, ull index);
  ull (*length)(Linklist *list);
  lboolean (*isEmpty)(Linklist *list);
  lboolean (*clear)(Linklist *list);
  UniversalData (*back)(Linklist *list);
  // 迭代器相关
  Iterator (*iterator)(Linklist *list, ull index);
  Iterator (*begin)(Linklist *list);
  Iterator (*end)(Linklist *list);
} LinklistApi;

typedef struct Node
{
  void *data;
  struct Node *next;
} Node;

typedef struct Head
{
  Node *first;
  Node *last;
  ull length;
} Head;

typedef struct Linklist
{
  Head *head;
  LinklistApi *functions;
} Linklist;
#ifdef __cplusplus
extern "C"
{
#endif
  /**
   * 创建一个链表
   * @param dataSet 初始数据集
   * @param length 初始数据集长度
   * @return 链表api接口指针
   */
  Linklist *Linklist_Create(UniversalData dataSet[], ull length);

  /*
  * 非接口函数
  */
  lboolean Linklist_Destroy(Linklist *list);
  lboolean Linklist_Insert(Linklist *list, ull index, UniversalData data);
  /**
   * 通过迭代器在指定节点前插入新节点
   * @param list 链表指针
   * @param iterator 迭代器指针，可为End迭代器
   * @param data 新节点数据
   * @return 插入成功返回true，失败返回false
   * @note 时间复杂度为O(1)
   */
  lboolean Linklist_InsertBeforeIterator(Linklist *list, Iterator iterator, UniversalData data);
  lboolean Linklist_Append(Linklist *list, UniversalData data);
  lboolean Linklist_Remove(Linklist *list, ull index);
  /**
   * 通过迭代器删除一个节点
   * @param list 链表指针
   * @param iterator 迭代器指针，不能为End迭代器
   * @return 删除成功返回true，失败返回false
   * @note 删除节点后，迭代器会自动指向下一个节点，时间复杂度为O(1)
   */
  lboolean Linklist_RemoveByIterator(Linklist *list, Iterator iterator);
  lboolean Linklist_Replace(Linklist *list, ull index, UniversalData data);
  /**
   * @param list 链表指针
   * @param iterator 迭代器指针，不能为End迭代器
   * @param data 新数据
   * @return 替换成功返回true，失败返回false
   * @note 此函数实际上并未使用list，但请确保list不为0，时间复杂度为O(1)
   */
  lboolean Linklist_ReplaceByIterator(Linklist *list, Iterator iterator, UniversalData data);
  UniversalData Linklist_Get(Linklist *list, ull index); // Unrecommended function
  UniversalData* Linklist_At(Linklist *list, ull index); // Unrecommended function
  ull Linklist_Length(Linklist *list);
  lboolean Linklist_IsEmpty(Linklist *list);
  lboolean Linklist_Clear(Linklist *list);
  UniversalData Linklist_Back(Linklist *list);
  // 迭代器
  Iterator Linklist_Iterator(Linklist *list, ull index);
  Iterator Linklist_Begin(Linklist *list);
  Iterator Linklist_End(Linklist *list);

#ifdef __cplusplus
}
#endif

#endif // LINKLIST_H
