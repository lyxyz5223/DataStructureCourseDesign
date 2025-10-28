#include "Array.h"
#include <stdlib.h>
/**
 * 定义从固定分配到加倍分配的临界点
 * 如：256
 * 此时：200 220 240 260 520 1040
 */
#define MinQuickMalloc 256
#define MallocStep 20
#define ReservedLength 10
// 一些声明
static ArrayApi _Array_Global_Api;
typedef struct _Iterator *_Iterator;
// 一些定义
typedef struct _Array {
  UniversalData *data; // 数据指针(动态数组)
  ArrayApi *functions;
  ull length;
  ull capacity;
  _Iterator iteratorEnd; // 尾部的迭代器
} _Array;
typedef struct _Iterator {
  UniversalData *dataPtr;                                            // 指向当前元素的指针，data为空表示迭代结束
  void (*next)(struct Iterator *iter);                               // 迭代器到下一个元素，如果data为空则表示迭代结束
  lboolean (*hasNext)(struct Iterator *iter);                         // 判断是否还有下一个元素
  lboolean (*equals)(struct Iterator *iterA, struct Iterator *iterB); // 比较两个迭代器是否相等
  void (*destroy)(struct Iterator *iter);                            // 销毁迭代器
  // 下面是基于Iterator扩展的成员，用于内部函数的实现
  _Array *array;
  ull currentIndex; // 当前索引
} *_Iterator;

/**
 * 内部函数
 * 自动扩容
 * @param arr 数组指针
 */
void _Array_AutoRealloc(_Array* arr)
{
  if (!arr)
    return;
  if (arr->length + ReservedLength >= arr->capacity)
  {
    // 剩余内存少于预存长度，进行扩容
    if (arr->capacity < MinQuickMalloc)
      arr->capacity += MallocStep;
    else
      arr->capacity *= 2;
    arr->data = (UniversalData*)realloc(arr->data, sizeof(UniversalData) * arr->capacity);
  }
}

// Iterator相关内部接口
/**
 * 内部函数
 * 迭代器前进到下一个元素
 */
void _Array_Iterator_Next(Iterator iter)
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
lboolean _Array_Iterator_HasNext(Iterator iter)
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
lboolean _Array_Iterator_Equals(Iterator iterA, Iterator iterB)
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
void _Array_Iterator_Destroy(Iterator iter)
{
  if (!iter)
    return;
  free(iter);
}
/**
 * 内部函数
 * 创建一个新的迭代器
 */
_Iterator _Array_Iterator_Create(_Array *arr, ull index)
{
  _Iterator iter = (_Iterator)malloc(sizeof(struct _Iterator));
  iter->dataPtr = index < arr->length ? &arr->data[index] : NULL;
  iter->next = _Array_Iterator_Next;
  iter->hasNext = _Array_Iterator_HasNext;
  iter->equals = _Array_Iterator_Equals;
  iter->destroy = _Array_Iterator_Destroy;
  iter->array = arr;
  iter->currentIndex = index < arr->length ? index : arr->length; // 如果index越界，则指向结束位置
  return iter;
}


Array* Array_Create(UniversalData dataSet[], ull length)
{
  _Array* arr = (_Array*)malloc(sizeof(_Array));
  if (!arr || length < 0)
    return 0;
  arr->length = length;
  arr->capacity = length > MinQuickMalloc ? length * 2 : length + MallocStep;
  arr->data = (UniversalData*)malloc(sizeof(UniversalData) * arr->capacity);
  arr->functions = &_Array_Global_Api;
  if (dataSet)
    for (ull i = 0; i < length; i++)
      arr->data[i] = dataSet[i];
  arr->iteratorEnd = _Array_Iterator_Create(arr, arr->length);
  return (Array *)arr;
}

const ArrayApi* Array_GetApi()
{
  return &_Array_Global_Api;
}

lboolean Array_Destroy(Array *arr)
{
  if (!arr)
    return false;
  _Array *array = (_Array *)arr;
  if (array->data)
    free(array->data);
  free(array);
  return true;
}

UniversalData Array_Get(Array *array, ull index)
{
  if (!array)
    return NULL;
  _Array* arr = (_Array*)array;
  if (index >= arr->length)
    return NULL;
  return arr->data[index];
}

