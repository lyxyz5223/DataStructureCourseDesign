#include "LoserTree.h"
#include <stdlib.h>
#include <memory.h>
#include "../UniversalTools.h"
#include <math.h>
#define SIZETYPE_MAX ((LoserTree_SizeType)(~(LoserTree_SizeType)0))
typedef struct _LoserTree_Node
{
  LoserTree_ElementType *elem; // 元素指针
  LoserTree_SizeType elemIdx; // 元素索引
  LoserTree_SizeType parent;   // 父节点，失败者
  LoserTree_SizeType opponent; // 对手节点
} _LoserTree_Node;

typedef struct _LoserTree {
  _LoserTree_Node *nodes; // 败者树节点数组
  LoserTree_ElementType *elems; // 叶子节点元素数组
  ConstComparator cmp;       // 比较函数
  LoserTree_SizeType size;     // 叶子节点数量
  LoserTree_SizeType capacity; // 节点数组容量
  LoserTree_SizeType depth;    // 树的深度
  LoserTree_SizeType lastLevelStartIndex; // 最后一层节点的起始索引
  LoserTree_SizeType lastLevelEndIndex;   // 最后一层节点的结束索引
  LoserTree_ElementType *elemsOut; // 输出数组
} _LoserTree;

_LoserTree_Node* _LoserTree_Compare(ConstComparator cmp, _LoserTree_Node* a, _LoserTree_Node* b) {
  if (!a->elem)
    return b;
  else if (!b->elem)
    return a;
  int rst = cmp(a->elem, b->elem);
  if (rst <= 0)
    return a;
  else
    return b;
}


LoserTree LoserTree_Create(LoserTree_ElementType* elements, LoserTree_SizeType elemSize, LoserTree_SizeType count, ConstComparator cmp)
{
  if (elemSize <= 0 || count <= 0 || !elements)
    return NULL;
  assert(count > 1 && "LoserTree_Create: count must be greater than 1");
  if (count == 1)
    return NULL; // 只有一个元素，不需要败者树
  _LoserTree *tree = (_LoserTree *)malloc(sizeof(struct _LoserTree));
  if (!tree)
    return NULL;
  tree->cmp = cmp;
  tree->size = count;
  tree->elems = (LoserTree_ElementType *)malloc(elemSize * count);
  tree->elemsOut = NULL;
  if (!tree->elems) {
    free(tree);
    return NULL;
  }
  memcpy(tree->elems, elements, elemSize * count);
  // 计算树的深度（根节点为0）
  ull depth = log2ull_ceil(count, NULL);
  tree->depth = depth;
  // 计算完全二叉树的节点总数
  ull totalNodes = (1ULL << (depth + 1)) - 1;
  tree->capacity = totalNodes;
  // 创建节点
  tree->nodes = (_LoserTree_Node *)malloc(sizeof(_LoserTree_Node) * totalNodes);
  if (!tree->nodes) {
    free(tree);
    return NULL;
  }
  memset(tree->nodes, 0, sizeof(_LoserTree_Node) * totalNodes);
  // 初始化节点
  ull startIndex = (1ULL << depth) - 1; // 最底层节点的起始索引，2^depth - 1
  ull lastIndex = startIndex + count - 1; // 最后一个叶子节点的索引
  tree->lastLevelStartIndex = startIndex;
  tree->lastLevelEndIndex = lastIndex;
  // 赋值叶子节点
  for (ull i = totalNodes - 1; i > lastIndex; i--)
  {
    tree->nodes[i].elem = 0; // 空节点
    tree->nodes[i].elemIdx = SIZETYPE_MAX;
    tree->nodes[i].parent = (i - 1) / 2; // 父节点索引计算
    tree->nodes[i].opponent = (i % 2 == 0) ? (i - 1) : (i + 1);
  }
  for (ull i = lastIndex; i >= startIndex; i--)
  {
    tree->nodes[i].elem = &tree->elems[i - startIndex];
    tree->nodes[i].elemIdx = i - startIndex;
    tree->nodes[i].parent = (i - 1) / 2; // 父节点索引计算
    tree->nodes[i].opponent = (i % 2 == 0) ? (i - 1) : (i + 1);
  }
  for (ull i = startIndex - 1; i > 0; i--)
  {
    _LoserTree_Node* tmp = _LoserTree_Compare(tree->cmp, &tree->nodes[i * 2 + 1], &tree->nodes[i * 2 + 2]);
    tree->nodes[i].elem = tmp ? tmp->elem : 0;
    tree->nodes[i].elemIdx = tmp ? tmp->elemIdx : SIZETYPE_MAX;
    tree->nodes[i].parent = (i - 1) / 2; // 父节点索引计算
    tree->nodes[i].opponent = (i % 2 == 0) ? (i - 1) : (i + 1);
  }
  // 根节点
  if (totalNodes > 1)
  {
    // 确保根节点有孩子
    _LoserTree_Node* tmp = _LoserTree_Compare(tree->cmp, &tree->nodes[1], (totalNodes > 2 ? (&tree->nodes[2]) : 0));
    tree->nodes[0].elem = tmp ? tmp->elem : 0; // 根节点元素
    tree->nodes[0].elemIdx = tmp ? tmp->elemIdx : SIZETYPE_MAX;
  }
  tree->nodes[0].parent = 0; // 根节点没有父节点
  tree->nodes[0].opponent = 0; // 根节点没有对手
  return (LoserTree)tree;
}

