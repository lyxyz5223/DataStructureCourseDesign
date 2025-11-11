#include "Universal/Stack/Stack.h"
#include <stdio.h>

int main()
{
  const char *data[] = {
    "Hello world",
    "Hello zjc",
    "Hello hust",
    "Hello see",
    "Fuck you"
  };
  Stack *st = Stack_Create();
  StackItem tmpItem;
  for (int i = 0; i < sizeof(data) / sizeof(data[0]); i++)
  {
    tmpItem.data = (void*)data[i];
    st->functions->push(st, tmpItem);
  }
  while (!st->functions->isEmpty(st))
  {
    StackItem item = st->functions->pop(st);
    printf("%s\n", item.data);
  }
  return 0;
}
