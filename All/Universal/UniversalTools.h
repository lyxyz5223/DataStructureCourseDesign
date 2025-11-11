#ifndef UNIVERSALTOOLS_H
#define UNIVERSALTOOLS_H
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <intrin.h>
#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#include <intrin.h>
static inline ull __builtin_clzll(ull x) // 定义MSVC版本的clzll
{
  unsigned long index;
  if (_BitScanReverse64(&index, x))
    return sizeof(x) * 8 - 1 - index;
  else
    return sizeof(x) * 8; // 如果x为0，返回64
}
static inline ull __builtin_popcountll(ull x) // 定义MSVC版本的popcountll
{
  return __popcnt64(x);
}
#elif defined(__GNUC__) || defined(__clang__)
// 使用GCC或Clang的内置函数
#else
static inline ull __builtin_clzll(ull x) // 如果不是GCC或Clang，提供一个简单的实现
{
  if (x == 0)
    return sizeof(x) * 8;
  ull count = 0;
  for (ull i = sizeof(x) * 8ULL; i > 0; i--)
  {
    if ((x >> (i - 1)) & 1)
      break;
    count++;
  }
  return count;
}
static inline ull __builtin_popcountll(ull x) // 简单实现
{
  ull count = 0;
  while (x)
  {
    x &= (x - 1); // 清除最低位的1
    count++;
  }
  return count;
}
#endif

  /**
   * 数据交换
   * @param a 数据指针a
   * @param b 数据指针b
   * @param size 数据大小，单位字节
   */
  static inline void swap(void *a, void *b, size_t size)
  {
    void* temp = malloc(size);
    if (!temp) return; // 内存分配失败，直接返回
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
    free(temp);
  }
  /**
   * 计算二进制中前导零的个数
   * @param n 输入数
   * @return 前导零的个数
   */
  static inline ull clz_ull(ull n)
  {
    if (n == 0)
      return sizeof(n) * 8ULL; // 如果n为0，前导零为64
    return __builtin_clzll(n); // 使用GCC内置函数计算前导零
  }

  /**
   * 计算二进制表示中1的个数
   * @param n 输入数
   * @return 1的个数
   */
  static inline ull popcount_ull(ull n)
  {
    return __builtin_popcountll(n);
  }

  /**
   * 计算对于2的对数，向下取整
   * @param n 输入数
   * @param isExact 输出是否为精确的2的幂次方
   * @return floor(log2(n))，n必须大于0
   */
  static inline ull log2ull_floor(ull n, lboolean *isExact)
  {
    assert(n > 0); // 确保n大于0
    ull rst = sizeof(n) * 8ULL - 1 - clz_ull(n);
    if (isExact)
      *isExact = (popcount_ull(n) == 1);
    return rst;
  }
  /**
   * 计算对于2的对数，向上取整
   * @param n 输入数
   * @param isExact 输出是否为精确的2的幂次方
   * @return ceil(log2(n))，n必须大于0
   */
  static inline ull log2ull_ceil(ull n, lboolean *isExact)
  {
    lboolean isExactLocal = false;
    ull rst = log2ull_floor(n, &isExactLocal);
    if (!isExactLocal)
      rst += 1;
    if (isExact)
      *isExact = isExactLocal;
    return rst;
  }


#ifdef __cplusplus
}
#endif

#endif // UNIVERSALTOOLS_H