UniversalData* Array_At(Array *array, ull index)
{
  if (!array)
    return NULL;
  _Array* arr = (_Array*)array;
  if (index >= arr->length)
    return NULL;
  return &arr->data[index];
}

lboolean Array_Set(Array *array, ull index, UniversalData data)
{
  if (!array)
    return false;
  _Array *arr = (_Array *)array;
  if (index >= arr->length)
    return false;
  arr->data[index] = data;
  return true;
}

ull Array_Length(Array* array)
{
  if (!array)
    return 0;
  _Array *arr = (_Array *)array;
  return arr->length;
}

ull Array_Capacity(Array* array)
{
  if (!array)
    return 0;
  _Array *arr = (_Array *)array;
  return arr->capacity;
}

lboolean Array_IsEmpty(Array *array)
{
  if (!array)
    return true;
  _Array *arr = (_Array *)array;
  return arr->length == 0;
}

lboolean Array_Clear(Array *array)
{
  if (!array)
    return false;
  _Array *arr = (_Array *)array;
  arr->length = 0;
  return true;
}

lboolean Array_Insert(Array *array, ull index, UniversalData data)
{
  if (!array || !data)
    return false;
  _Array *arr = (_Array *)array;
  if (index > arr->length) // 允许在末尾插入
    return false;
  _Array_AutoRealloc(arr);
  for (ull i = arr->length; i > index; i--)
    arr->data[i] = arr->data[i - 1];
  arr->data[index] = data;
  arr->length++;
  return true;
}

lboolean Array_Append(Array *array, UniversalData data)
{
  if (!array || !data)
    return false;
  _Array *arr = (_Array *)array;
  _Array_AutoRealloc(arr);
  arr->data[arr->length] = data;
  arr->length++;
  return true;
}

lboolean Array_Replace(Array *array, ull index, UniversalData data)
{
  if (!array || !data)
    return false;
  _Array *arr = (_Array *)array;
  if (index >= arr->length)
    return false;
  arr->data[index] = data;
  return true;
}

lboolean Array_Remove(Array *array, ull index)
{
  if (!array)
    return false;
  _Array *arr = (_Array *)array;
  if (index >= arr->length)
    return false;
  for (ull i = index; i < arr->length - 1; i++)
    arr->data[i] = arr->data[i + 1];
  arr->length--;
  return true;
}

UniversalData Array_Back(Array *array)
{
  if (!array)
    return NULL;
  _Array *arr = (_Array *)array;
  if (arr->length == 0)
    return NULL;
  return arr->data[arr->length - 1];
}

UniversalData *Array_Data(Array *array)
{
  if (!array)
    return NULL;
  _Array *arr = (_Array *)array;
  return arr->data;
}

// Iterator相关接口
Iterator Array_Iterator(Array *array, ull *index)
{
  if (!array || !index)
    return NULL;
  _Array *arr = (_Array *)array;
  if ((*index) >= array->functions->length(array)) // 如果索引超出范围，
    return (Iterator)arr->iteratorEnd; // 返回尾部迭代器
  UniversalData *current = &arr->data[*index];
  (*index)++;
  return (Iterator)_Array_Iterator_Create(arr, *index);
}

Iterator Array_Begin(Array *array)
{
  if (!array)
    return NULL;
  return (Iterator)_Array_Iterator_Create((_Array *)array, 0);
}
Iterator Array_End(Array *array)
{
  if (!array)
    return NULL;
  _Array *arr = (_Array *)array;
  return (Iterator)arr->iteratorEnd;
}

static ArrayApi _Array_Global_Api = {
  .destroy = Array_Destroy,
  .set = Array_Set,
  .get = Array_Get,
  .at = Array_At,
  .length = Array_Length,
  .capacity = Array_Capacity,
  .isEmpty = Array_IsEmpty,
  .clear = Array_Clear,
  .insert = Array_Insert,
  .append = Array_Append,
  .remove = Array_Remove,
  .replace = Array_Replace,
  .back = Array_Back,
  .data = Array_Data,

  // Iterator相关接口
  .iterator = Array_Iterator,
  .begin = Array_Begin,
  .end = Array_End
};
