#include "LinklistStack.h"
#include <stdlib.h>
#include "../Linklist/Linklist.h"

StackApi _LinklistStack_Global_Api;

typedef struct _LinklistStack
{
  StackApi *functions;
  // 内部
  Linklist *linklist;
} _LinklistStack;

Stack* LinklistStack_Create()
{
  _LinklistStack *stack = malloc(sizeof(_LinklistStack));
  stack->functions = &_LinklistStack_Global_Api;
  stack->linklist = Linklist_Create(0, 0);
  return (Stack *)stack;
}

lboolean LinklistStack_Destroy(Stack *stack)
{
  _LinklistStack* _stack = (_LinklistStack*)stack;
  lboolean rst = true;
  rst = _stack->linklist->functions->destroy(_stack->linklist);
  return rst;
}

lboolean LinklistStack_Push(Stack *stack, StackItem item)
{
  if (!stack)
    return false;
  // 头插法
  _LinklistStack *_stack = (_LinklistStack *)stack;
  StackItem* dataCopy = malloc(sizeof(StackItem));
  if (!dataCopy)
    return false;
  *dataCopy = item;
  lboolean rst = _stack->linklist->functions->insert(_stack->linklist, 0, (UniversalData)dataCopy);
  return rst;
}

StackItem LinklistStack_Pop(Stack *stack)
{
  StackItem emptyItem = {
    .data = NULL
  };
  _LinklistStack *_stack = (_LinklistStack *)stack;
  if (!stack || _stack->linklist->functions->isEmpty(_stack->linklist))
    return emptyItem;
  StackItem* item = (StackItem*)_stack->linklist->functions->get(_stack->linklist, 0);
  emptyItem = *item;
  lboolean rst = _stack->linklist->functions->remove(_stack->linklist, 0);
  free(item);
  return emptyItem;
}

StackItem *LinklistStack_TopPtr(Stack *stack)
{
  if (!stack)
    return NULL;
  _LinklistStack *_stack = (_LinklistStack *)stack;
  StackItem *item = _stack->linklist->functions->get(_stack->linklist, 0);
  return item;
}

StackItem LinklistStack_Top(Stack *stack)
{
  if (!stack)
  {
    StackItem emptyItem = {
      .data = NULL
    };
    return emptyItem;
  }
  StackItem *item = LinklistStack_TopPtr(stack);
  if (!item)
  {
    StackItem emptyItem = {
      .data = NULL
    };
    return emptyItem;
  }
  return *item;
}

lboolean LinklistStack_IsEmpty(Stack *stack)
{
  if (!stack)
    return true;
  _LinklistStack *_stack = (_LinklistStack *)stack;
  return _stack->linklist->functions->isEmpty(_stack->linklist);
}

ull LinklistStack_Size(Stack *stack)
{
  if (!stack)
    return 0;
  _LinklistStack *_stack = (_LinklistStack *)stack;
  return _stack->linklist->functions->length(_stack->linklist);
}

StackApi _LinklistStack_Global_Api = {
  .create = LinklistStack_Create,
  .destroy = LinklistStack_Destroy,
  .isEmpty = LinklistStack_IsEmpty,
  .pop = LinklistStack_Pop,
  .push = LinklistStack_Push,
  .size = LinklistStack_Size,
  .top = LinklistStack_Top,
  .topPtr = LinklistStack_TopPtr
};
