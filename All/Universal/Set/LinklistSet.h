#ifndef LINKLIST_SET_H
#define LINKLIST_SET_H
// 导入公共接口
#include "Set.h"
#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * 创建一个集合，并且定义集合元素的比较函数（主要用于集合元素的等值判断）
   * @param dataSet 初始数据集
   * @param length 初始数据集长度
   * @param cmp 集合元素的比较函数
   * @return 集合指针
   */
  Set *LinklistSet_Create(UniversalData dataSet[], ull length, Comparator cmp);
  /*
  * 非接口函数
  */
  lboolean LinklistSet_Destroy(Set *set);
  lboolean LinklistSet_Add(Set *set, UniversalData data);
  lboolean LinklistSet_Remove(Set *set, UniversalData data);
  lboolean LinklistSet_Exist(Set *set, UniversalData data);
  ull LinklistSet_Length(Set *set);
  lboolean LinklistSet_IsEmpty(Set *set);
  lboolean LinklistSet_Clear(Set *set);
  UniversalData LinklistSet_Get(Set *set, ull index);
  UniversalData *LinklistSet_At(Set *set, ull index);
  SetIterator LinklistSet_Iterator(Set *set, ull index);
  SetIterator LinklistSet_Begin(Set *set);
  SetIterator LinklistSet_End(Set *set);
  /**
   * 集合运算函数
   */
  Set *LinklistSet_Union(Set *setA, Set *setB);
  Set *LinklistSet_Intersection(Set *setA, Set *setB);
  Set *LinklistSet_Difference(Set *setA, Set *setB);
  Set *LinklistSet_Complement(Set *universalSet, Set *subset);
  lboolean LinklistSet_IsSubset(Set *subset, Set *set);

#ifdef __cplusplus
}
#endif

#endif // LINKLIST_SET_H
