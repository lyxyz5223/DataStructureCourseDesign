#ifndef ARRAYSTACK_H
#define ARRAYSTACK_H
#include "../types.h"
#include "Stack.h"

#ifdef __cplusplus
extern "C"
{
#endif
  /**
   * 创建一个栈
   * @return 栈指针，失败时返回NULL
   */
  Stack *ArrayStack_Create();
  /**
   * 销毁栈
   * @param stack 栈指针
   * @return true成功，false失败
   */
  lboolean ArrayStack_Destroy(Stack *stack);

  /**
   * 压栈
   * @param stack 栈指针
   * @param item StackItem结构体
   * @return true成功，false失败
   */
  lboolean ArrayStack_Push(Stack *stack, StackItem item);
  /**
   * 弹栈
   * @param stack 栈指针
   * @return 栈顶元素，栈为空时返回StackItem结构体的data为NULL
   */
  StackItem ArrayStack_Pop(Stack *stack);
  /**
   * 获取栈顶元素但不弹出
   * @param stack 栈指针
   * @return 栈顶元素，栈为空时返回StackItem结构体的data为NULL
   */
  StackItem ArrayStack_Top(Stack *stack);
  /**
   * 获取栈顶元素地址但不弹出
   * @param stack 栈指针
   * @return 栈顶元素地址，栈为空时返回NULL
   * @note 有效期限与栈顶元素一致，弹出栈后该地址失效
   */
  StackItem *ArrayStack_TopPtr(Stack *stack);

  /**
   * 判断栈是否为空
   * @param stack 栈指针
   * @return true为空，false不为空
   */
  lboolean ArrayStack_IsEmpty(Stack *stack);
  /**
   * 获取栈的大小
   * @param stack 栈指针
   * @return 栈的元素个数
   */
  ull ArrayStack_Size(Stack *stack);

#ifdef __cplusplus
}
#endif

#endif // ARRAYSTACK_H
