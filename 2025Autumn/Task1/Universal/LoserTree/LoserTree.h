#ifndef LOSERTREE_H
#define LOSERTREE_H

#include "../types.h"
typedef struct LoserTree {

} *LoserTree;

typedef int LoserTree_ElementType;
typedef ull LoserTree_SizeType;

#ifdef __cplusplus
extern "C"
{
#endif
  /**
   * 创建败者树
   * @param elements 叶子节点元素数组
   * @param elemSize 元素大小，单位字节
   * @param count 叶子节点数量
   * @return 败者树指针，失败返回NULL
   * @note 元素数组内容会被复制到败者树内部，作为初始元素
   */
  LoserTree LoserTree_Create(LoserTree_ElementType *elements, LoserTree_SizeType elemSize, LoserTree_SizeType count, ConstComparator cmp);

  /**
   * 销毁败者树
   * @param lt 败者树
   */
  void LoserTree_Destroy(LoserTree lt);

  /**
   * 替换败者节点的元素，并调整树
   * @param lt 败者树
   * @param element 新元素
   */
  void LoserTree_ReplaceLoser(LoserTree lt, LoserTree_ElementType element);

  /**
   * 获取当前败者元素指针
   * @param lt 败者树
   * @param outIndex 输出败者元素索引指针，可以为NULL
   * @return 当前败者节点的元素，失败返回0
   */
  const LoserTree_ElementType *LoserTree_GetLoserPtr(LoserTree lt, LoserTree_SizeType *outIndex);

  /**
   * 获取当前败者元素值
   * @param lt 败者树
   * @param outIndex 输出败者元素索引指针，可以为NULL
   * @return 当前败者节点的元素值
   */
  LoserTree_ElementType LoserTree_GetLoserValue(LoserTree lt, LoserTree_SizeType *outIndex);

  /**
   * 按照失败者顺序弹出一次败者元素
   * @param lt 败者树
   * @param outIndex 输出败者元素索引指针，可以为NULL
   * @return 当前败者元素值
   */
  LoserTree_ElementType LoserTree_PopLoser(LoserTree lt, LoserTree_SizeType *outIndex);

  /**
   * 获取当前元素列表
   * @param lt 败者树
   * @param outCount 输出元素数量指针，可以为NULL
   * @return 元素列表指针，失败返回NULL
   * @note 返回的数组长度可能小于败者树的叶子节点数量
   * @note 调用者不应修改返回的数组内容，不应该释放返回的指针
   * @note 返回的指针在败者树被销毁后失效
   */
  const LoserTree_ElementType *LoserTree_GetElements(LoserTree lt, LoserTree_SizeType *outCount);

#ifdef __cplusplus
}
#endif
#endif // LOSERTREE_H