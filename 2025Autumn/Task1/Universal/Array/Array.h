#ifndef ARRAY_H
#define ARRAY_H
#include "../types.h"

typedef struct Array Array;
typedef struct ArrayApi
{
  lboolean (*destroy)(Array *array);
  lboolean (*set)(Array *array, ull index, UniversalData data);
  /**
   * 获取index位置的元素
   * @note 推荐使用 at() 函数获取元素地址
   * @param array 数组指针
   * @param index 索引
   * @return 元素副本
   */
  UniversalData (*get)(Array *array, ull index);
  /**
   * 返回index位置的元素引用
   * @param array 数组指针
   * @param index 索引
   * @return 元素地址
   */
  UniversalData *(*at)(Array *array, ull index);
  /**
   * 获取数组长度
   * @param array 数组指针
   * @return 数组长度
   */
  ull (*length)(Array *array);
  /**
   * 获取数组容量
   * @param array 数组指针
   * @return 数组容量
   */
  ull (*capacity)(Array *array);
  /**
   * 判断数组是否为空
   * @param array 数组指针
   * @return true为空，false不为空
   */
  lboolean (*isEmpty)(Array *array);
  /**
   * 清空数组
   * @param array 数组指针
   * @return true成功，false失败
   * @note 清空操作不会释放内存，只会将长度置0
   */
  lboolean (*clear)(Array *array);
  /**
   * 在index位置插入一个元素，原index及其后续元素后移
   * @param array 数组指针
   * @param index 索引
   * @param data 元素
   * @return true成功，false失败
   * @warning 请不要同时使用迭代器和插入操作，否则可能由于数组内存重新分配导致迭代器失效
   * @note 可能导致数组重新分配内存
   */
  lboolean (*insert)(Array *array, ull index, UniversalData data);
  /**
   * 在数组末尾添加一个元素
   * @param array 数组指针
   * @param data 元素
   * @return true成功，false失败
   * @warning 请不要同时使用迭代器和插入操作，否则可能由于数组内存重新分配导致迭代器失效
   * @note 可能导致数组重新分配内存
   */
  lboolean (*append)(Array *array, UniversalData data);
  /**
   * 删除index位置的元素
   * @param array 数组指针
   * @param index 索引
   * @return true成功，false失败
   * @warning 请不要同时使用迭代器和删除操作，否则可能导致迭代器跳过n个元素（n为删除个数）
   * @note 删除操作会移动后续元素以填补空缺
   */
  lboolean (*remove)(Array *array, ull index);
  /**
   * 替换index位置的元素
   * @param array 数组指针
   * @param index 索引
   * @param data 新元素
   * @return true成功，false失败
   * @warning 同时使用迭代器和替换操作是安全的，但是可能导致迭代器的元素地址不是最新地址
   */
  lboolean (*replace)(Array *array, ull index, UniversalData data);
  /**
   * 获取最后一个元素
   * @param array 数组指针
   * @return 元素副本
   * @note 推荐使用 at() 函数获取元素地址
   */
  UniversalData (*back)(Array *array);
  /**
   * 获取数组的底层数据指针
   * @param array 数组指针
   * @return 底层数据指针
   * @warning 生命周期与Array相同，修改数据将会修改Array内部数据
   */
  UniversalData* (*data)(Array* array);

  // Iterator迭代器相关接口
  /**
   * 创建一个迭代器，迭代器指向index位置的元素
   * @param array 数组指针
   * @param index 索引
   * @return 迭代器指针
   */
  Iterator (*iterator)(Array *array, ull *index);
  /**
   * 创建一个迭代器，迭代器指向第一个元素
   * @param array 数组指针
   * @return 迭代器指针
   */
  Iterator (*begin)(Array *array);
  /**
   * 创建一个迭代器，迭代器指向最后一个元素的下一个位置（即迭代结束位置）
   * @param array 数组指针
   * @return 迭代器指针
   */
  Iterator (*end)(Array *array);
} ArrayApi;

typedef struct Array
{
  const UniversalData *data; // 数据指针(动态数组)
  const ArrayApi *functions;
} Array;
#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * 创建一个动态数组
   * @param dataSet 初始数据集
   * @param length 初始数据集长度
   * @return 数组指针
   */
  Array *Array_Create(UniversalData dataSet[], ull length);
  /**
   * 获取数组API指针
   * @return 数组API指针
   */
  const ArrayApi *Array_GetApi();
  
  /**
   * 内部接口
   */
  lboolean Array_Destroy(Array *array);
  UniversalData Array_Get(Array *array, ull index);
  lboolean Array_Set(Array *array, ull index, UniversalData data);
  UniversalData *Array_At(Array *array, ull index);
  ull Array_Length(Array *array);
  ull Array_Capacity(Array *array);
  lboolean Array_IsEmpty(Array *array);
  lboolean Array_Clear(Array *array);
  lboolean Array_Insert(Array *array, ull index, UniversalData data);
  lboolean Array_Append(Array *array, UniversalData data);
  lboolean Array_Replace(Array *array, ull index, UniversalData data);
  lboolean Array_Remove(Array *array, ull index);
  UniversalData Array_Back(Array *array);
  /**
   * 获取数组的底层数据指针
   * @param array 数组指针
   * @return 底层数据指针
   * @warning 生命周期与Array相同，修改数据将会修改Array内部数据
   */
  UniversalData* Array_Data(Array* array);

  // Iterator相关接口
  Iterator Array_Iterator(Array *array, ull *index);
  Iterator Array_Begin(Array *array);
  Iterator Array_End(Array *array);

#ifdef __cplusplus
}
#endif

#endif // ARRAY_H