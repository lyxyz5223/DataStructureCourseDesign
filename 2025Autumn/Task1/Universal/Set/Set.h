#ifndef SET_H
#define SET_H
#include "../types.h"
typedef struct Set Set;
typedef Iterator SetIterator;
/**
 * 集合API接口
 */
typedef struct SetApi
{
  /**
   * 销毁集合
   * @param set 集合
   * @return true成功，false失败
   */
  lboolean (*destroy)(Set *set);
  /**
   * 向集合中添加元素
   * @param set 集合
   * @param data 元素
   * @return true成功，false失败
   */
  lboolean (*add)(Set *set, UniversalData data);
  /**
   * 从集合中删除元素
   * @param set 集合
   * @param data 元素
   * @return true成功，false失败
   */
  lboolean (*remove)(Set *set, UniversalData data);
  /**
   * 判断元素是否存在于集合中
   * @param set 集合
   * @param data 元素
   * @return true存在，false不存在
   */
  lboolean (*exist)(Set *set, UniversalData data);
  /**
   * 获取集合元素个数
   * @param set 集合
   * @return 元素个数
   */
  ull (*length)(Set *set);
  /**
   * 判断集合是否为空
   * @param set 集合
   * @return true为空，false不为空
   */
  lboolean (*empty)(Set *set);
  /**
   * 清空集合
   * @param set 集合
   * @return true成功，false失败
   */
  lboolean (*clear)(Set *set);
  /**
   * 通过索引访问元素
   * @warning 该方法不推荐使用，因为集合是无序的，索引没有实际意义
   * @param set 集合
   * @param index 索引
   * @return 元素副本
   */
  UniversalData (*get)(Set *set, ull index); // get返回元素副本
  /**
   * 通过索引访问元素
   * @warning 该方法不推荐使用，因为集合是无序的，索引没有实际意义
   * @param set 集合
   * @param index 索引
   * @return 元素引用
   */
  UniversalData* (*at)(Set *set, ull index); // at返回元素地址
  // 迭代器
  /**
   * 获取集合的迭代器
   * @param set 集合
   * @param index 索引指针，作为当前索引，传入时应提前初始化，函数内部会自增
   * @return 当前位置的迭代器
   */
  SetIterator (*iterator)(Set *set, ull *index);
  /**
   * 获取集合的开始迭代器
   * @param set 集合
   * @return 指向第一个元素的迭代器
   */
  SetIterator (*begin)(Set *set); // 指向第一个元素的迭代器
  /**
   * 获取集合的结束迭代器
   * @param set 集合
   * @return data为空的迭代器
   */
  SetIterator (*end)(Set *set); // data为空的迭代器
  // 集合运算
  /**
   * 并集计算(A ∪ B)
   * opUnion === operator union
   * @warning 两个集合的比较函数必须相同
   * @param setA 集合A
   * @param setB 集合B
   * @return 并集
   */
  Set *(*opUnion)(Set *setA, Set *setB);
  /**
   * 交集计算(A ∩ B)
   * opIntersection === operator intersection
   * @warning 两个集合的比较函数必须相同
   * @param setA 集合A
   * @param setB 集合B
   * @return 交集
   */
  Set *(*opIntersection)(Set *setA, Set *setB);
  /**
   * 差集计算(A - B)
   * opDifference === operator difference
   * @warning 两个集合的比较函数必须相同
   * @param setA 集合A
   * @param setB 集合B
   * @return 差集
   */
  Set *(*opDifference)(Set *setA, Set *setB);
  /**
   * 补集计算(A^c)
   * opComplement === operator complement
   * @warning 两个集合的比较函数必须相同
   * @param universalSet 全集
   * @param subset 子集
   * @return 补集
   */
  Set *(*opComplement)(Set *universalSet, Set *subset);
  /**
   * 子集判断(A ⊆ B)
   * isSubset === is Subset
   * @warning 两个集合的比较函数必须相同
   * @param subset 可能的子集
   * @param set 可能的全集
   */
  lboolean (*isSubset)(Set *subset, Set *set);
} SetApi;

/**
 * 集合结构体
 */
typedef struct Set
{
  SetApi *functions;
  Comparator cmp; // 比较函数，用于判断元素是否相等
} Set;

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
  Set *Set_Create(UniversalData dataSet[], ull length, Comparator cmp);

#ifdef __cplusplus
}
#endif

#endif // SET_H
