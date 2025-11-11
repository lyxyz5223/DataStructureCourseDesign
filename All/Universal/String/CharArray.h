#ifndef CHAR_ARRAY_H
#define CHAR_ARRAY_H
#include "../types.h"

typedef struct CharArray CharArray;
typedef struct CharArrayApi
{
  lboolean (*destroy)(CharArray *array);
  /**
   * 获取index位置的元素
   * @note 推荐使用 at() 函数获取元素地址
   * @param array 数组指针
   * @param index 索引
   * @return 元素副本
   */
  char (*get)(CharArray *array, ull index);
  /**
   * 返回index位置的元素引用
   * @param array 数组指针
   * @param index 索引
   * @return 元素地址
   */
  char *(*at)(CharArray *array, ull index);
  /**
   * 获取数组长度
   * @param array 数组指针
   * @return 数组长度
   */
  ull (*length)(CharArray *array);
  /**
   * 获取数组容量
   * @param array 数组指针
   * @return 数组容量
   */
  ull (*capacity)(CharArray *array);
  /**
   * 判断数组是否为空
   * @param array 数组指针
   * @return true为空，false不为空
   */
  lboolean (*isEmpty)(CharArray *array);
  /**
   * 清空数组
   * @param array 数组指针
   * @return true成功，false失败
   * @note 清空操作不会释放内存，只会将长度置0
   */
  lboolean (*clear)(CharArray *array);
  /**
   * 在index位置插入一个元素，原index及其后续元素后移
   * @param array 数组指针
   * @param index 索引
   * @param data 元素
   * @return true成功，false失败
   * @warning 请不要同时使用迭代器和插入操作，否则可能由于数组内存重新分配导致迭代器失效
   * @note 可能导致数组重新分配内存
   */
  lboolean (*insert)(CharArray *array, ull index, char data);
  /**
   * 在index位置插入多个元素，原index及其后续元素后移
   * @param array 数组指针
   * @param index 索引
   * @param data 元素数组
   * @param count 元素数量
   * @return true成功，false失败
   * @warning 请不要同时使用迭代器和插入操作，否则可能由于数组内存重新分配导致迭代器失效
   * @note 可能导致数组重新分配内存
   */
  lboolean (*insertMultiple)(CharArray *array, ull index, const char data[], ull count);
  
  /**
   * 在数组末尾添加一个元素
   * @param array 数组指针
   * @param data 元素
   * @return true成功，false失败
   * @warning 请不要同时使用迭代器和插入操作，否则可能由于数组内存重新分配导致迭代器失效
   * @note 可能导致数组重新分配内存
   */
  lboolean (*append)(CharArray *array, char data);
  /**
   * 在数组末尾添加多个元素
   * @param array 数组指针
   * @param data 元素数组
   * @param count 元素数量
   * @return true成功，false失败
   */
  lboolean (*appendMultiple)(CharArray *array, const char data[], ull count);

  /**
   * 删除index位置的元素
   * @param array 数组指针
   * @param index 索引
   * @return true成功，false失败
   * @warning 请不要同时使用迭代器和删除操作，否则可能导致迭代器跳过n个元素（n为删除个数）
   * @note 删除操作会移动后续元素以填补空缺
   */
  lboolean (*remove)(CharArray *array, ull index);
  /**
   * 删除范围内的元素
   * @param array 数组指针
   * @param index 起始索引
   * @param count 删除个数
   * @return true成功，false失败
   * @warning 请不要同时使用迭代器和删除操作，否则可能导致迭代器跳过n个元素（n为删除个数）
   * @note 删除操作会移动后续元素以填补空缺
   */
  lboolean (*removeRange)(CharArray *array, ull index, ull count);
  /**
   * 替换index位置的元素
   * @param array 数组指针
   * @param index 索引
   * @param data 新元素
   * @return true成功，false失败
   * @warning 同时使用迭代器和替换操作是安全的，但是可能导致迭代器的元素地址不是最新地址
   */
  lboolean (*replace)(CharArray *array, ull index, char data);
  /**
   * 替换范围内的元素
   * @param array 数组指针
   * @param index 起始索引
   * @param count 替换个数
   * @param data 新元素数组，数量可以大于count
   * @return true成功，false失败
   * @warning 同时使用迭代器和替换操作可能会导致迭代器失效，可能存在重新分配内存
   */
  lboolean (*replaceRange)(CharArray *array, ull index, ull count, const char data[], ull dataCount);

  /**
   * 获取最后一个元素
   * @param array 数组指针
   * @return 元素副本
   * @note 推荐使用 at() 函数获取元素地址
   */
  char (*back)(CharArray *array);

  // Iterator迭代器相关接口
  /**
   * 创建一个迭代器，迭代器指向index位置的元素
   * @param array 数组指针
   * @param index 索引
   * @return 迭代器指针
   */
  Iterator (*iterator)(CharArray *array, ull index);
  /**
   * 创建一个迭代器，迭代器指向第一个元素
   * @param array 数组指针
   * @return 迭代器指针
   */
  Iterator (*begin)(CharArray *array);
  /**
   * 创建一个迭代器，迭代器指向最后一个元素的下一个位置（即迭代结束位置）
   * @param array 数组指针
   * @return 迭代器指针
   */
  Iterator (*end)(CharArray *array);
} CharArrayApi;

typedef struct CharArray
{
  const CharArrayApi *functions;
  const char *data; // 数据指针(动态数组)
} CharArray;
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
  CharArray *CharArray_Create(char dataSet[], ull length);
  /**
   * 内部接口
   */
  lboolean CharArray_Destroy(CharArray *array);
  char CharArray_Get(CharArray *array, ull index);
  char *CharArray_At(CharArray *array, ull index);
  ull CharArray_Length(CharArray *array);
  ull CharArray_Capacity(CharArray *array);
  lboolean CharArray_IsEmpty(CharArray *array);
  lboolean CharArray_Clear(CharArray *array);
  lboolean CharArray_Insert(CharArray *array, ull index, char data);
  lboolean CharArray_InsertMultiple(CharArray *array, ull index, const char data[], ull count);
  lboolean CharArray_Append(CharArray *array, char data);
  lboolean CharArray_AppendMultiple(CharArray *array, const char data[], ull count);
  lboolean CharArray_Replace(CharArray *array, ull index, char data);
  lboolean CharArray_ReplaceRange(CharArray *array, ull index, ull count, const char data[], ull dataCount);
  lboolean CharArray_Remove(CharArray *array, ull index);
  lboolean CharArray_RemoveRange(CharArray *array, ull index, ull count);
  char CharArray_Back(CharArray *array);

  // Iterator相关接口
  Iterator CharArray_Iterator(CharArray *array, ull index);
  Iterator CharArray_Begin(CharArray *array);
  Iterator CharArray_End(CharArray *array);

#ifdef __cplusplus
}
#endif

#endif // CHAR_ARRAY_H
