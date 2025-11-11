#include "Stack.h"
#include "LinklistStack.h"

Stack* Stack_Create()
{
  return LinklistStack_Create();
}

lboolean Stack_Destroy(Stack* stack)
{
  return LinklistStack_Destroy(stack);
}
