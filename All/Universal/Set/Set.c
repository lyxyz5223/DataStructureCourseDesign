#include "LinklistSet.h"
Set *Set_Create(UniversalData dataSet[], ull length, Comparator cmp)
{
  // 默认使用 LinklistSet 作为实现
  return LinklistSet_Create(dataSet, length, cmp);
}
