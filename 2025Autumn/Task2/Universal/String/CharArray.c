#include "CharArray.h"
#include <stdlib.h>
#include <memory.h> // memset, memcpy

/**
 * 定义从固定分配到加倍分配的临界点
 * 如：256
 * 此时：200 220 240 260 520 1040
 */
#define MinQuickMalloc 256
#define MallocStep 20
#define ReservedLength 10
// 一些声明
static CharArrayApi _CharArray_Global_Api;
typedef struct _Iterator *_Iterator;
// 一些定义
typedef struct _CharArray {
  CharArrayApi *functions;
  char *data; // 数据指针(动态数组)
  ull length;
  ull capacity;
  _Iterator iteratorEnd; // 尾部的迭代器
} _CharArray;
typedef struct _Iterator {
  char *dataPtr;                                            // 指向当前元素的指针，data为空表示迭代结束
  void (*next)(struct Iterator *iter);                               // 迭代器到下一个元素，如果data为空则表示迭代结束
  lboolean (*hasNext)(struct Iterator *iter);                         // 判断是否还有下一个元素
  lboolean (*equals)(struct Iterator *iterA, struct Iterator *iterB); // 比较两个迭代器是否相等
  void (*destroy)(struct Iterator *iter);                            // 销毁迭代器
  // 下面是基于Iterator扩展的成员，用于内部函数的实现
  _CharArray *array;
  ull currentIndex; // 当前索引
} *_Iterator;


/**
 * 内部函数
 * 扩容
 * @param arr 数组指针
 * @return true 成功，false 失败
 */
lboolean _CharArray_GrowUp(_CharArray* arr)
{
  if (!arr)
    return false;
  if (arr->capacity < MinQuickMalloc)
    arr->capacity += MallocStep;
  else
    arr->capacity *= 2;
  char* newData = (char*)realloc(arr->data, sizeof(char) * arr->capacity);
  if (!newData)
    return false;
  arr->data = newData;
  return true;
}

lboolean _CharArray_Memory_Zero(_CharArray* arr)
{
  if (!arr)
    return false;
  arr->data = memset(arr->data, 0, arr->capacity * sizeof(char));
  return true;
}

/**
 * 插入元素前的检测是否有充足空间，如果没有则需要扩容
 * @param arr 数组指针
 * @param insertCount 插入的元素数量
 * @return true 内存充足，false 需要扩容 
 */
lboolean _CharArray_CheckIfArrayEnoughForInsertion(_CharArray *arr, ull insertCount)
{
  if (arr->length + insertCount + ReservedLength >= arr->capacity)
    return false;
  return true;
}
/**
 * 插入前的自动数组内存充足性检测并在不足时进行扩容操作
 * @param arr 数组指针
 * @param insertCount 插入的元素数量
 * @return true 扩容成功或不需要扩容，false 扩容失败
 */
lboolean _CharArray_BeforeInsertAutoGrowUp(_CharArray* arr, ull insertCount)
{
  if (!arr)
    return false;
  while (!_CharArray_CheckIfArrayEnoughForInsertion(arr, insertCount))
    if (!_CharArray_GrowUp(arr))
      return false;
  return true;
}

// Iterator相关内部接口
/**
 * 内部函数
 * 迭代器前进到下一个元素
 */
void _CharArray_Iterator_Next(Iterator iter)
{
  if (!iter)
    return;
  _Iterator _iter = (_Iterator)iter;
  // 检查当前索引是否越界
  _iter->currentIndex++;
  if (_iter->currentIndex >= _iter->array->length)
  {
    _iter->dataPtr = NULL;
    return;
  }
  _iter->dataPtr = &_iter->array->data[_iter->currentIndex];
}
/**
 * 内部函数
 * 判断是否还有下一个元素
 * @param iter 迭代器
 * @return true 有下一个元素，false 没有下一个元素
 */
lboolean _CharArray_Iterator_HasNext(Iterator iter)
{
  if (!iter)
    return false;
  _Iterator _iter = (_Iterator)iter;
  return _iter->currentIndex + 1 < _iter->array->length;
}
/**
 * 内部函数
 * 比较两个迭代器是否相等
 * @note 目前仅比较数组地址和索引
 */
