#include <stdio.h>
#include <string.h>
#include "Universal/Set/LinklistSet.h"

// UniversalData适配器
int universalDataStrcmp(const UniversalData *a, const UniversalData *b)
{
  return strcmp((const char *)*a, (const char *)*b);
}

void printSet(Set *set)
{
  printf("{ ");
  SetIterator iter = set->functions->begin(set);
  for (SetIterator iter = set->functions->begin(set); !iter->equals(iter, set->functions->end(set)); iter->next(iter))
  {
    if (iter->hasNext(iter))
      printf("%s, ", (char *)*iter->dataPtr);
    else
      printf("%s ", (char *)*iter->dataPtr);
  }
  printf("}\n");
  iter->destroy(iter);
}

int main()
{
  // 初始化数据
  char *dataA[] = {"apple", "banana", "cherry"};
  char *dataB[] = {"banana", "date", "fig", "cherry"};

  // 创建集合
  Set *setA = LinklistSet_Create((UniversalData *)dataA, 3, universalDataStrcmp);
  Set *setB = LinklistSet_Create((UniversalData *)dataB, 4, universalDataStrcmp);

  printf("Set A: ");
  printSet(setA);
  printf("Set B: ");
  printSet(setB);

  // 添加元素
  setA->functions->add(setA, "date");
  printf("Set A after add 'date': ");
  printSet(setA);

  // 清空集合
  setA->functions->clear(setA);
  printf("Set A after clear: ");
  printSet(setA);

  // 重新添加不同的元素
  setA->functions->add(setA, "kiwi");
  setA->functions->add(setA, "banana");
  setA->functions->add(setA, "mango");
  setA->functions->add(setA, "cherry");
  printf("Set A after re-adding elements: ");
  printSet(setA);

  // 查找元素
  printf("Set A exist 'banana': %d\n", setA->functions->exist(setA, "banana"));
  printf("Set A exist 'fig': %d\n", setA->functions->exist(setA, "fig"));

  // 删除元素
  setA->functions->remove(setA, "banana");
  printf("Set A after remove 'banana': ");
  printSet(setA);

  // 并集
  Set *setUnion = setA->functions->opUnion(setA, setB);
  printf("Union: ");
  printSet(setUnion);

  // 交集
  Set *setIntersection = setA->functions->opIntersection(setA, setB);
  printf("Intersection: ");
  printSet(setIntersection);

  // 差集
  Set *setDiff = setA->functions->opDifference(setA, setB);
  printf("Difference (A - B): ");
  printSet(setDiff);

  // 补集
  Set *setComp = setA->functions->opComplement(setUnion, setA);
  printf("Complement (Union - A): ");
  printSet(setComp);

  // 子集判断
  printf("Is setA subset of setUnion? %d\n", setA->functions->isSubset(setA, setUnion));
  printf("Is setB subset of setA? %d\n", setB->functions->isSubset(setB, setA));

  // 清理
  setA->functions->destroy(setA);
  setB->functions->destroy(setB);
  setUnion->functions->destroy(setUnion);
  setIntersection->functions->destroy(setIntersection);
  setDiff->functions->destroy(setDiff);
  setComp->functions->destroy(setComp);

  return 0;
}