void LoserTree_Destroy(LoserTree lt)
{
  assert(lt != NULL);
  _LoserTree *tree = (_LoserTree *)lt;
  if (tree->nodes)
    free(tree->nodes);
  if (tree->elems)
    free(tree->elems);
  if (tree->elemsOut)
    free(tree->elemsOut);
  free(tree);
}

void _LoserTree_ReplaceLoser(LoserTree lt, LoserTree_ElementType* element)
{
  assert(lt != NULL);
  _LoserTree *tree = (_LoserTree *)lt;
  // 替换叶子节点
  ull startIndex = tree->lastLevelStartIndex; // 最底层节点的起始索引，2^depth - 1
  ull lastIndex = tree->lastLevelEndIndex; // 最后一个叶子节点的索引
  LoserTree_SizeType loserElemIdx = tree->nodes[0].elemIdx;
  LoserTree_SizeType loserNodeIdx = startIndex + loserElemIdx;
  if (element)
  {
    tree->elems[loserElemIdx] = *element; // 替换叶子节点的元素
  }
  else
  {
    tree->nodes[loserNodeIdx].elem = NULL; // 如果element为NULL，表示删除该节点
    tree->nodes[loserNodeIdx].elemIdx = SIZETYPE_MAX;
  }
  // 从叶子节点开始向上调整
  _LoserTree_Node *current = &tree->nodes[loserNodeIdx];
  while (current->opponent) // 直到没有对手节点
  {
    _LoserTree_Node *parent = &tree->nodes[current->parent];
    _LoserTree_Node *opponent = &tree->nodes[current->opponent];
    _LoserTree_Node *loser = _LoserTree_Compare(tree->cmp, current, opponent);
    parent->elem = loser ? loser->elem : 0;
    parent->elemIdx = loser ? loser->elemIdx : SIZETYPE_MAX;
    current = parent;
  }
}

void LoserTree_ReplaceLoser(LoserTree lt, LoserTree_ElementType element)
{
  _LoserTree_ReplaceLoser(lt, &element);
}

const LoserTree_ElementType *LoserTree_GetLoserPtr(LoserTree lt, LoserTree_SizeType *outIndex)
{
  if (!lt)
    return NULL;
  _LoserTree *tree = (_LoserTree *)lt;
  if (!tree->nodes[0].elem)
    return NULL;
  if (outIndex)
    *outIndex = tree->nodes[0].elemIdx;
  return tree->nodes[0].elem;
}

LoserTree_ElementType LoserTree_GetLoserValue(LoserTree lt, LoserTree_SizeType *outIndex)
{
  assert(lt != NULL);
  return *(LoserTree_GetLoserPtr(lt, outIndex));
}

LoserTree_ElementType LoserTree_PopLoser(LoserTree lt, LoserTree_SizeType *outIndex)
{
  assert(lt != NULL);
  LoserTree_ElementType loserValue = LoserTree_GetLoserValue(lt, outIndex);
  _LoserTree_ReplaceLoser(lt, 0);
  return loserValue;
}

const LoserTree_ElementType *LoserTree_GetElements(LoserTree lt, LoserTree_SizeType *outCount)
{
  if (!lt)
    return NULL;
  _LoserTree *tree = (_LoserTree *)lt;
  if (!tree->elemsOut)
    tree->elemsOut = (LoserTree_ElementType *)malloc(sizeof(LoserTree_ElementType) * tree->size);
  LoserTree_SizeType k = 0;
  for (LoserTree_SizeType i = 0; i < tree->size; i++) {
    if (tree->nodes[tree->lastLevelStartIndex + i].elem)
      tree->elemsOut[k++] = tree->elems[i];
  }
  if (outCount)
    *outCount = k;
  return tree->elemsOut;
}