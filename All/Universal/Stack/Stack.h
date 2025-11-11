#ifndef STACK_H
#define STACK_H
#include "../types.h"

typedef struct StackApi StackApi;

typedef struct StackItem
{
  UniversalData data;
} StackItem;

typedef struct Stack
{
  StackApi *functions;
} Stack;

typedef struct StackApi
{
  /**
   * 创建一个栈
   * @return 栈指针，失败时返回NULL
   */
  Stack* (*create)();
  /**
   * 销毁栈
   * @param stack 栈指针
   * @return true成功，false失败
   */
  lboolean (*destroy)(Stack* stack);
  /**
   * 压栈
   * @param stack 栈指针
   * @param data StackItem结构体
   * @return true成功，false失败
   */
  lboolean (*push)(Stack *stack, StackItem item);
  /**
   * 弹栈
   * @param stack 栈指针
   * @return 弹出的元素，栈为空时返回StackItem结构体的data为NULL
   */
  StackItem (*pop)(Stack *stack);
  /**
   * 获取栈顶元素但不弹出
   * @param stack 栈指针
   * @return 栈顶元素，栈为空时返回StackItem结构体的data为NULL
   */
  StackItem (*top)(Stack *stack);
  /**
   * 获取栈顶元素地址但不弹出
   * @param stack 栈指针
   * @return 栈顶元素地址，栈为空时返回NULL
   */
  StackItem* (*topPtr)(Stack *stack);
  /**
   * 判断栈是否为空
   * @param stack 栈指针
   * @return true为空，false不为空
   */
  lboolean (*isEmpty)(Stack* stack);
  /**
   * 获取栈的大小
   * @param stack 栈指针
   * @return 栈的元素个数
   */
  ull (*size)(Stack* stack);
} StackApi;


#ifdef __cplusplus
extern "C"
{
#endif
  /**
   * 创建一个栈
   * @return 栈指针，失败时返回NULL
   */
  Stack* Stack_Create();
  /**
   * 销毁栈
   * @param stack 栈指针
   * @return true成功，false失败
   */
  lboolean Stack_Destroy(Stack* stack);

#ifdef __cplusplus
}
#endif


#endif // STACK_H
