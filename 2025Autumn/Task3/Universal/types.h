#ifndef UNIVERSAL_TYPES_H
#define UNIVERSAL_TYPES_H
#include <stdbool.h>

typedef char lboolean;
#define false 0
#define true 1
/**
 * 定义NULL
 * @note 某些编译器可能没有定义NULL
 * @note C23标准中，NULL被定义为宏，表示空指针常量，通常定义为0或((void*)0)
 */
#ifndef NULL
#define NULL 0
#endif

#define null 0
typedef unsigned long long ull;
typedef long long ll;
typedef void* UniversalData;
/**
 * 定义比较函数类型
 * 返回值：0表示相等，>0表示a大于b，<0表示a小于b
 */
typedef int (*Comparator)(const UniversalData* a, const UniversalData* b);
typedef int (*ConstComparator)(const void* a, const void* b);

/**
 * 迭代器结构
 */
typedef struct Iterator
{
  UniversalData *dataPtr;                                                  // 指向当前元素的指针，data为空表示迭代结束
  void (*next)(struct Iterator *iter);                                     // 迭代器到下一个元素，如果data为空则表示迭代结束
  lboolean (*hasNext)(struct Iterator *iter);                               // 判断是否还有下一个元素
  lboolean (*equals)(struct Iterator *iterA, struct Iterator *iterB);      // 比较两个迭代器是否相等
  void (*destroy)(struct Iterator *iter);                                  // 销毁迭代器
} *Iterator;


#endif // UNIVERSAL_TYPES_H