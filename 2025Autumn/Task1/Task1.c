#include "Universal/Set/Set.h"
#include <stdio.h>
#include <string.h>

/**
 * 打印集合内容
 */
void printSet(Set *set)
{
  printf("{ ");
  SetIterator iter;
  for (iter = set->functions->begin(set);
       !iter->equals(iter, set->functions->end(set)); iter->next(iter))
  {
    if (iter->hasNext(iter))
      printf("%s, ", (char *)*iter->dataPtr);
    else
      printf("%s ", (char *)*iter->dataPtr);
  }
  printf("}\n");
  iter->destroy(iter);
}

int universalDataStrcmp(const UniversalData* a, const UniversalData* b)
{
  return strcmp((const char *)*a, (const char *)*b);
}

void testSet()
{
  Set *set = Set_Create(0, 0, universalDataStrcmp);
  printf("Set is empty: %s\n", set->functions->isEmpty(set) ? "true" : "false");
  set->functions->add(set, "HUST");
  set->functions->add(set, "Hello, world!");
  set->functions->add(set, "Huazhong University of Science and Technology");
  set->functions->add(set, "1037");
  set->functions->add(set, "Huazhong University of Science and Technology");
  set->functions->add(set, "Hello, world!");
  set->functions->add(set, "Huazhong University");
  printf("Set is empty: %s\n", set->functions->isEmpty(set) ? "true" : "false");
  printf("Set length: %llu\n", set->functions->length(set));
  printSet(set);
  set->functions->remove(set, "Hello, world!");
  printf("[After remove \"Hello, world!\"]:\n");
  printf("Set is empty: %s\n", set->functions->isEmpty(set) ? "true" : "false");
  printf("Set length: %llu\n", set->functions->length(set));
  printSet(set);
  set->functions->clear(set);
  printf("[After clear]:\n");
  printf("Set is empty: %s\n", set->functions->isEmpty(set) ? "true" : "false");
  printf("Set length: %llu\n", set->functions->length(set));
  printSet(set);

  set->functions->destroy(set);
}

void testSetOperations()
{
  // 创建两个集合
  Set *setA = Set_Create(0, 0, universalDataStrcmp);
  Set *setB = Set_Create(0, 0, universalDataStrcmp);

  // 添加元素
  setA->functions->add(setA, "A");
  setA->functions->add(setA, "B");
  setA->functions->add(setA, "C");
  setB->functions->add(setB, "B");
  setB->functions->add(setB, "C");
  setB->functions->add(setB, "D");

  printf("SetA: ");
  printSet(setA);
  printf("SetB: ");
  printSet(setB);

  // 并集
  Set *unionSet = setA->functions->opUnion(setA, setB);
  printf("Union (A ∪ B): ");
  printSet(unionSet);

  // 交集
  Set *intersectionSet = setA->functions->opIntersection(setA, setB);
  printf("Intersection (A ∩ B): ");
  printSet(intersectionSet);

  // 差集
  Set *diffSet = setA->functions->opDifference(setA, setB);
  printf("Difference (A - B): ");
  printSet(diffSet);

  // 补集（以 setA 为全集，setB 为子集）
  Set *complementSet = setA->functions->opComplement(setA, setB);
  printf("Complement (∁_A(B)): ");
  printSet(complementSet);

  // 子集判断
  printf("Is B subset of A? %s\n", setA->functions->isSubset(setB, setA) ? "true" : "false");

  // 删除一个元素D继续判断
  setB->functions->remove(setB, "D");
  // 子集判断
  printf("After remove \"D\": Is B subset of A? %s\n", setA->functions->isSubset(setB, setA) ? "true" : "false");

  // 清理
  setA->functions->destroy(setA);
  setB->functions->destroy(setB);
  unionSet->functions->destroy(unionSet);
  intersectionSet->functions->destroy(intersectionSet);
  diffSet->functions->destroy(diffSet);
  complementSet->functions->destroy(complementSet);
}

int main()
{
  testSet();
  testSetOperations();
  // system("pause");
  return 0;
}
