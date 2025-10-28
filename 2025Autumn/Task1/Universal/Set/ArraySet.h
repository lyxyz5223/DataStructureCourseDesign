#ifndef ARRAY_SET_H
#define ARRAY_SET_H
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
  Set *ArraySet_Create(UniversalData dataSet[], ull length, Comparator cmp);

  /*
    * 非接口函数
    */
  lboolean ArraySet_Destroy(Set *set);
  lboolean ArraySet_Add(Set *set, UniversalData data);
  lboolean ArraySet_Remove(Set *set, UniversalData data);
  lboolean ArraySet_Exist(Set *set, UniversalData data);
  ull ArraySet_Length(Set *set);
  lboolean ArraySet_IsEmpty(Set *set);
  lboolean ArraySet_Clear(Set *set);
  UniversalData ArraySet_Get(Set *set, ull index);
  UniversalData *ArraySet_At(Set *set, ull index);
  SetIterator ArraySet_Iterator(Set *set, ull *index);
  SetIterator ArraySet_Begin(Set *set);
  SetIterator ArraySet_End(Set *set);
  /**
   * 集合运算函数
   */
  Set *ArraySet_Union(Set *setA, Set *setB);
  Set *ArraySet_Intersection(Set *setA, Set *setB);
  Set *ArraySet_Difference(Set *setA, Set *setB);
  Set *ArraySet_Complement(Set *universalSet, Set *subset);
  lboolean ArraySet_IsSubset(Set *subset, Set *set);

#ifdef __cplusplus
}
#endif

#endif // ARRAY_SET_H