lboolean _CharArray_Iterator_Equals(Iterator iterA, Iterator iterB)
{
  if (!iterA || !iterB)
    return false;
  _Iterator _iterA = (_Iterator)iterA;
  _Iterator _iterB = (_Iterator)iterB;
  return _iterA->array == _iterB->array                   // 必须来自同一个数组
         && _iterA->dataPtr == _iterB->dataPtr; // 且存在数据
}
/**
 * 内部函数
 * 销毁迭代器
 * @param iter 迭代器
 */
void _CharArray_Iterator_Destroy(Iterator iter)
{
  if (!iter)
    return;
  free(iter);
}
/**
 * 内部函数
 * 创建一个新的迭代器
 */
_Iterator _CharArray_Iterator_Create(_CharArray *arr, ull index)
{
  _Iterator iter = (_Iterator)malloc(sizeof(struct _Iterator));
  iter->dataPtr = index < arr->length ? &arr->data[index] : NULL;
  iter->next = _CharArray_Iterator_Next;
  iter->hasNext = _CharArray_Iterator_HasNext;
  iter->equals = _CharArray_Iterator_Equals;
  iter->destroy = _CharArray_Iterator_Destroy;
  iter->array = arr;
  iter->currentIndex = index < arr->length ? index : arr->length; // 如果index越界，则指向结束位置
  return iter;
}


CharArray* CharArray_Create(char dataSet[], ull length)
{
  _CharArray* arr = (_CharArray*)malloc(sizeof(_CharArray));
  if (!arr || length < 0)
    return 0;
  arr->length = length;
  arr->capacity = length > MinQuickMalloc ? length * 2 : length + MallocStep;
  arr->data = (char*)malloc(sizeof(char) * arr->capacity);
  arr->functions = &_CharArray_Global_Api;
  if (dataSet)
    for (ull i = 0; i < length; i++)
      arr->data[i] = dataSet[i];
  arr->iteratorEnd = _CharArray_Iterator_Create(arr, arr->length);
  return (CharArray *)arr;
}

lboolean CharArray_Destroy(CharArray *arr)
{
  if (!arr)
    return false;
  _CharArray *array = (_CharArray *)arr;
  if (array->data)
    free(array->data);
  free(array);
  return true;
}

char CharArray_Get(CharArray *array, ull index)
{
  if (!array)
    return NULL;
  _CharArray* arr = (_CharArray*)array;
  if (index >= arr->length)
    return NULL;
  return arr->data[index];
}

char* CharArray_At(CharArray *array, ull index)
{
  if (!array)
    return NULL;
  _CharArray* arr = (_CharArray*)array;
  if (index >= arr->length)
    return NULL;
  return &arr->data[index];
}

ull CharArray_Length(CharArray* array)
{
  if (!array)
    return 0;
  _CharArray *arr = (_CharArray *)array;
  return arr->length;
}

ull CharArray_Capacity(CharArray* array)
{
  if (!array)
    return 0;
  _CharArray *arr = (_CharArray *)array;
  return arr->capacity;
}

lboolean CharArray_IsEmpty(CharArray *array)
{
  if (!array)
    return true;
  _CharArray *arr = (_CharArray *)array;
  return arr->length == 0;
}

lboolean CharArray_Clear(CharArray *array)
{
  if (!array)
    return false;
  _CharArray *arr = (_CharArray *)array;
  arr->capacity = 0;
  arr->length = 0;
  _CharArray_GrowUp(arr); // 重置为初始容量
  _CharArray_Memory_Zero(arr); // 清零
  return true;
}

lboolean CharArray_Insert(CharArray *array, ull index, char data)
{
  if (!array)
    return false;
  _CharArray *arr = (_CharArray *)array;
  if (index > arr->length) // 允许在末尾插入
    return false;
  if (!_CharArray_BeforeInsertAutoGrowUp(arr, 1) && arr->length + 1 > arr->capacity)
    return false; // 自动分配失败并且剩余空间不足以插入元素
  for (ull i = arr->length; i > index; i--)
    arr->data[i] = arr->data[i - 1];
  arr->data[index] = data;
  arr->length++;
  return true;
}

lboolean CharArray_InsertMultiple(CharArray *array, ull index, const char data[], ull count)
{
  if (!array || !data || count <= 0)
    return false;
  _CharArray *arr = (_CharArray *)array;
  if (index > arr->length) // 允许在末尾插入
    return false;
  if (!_CharArray_BeforeInsertAutoGrowUp(arr, count) && arr->length + count > arr->capacity)
    return false; // 自动分配失败并且剩余空间不足以插入元素
  for (ull i = arr->length + count - 1; i >= index + count; i--)
    arr->data[i] = arr->data[i - count];
  for (ull i = 0; i < count; i++)
    arr->data[index + i] = data[i];
  arr->length += count;
  return true;
}

