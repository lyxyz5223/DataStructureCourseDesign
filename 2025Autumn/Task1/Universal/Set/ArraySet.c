#include "ArraySet.h"
#include "../Array/Array.h"
#include <stdlib.h>
// 内部Set结构体，包含数组和比较函数
typedef struct _ArraySet
{
  SetApi *functions;
  Comparator cmp;    // 比较函数，用于判断元素是否相等
  Array *array;      // 使用动态数组作为底层数据结构
} _ArraySet;

SetApi _ArraySet_Global_Api;
/**
 * 创建一个集合，并且定义集合元素的比较函数（主要用于集合元素的等值判断）
 * @param dataSet 初始数据集
 * @param length 初始数据集长度
 * @param cmp 集合元素的比较函数
 * @return 集合指针
 */
Set *ArraySet_Create(UniversalData dataSet[], ull length, Comparator cmp)
{
  Array* api = Array_Create(dataSet, length);
  _ArraySet* set = malloc(sizeof(_ArraySet));
  set->array = api;
  set->functions = &_ArraySet_Global_Api;
  set->cmp = cmp;
  return (Set *)set;
}


/*
 * 非接口函数
 */
lboolean ArraySet_Destroy(Set *set)
{
  if (!set)
    return false;
  _ArraySet *_set = (_ArraySet *)set;
  if (_set->array)
    _set->array->functions->destroy(_set->array);
  free(_set);
  return true;
}
lboolean ArraySet_Add(Set *set, UniversalData data)
{
  if (!set || !data)
    return false;
  if (ArraySet_Exist(set, data))
    return true; // 如果集合中已经存在该元素，则直接返回
  _ArraySet *_set = (_ArraySet *)set;
  return _set->array->functions->append(_set->array, data);
}
lboolean ArraySet_Remove(Set *set, UniversalData data)
{
  if (!set || !data)
    return false;
  _ArraySet *_set = (_ArraySet *)set;
  if (!_set->cmp)
    return false;
  for (ull i = 0; i < _set->array->functions->length(_set->array); i++)
  {
    if (_set->cmp(_set->array->functions->at(_set->array, i), &data) == 0)
    {
      _set->array->functions->remove(_set->array, i);
      return true;
    }
  }
  return false;
}
lboolean ArraySet_Exist(Set *set, UniversalData data)
{
  if (!set || !data)
    return false;
  _ArraySet *_set = (_ArraySet *)set;
  if (!_set->cmp)
    return false;
  for (ull i = 0; i < _set->array->functions->length(_set->array); i++)
  {
    if (_set->cmp(_set->array->functions->at(_set->array, i), &data) == 0)
      return true;
  }
  return false;
}
ull ArraySet_Length(Set *set)
{
  if (!set)
    return 0;
  _ArraySet *_set = (_ArraySet *)set;
  return _set->array->functions->length(_set->array);
}
lboolean ArraySet_IsEmpty(Set *set)
{
  if (!set)
    return true;
  _ArraySet *_set = (_ArraySet *)set;
  return _set->array->functions->isEmpty(_set->array);
}
lboolean ArraySet_Clear(Set *set)
{
  if (!set)
    return false;
  _ArraySet *_set = (_ArraySet *)set;
  return _set->array->functions->clear(_set->array);
}
UniversalData ArraySet_Back(Set *set)
{
  if (!set)
    return NULL;
  _ArraySet *_set = (_ArraySet *)set;
  return _set->array->functions->back(_set->array);
}
UniversalData ArraySet_Get(Set *set, ull index)
{
  if (!set)
    return NULL;
  _ArraySet *_set = (_ArraySet *)set;
  return _set->array->functions->get(_set->array, index);
}
UniversalData *ArraySet_At(Set *set, ull index)
{
  if (!set)
    return NULL;
  _ArraySet *_set = (_ArraySet *)set;
  return _set->array->functions->at(_set->array, index);
}

// 迭代器相关接口
Iterator ArraySet_Iterator(Set *set, ull *index)
{
  if (!set || !index)
    return NULL;
  _ArraySet *_set = (_ArraySet *)set;
  return _set->array->functions->iterator(_set->array, index);
}
Iterator ArraySet_Begin(Set *set)
{
  if (!set)
    return NULL;
  _ArraySet *_set = (_ArraySet *)set;
  return _set->array->functions->begin(_set->array);
}
Iterator ArraySet_End(Set *set)
{
  if (!set)
    return NULL;
  _ArraySet *_set = (_ArraySet *)set;
  return _set->array->functions->end(_set->array);
}

/**
 * 并集计算
 * @warning 两个集合的比较函数必须相同
 * @param setA 集合A
 * @param setB 集合B
 * @return 并集
 */
