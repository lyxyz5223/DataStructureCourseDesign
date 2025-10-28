#include "Universal/LoserTree/LoserTree.h"
#include <stdio.h>
#include <stdlib.h>

// 简单整型比较器
int int_cmp(const void* a, const void* b)
{
  int x = *(const int *)a;
  int y = *(const int *)b;
  return x < y;
}

int main()
 {
  int arr[] = {7, 3, 9, 1, 5};
  int replace_values[] = {8, 0, 15, -3};
  int n = sizeof(arr) / sizeof(arr[0]);
  int m = sizeof(replace_values) / sizeof(replace_values[0]);
  LoserTree lt = LoserTree_Create(arr, sizeof(int), n, int_cmp);
  if (!lt)
  {
    printf("LoserTree 创建失败！\n");
    return 1;
  }
  printf("LoserTree 创建成功。\n");

  printf("初始数组: ");
  for (int i = 0; i < n; ++i)
    printf("%d ", arr[i]);
  printf("\n");
  printf("替换数组: ");
  for (int i = 0; i < m; ++i) {
    printf("%d ", replace_values[i]);
  }
  printf("\n");

  for (int i = 0; i < m; ++i) {
    int new_val = replace_values[i];
    const int* min_ptr = LoserTree_GetLoserPtr(lt, NULL);
    printf("Pop out: %d\n", *min_ptr);
    LoserTree_ReplaceLoser(lt, new_val);

    // Test
    LoserTree_SizeType outCount = 0;
    LoserTree_ElementType *es = (LoserTree_ElementType *)LoserTree_GetElements(lt, &outCount);
    printf("Current elements: ");
    for (int j = 0; j < outCount; ++j)
      printf("%d ", es[j]);
    printf("\n");
  }
  for (int i = 0; i < n; ++i, LoserTree_PopLoser(lt, NULL))
  {
    const int *p = LoserTree_GetLoserPtr(lt, NULL);
    printf("Pop out: %d\n", *p);

    // Test
    LoserTree_SizeType outCount = 0;
    LoserTree_ElementType *es = (LoserTree_ElementType *)LoserTree_GetElements(lt, &outCount);
    printf("Current elements: ");
    for (int j = 0; j < outCount; ++j)
      printf("%d ", es[j]);
    printf("\n");
  }
  printf("\n");
  // 销毁
  LoserTree_Destroy(lt);
  return 0;
}