lboolean CharArray_Append(CharArray *array, char data)
{
  return CharArray_Insert(array, ((_CharArray *)array)->length, data);
}

lboolean CharArray_AppendMultiple(CharArray *array, const char data[], ull count)
{
  return CharArray_InsertMultiple(array, ((_CharArray*)array)->length, data, count);
}

lboolean CharArray_Replace(CharArray *array, ull index, char data)
{
  if (!array)
    return false;
  _CharArray *arr = (_CharArray *)array;
  if (index >= arr->length)
    return false;
  arr->data[index] = data;
  return true;
}

lboolean CharArray_ReplaceRange(CharArray *array, ull index, ull count, const char data[], ull dataCount)
{
  _CharArray *arr = (_CharArray *)array;
  if (!array || !data || index >= arr->length)
    return false;
  if (index + count > arr->length) // 判断需要被替换的范围是否超出了数组边界
    count = arr->length - index; // 取小的那个
  for (ull i = 0; i < dataCount && i < count; i++) // 共同数量内进行替换操作
    arr->data[index + i] = data[i];
  if (count > dataCount) // 如果选中的范围中的数量 > 替换的数据数量
  {
    // 删除多余元素
    return CharArray_RemoveRange(array, index + dataCount, count - dataCount);
  }
  else // 如果选中的范围中的数量 <= 替换的数据数量
  {
    // 插入未插入的元素
    return CharArray_InsertMultiple(array, index + dataCount, data + dataCount, dataCount - count);
  }
  return true;
}

lboolean CharArray_Remove(CharArray *array, ull index)
{
  if (!array)
    return false;
  _CharArray *arr = (_CharArray *)array;
  if (index >= arr->length)
    return false;
  for (ull i = index; i < arr->length - 1; i++)
    arr->data[i] = arr->data[i + 1];
  arr->length--;
  return true;
}

lboolean CharArray_RemoveRange(CharArray *array, ull index, ull count)
{
  if (!array || count <= 0)
    return false;
  _CharArray *arr = (_CharArray *)array;
  if (index >= arr->length)
    return false;
  if (index + count > arr->length) // 判断需要被删除的范围是否超出了数组边界
    count = arr->length - index; // 取小的那个
  for (ull i = index; i < arr->length - count; i++)
    arr->data[i] = arr->data[i + count];
  arr->length -= count;
  return true;
}

char CharArray_Back(CharArray *array)
{
  if (!array)
    return NULL;
  _CharArray *arr = (_CharArray *)array;
  if (arr->length == 0)
    return NULL;
  return arr->data[arr->length - 1];
}


// Iterator相关接口
Iterator CharArray_Iterator(CharArray *array, ull *index)
{
  if (!array || !index)
    return NULL;
  _CharArray *arr = (_CharArray *)array;
  if ((*index) >= array->functions->length(array)) // 如果索引超出范围，
    return (Iterator)arr->iteratorEnd; // 返回尾部迭代器
  char *current = &arr->data[*index];
  (*index)++;
  return (Iterator)_CharArray_Iterator_Create(arr, *index);
}

Iterator CharArray_Begin(CharArray *array)
{
  if (!array)
    return NULL;
  return (Iterator)_CharArray_Iterator_Create((_CharArray *)array, 0);
}
Iterator CharArray_End(CharArray *array)
{
  if (!array)
    return NULL;
  _CharArray *arr = (_CharArray *)array;
  return (Iterator)arr->iteratorEnd;
}

static CharArrayApi _CharArray_Global_Api = {
  .destroy = CharArray_Destroy,
  .get = CharArray_Get,
  .at = CharArray_At,
  .length = CharArray_Length,
  .capacity = CharArray_Capacity,
  .isEmpty = CharArray_IsEmpty,
  .clear = CharArray_Clear,
  .insert = CharArray_Insert,
  .insertMultiple = CharArray_InsertMultiple,
  .append = CharArray_Append,
  .appendMultiple = CharArray_AppendMultiple,
  .remove = CharArray_Remove,
  .removeRange = CharArray_RemoveRange,
  .replace = CharArray_Replace,
  .replaceRange = CharArray_ReplaceRange,
  .back = CharArray_Back,
  .iterator = CharArray_Iterator,
  .begin = CharArray_Begin,
  .end = CharArray_End
};
