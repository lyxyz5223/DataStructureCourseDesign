#include "Universal/Set/Set.h"
#include <stdio.h>
#include <string.h>

/**
 * 打印集合内容
 */
void printSet(Set *set)
{
  printf("{ ");
  for (SetIterator iter = set->functions->begin(set);
       !iter->equals(iter, set->functions->end(set)); iter->next(iter))
  {
    if (iter->hasNext(iter))
      printf("%s, ", (char *)*iter->dataPtr);
    else
      printf("%s ", (char *)*iter->dataPtr);
  }
  printf("}\n");
}

int universalDataStrcmp(const UniversalData* a, const UniversalData* b)
{
  return strcmp((const char *)*a, (const char *)*b);
}
int main()
{
  Set *set = Set_Create(0, 0, universalDataStrcmp);
  printf("Set is empty: %s\n", set->functions->empty(set) ? "true" : "false");
  set->functions->add(set, "HUST");
  set->functions->add(set, "Hello, world!");
  set->functions->add(set, "Huazhong University of Science and Technology");
  set->functions->add(set, "1037");
  set->functions->add(set, "Huazhong University of Science and Technology");
  set->functions->add(set, "Hello, world!");
  set->functions->add(set, "Huazhong University");
  printf("Set is empty: %s\n", set->functions->empty(set) ? "true" : "false");
  printf("Set length: %llu\n", set->functions->length(set));
  printSet(set);
  set->functions->remove(set, "Hello, world!");
  printf("[After remove \"Hello, world!\"]:\n");
  printf("Set is empty: %s\n", set->functions->empty(set) ? "true" : "false");
  printf("Set length: %llu\n", set->functions->length(set));
  printSet(set);
  set->functions->clear(set);
  printf("[After clear]:\n");
  printf("Set is empty: %s\n", set->functions->empty(set) ? "true" : "false");
  printf("Set length: %llu\n", set->functions->length(set));
  printSet(set);
  
  set->functions->destroy(set);
  // system("pause");
  return 0;
}