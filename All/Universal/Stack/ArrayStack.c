#include "ArrayStack.h"
#include "../Array/Array.h"

StackApi _ArrayStack_Global_Api;

typedef struct _ArrayStack
{
  StackApi *functions;
  // 内部
  Array *array;
} _ArrayStack;


Stack* ArrayStack_Create()
{
  _ArrayStack *stack = malloc(sizeof(_ArrayStack));
  stack->functions = &_ArrayStack_Global_Api;
  stack->array = Array_Create(NULL, 0);
  return (Stack *)stack;
}

lboolean ArrayStack_Destroy(Stack *stack)
{
  _ArrayStack* _stack = (_ArrayStack*)stack;
  lboolean rst = true;
  rst = _stack->array->functions->destroy(_stack->array);
  return rst;
}

lboolean ArrayStack_Push(Stack *stack, StackItem item)
{
  if (!stack)
    return false;
  _ArrayStack *_stack = (_ArrayStack *)stack;
  StackItem *dataCopy = malloc(sizeof(StackItem));
  if (!dataCopy)
    return false;
  *dataCopy = item;
  lboolean rst = _stack->array->functions->append(_stack->array, (UniversalData)dataCopy);
  return rst;
}

StackItem ArrayStack_Pop(Stack *stack)
{
  StackItem emptyItem = {
    .data = NULL
  };
  _ArrayStack *_stack = (_ArrayStack *)stack;
  if (!stack || _stack->array->functions->isEmpty(_stack->array))
    return emptyItem;
  ull index = _stack->array->functions->length(_stack->array) - 1;
  StackItem *item = (StackItem *)_stack->array->functions->get(_stack->array, index);
  emptyItem = *item;
  lboolean rst = _stack->array->functions->remove(_stack->array, index);
  free(item);
  return emptyItem;
}

StackItem *ArrayStack_TopPtr(Stack *stack)
{
  if (!stack)
    return NULL;
  _ArrayStack *_stack = (_ArrayStack *)stack;
  ull index = _stack->array->functions->length(_stack->array) - 1;
  StackItem *item = _stack->array->functions->get(_stack->array, index);
  return item;
}

StackItem ArrayStack_Top(Stack *stack)
{
  if (!stack)
  {
    StackItem emptyItem = {
      .data = NULL
    };
    return emptyItem;
  }
  StackItem *item = ArrayStack_TopPtr(stack);
  if (!item)
  {
    StackItem emptyItem = {
      .data = NULL
    };
    return emptyItem;
  }
  return *item;
}

lboolean ArrayStack_IsEmpty(Stack *stack)
{
  if (!stack)
    return true;
  _ArrayStack *_stack = (_ArrayStack *)stack;
  return _stack->array->functions->isEmpty(_stack->array);
}

ull ArrayStack_Size(Stack *stack)
{
  if (!stack)
    return 0;
  _ArrayStack *_stack = (_ArrayStack *)stack;
  return _stack->array->functions->length(_stack->array);
}

StackApi _ArrayStack_Global_Api = {
  .create = ArrayStack_Create,
  .destroy = ArrayStack_Destroy,
  .isEmpty = ArrayStack_IsEmpty,
  .pop = ArrayStack_Pop,
  .push = ArrayStack_Push,
  .size = ArrayStack_Size,
  .top = ArrayStack_Top,
  .topPtr = ArrayStack_TopPtr
};