Set *ArraySet_Union(Set *setA, Set *setB)
{
  _ArraySet *_setA = (_ArraySet *)setA;
  _ArraySet *_setB = (_ArraySet *)setB;
  if (!_setA || !_setB || _setA->cmp != _setB->cmp)
    return NULL;
  Set *resultSet = ArraySet_Create(NULL, 0, _setA->cmp);
  for (ull i = 0; i < _setA->array->functions->length(_setA->array); i++)
    ArraySet_Add(resultSet, _setA->array->functions->get(_setA->array, i));
  for (ull i = 0; i < _setB->array->functions->length(_setB->array); i++)
    ArraySet_Add(resultSet, _setB->array->functions->get(_setB->array, i));
  return resultSet;
}

/**
 * 交集计算
 * @warning 两个集合的比较函数必须相同
 * @param setA 集合A
 * @param setB 集合B
 * @return 交集
 */
Set *ArraySet_Intersection(Set *setA, Set *setB)
{
  _ArraySet *_setA = (_ArraySet *)setA;
  _ArraySet *_setB = (_ArraySet *)setB;
  if (!_setA || !_setB || _setA->cmp != _setB->cmp)
    return NULL;
  Set *resultSet = ArraySet_Create(NULL, 0, _setA->cmp);
  for (ull i = 0; i < _setA->array->functions->length(_setA->array); i++)
  {
    UniversalData data = _setA->array->functions->get(_setA->array, i);
    if (ArraySet_Exist(setB, data))
      ArraySet_Add(resultSet, data);
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
Set *ArraySet_Difference(Set *setA, Set *setB)
{
  // 方法一：查找不存在于B中的A的元素
  _ArraySet *_setA = (_ArraySet *)setA;
  _ArraySet *_setB = (_ArraySet *)setB;
  if (!_setA || !_setB)
    return NULL;
  Set *resultSet = ArraySet_Create(NULL, 0, _setA->cmp);
  for (ull i = 0; i < _setA->array->functions->length(_setA->array); i++)
  {
    UniversalData data = _setA->array->functions->get(_setA->array, i);
    if (!ArraySet_Exist(setB, data))
      ArraySet_Add(resultSet, data);
  }
  return resultSet;
  // 方法二：计算A与B的交集，然后从A中删除交集中的元素
  // _ArraySet *_setA = (_ArraySet *)setA;
  // _ArraySet *_setB = (_ArraySet *)setB;
  // if (!_setA || !_setB || _setA->cmp != _setB->cmp)
  //   return NULL;
  // Set *intersectionSet = ArraySet_Intersection(setA, setB);
  // _ArraySet *resultSet = ArraySet_Create(setA, _setA->array->functions->length(_setA->array), _setA->cmp);
  // resultSet->functions->remove(resultSet, intersectionSet);
  // ArraySet_Destroy(intersectionSet);
  return resultSet;
}

/**
 * 补集计算
 * @warning 两个集合的比较函数必须相同
 * @param universalSet 全集
 * @param subset 子集
 * @return 补集
 */
Set *ArraySet_Complement(Set *universalSet, Set *subset)
{
  _ArraySet *_universalSet = (_ArraySet *)universalSet;
  _ArraySet *_subset = (_ArraySet *)subset;
  if (!_universalSet || !_subset)
    return NULL;
  Set *resultSet = ArraySet_Create(NULL, 0, _universalSet->cmp);
  for (ull i = 0; i < _universalSet->array->functions->length(_universalSet->array); i++)
  {
    UniversalData data = _universalSet->array->functions->get(_universalSet->array, i);
    if (!ArraySet_Exist(subset, data))
      ArraySet_Add(resultSet, data);
  }
  return resultSet;
}

/**
 * 子集判断
 * @warning 两个集合的比较函数必须相同
 * @param subset 可能的子集
 * @param set 可能的全集
 */
lboolean ArraySet_IsSubset(Set *subset, Set *set)
{
  _ArraySet *_subset = (_ArraySet *)subset;
  _ArraySet *_set = (_ArraySet *)set;
  if (!_subset || !_set)
    return false;
  for (ull i = 0; i < _subset->array->functions->length(_subset->array); i++)
  {
    UniversalData data = _subset->array->functions->get(_subset->array, i);
    if (!ArraySet_Exist(set, data))
      return false;
  }
  return true;
}

SetApi _ArraySet_Global_Api = {
  .add = ArraySet_Add,
  .clear = ArraySet_Clear,
  .destroy = ArraySet_Destroy,
  .empty = ArraySet_IsEmpty,
  .exist = ArraySet_Exist,
  .length = ArraySet_Length,
  .remove = ArraySet_Remove,
  .get = ArraySet_Get,
  .at = ArraySet_At,

  //
  .iterator = ArraySet_Iterator,
  .begin = ArraySet_Begin,
  .end = ArraySet_End,
  
  //
  .opUnion = ArraySet_Union,
  .opIntersection = ArraySet_Intersection,
  .opDifference = ArraySet_Difference,
  .opComplement = ArraySet_Complement,
  .isSubset = ArraySet_IsSubset
};