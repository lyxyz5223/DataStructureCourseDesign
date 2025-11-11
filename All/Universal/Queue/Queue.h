#ifndef QUEUE_H
#define QUEUE_H
#include "../types.h"

// 声明
typedef struct Queue *Queue;
typedef struct QueueApi QueueApi;

// 定义
typedef void* ElementType; // 队列元素类型定义为void指针，可存储任意类型的指针
#define QUEUE_ELEMENT_NULL NULL
typedef struct Queue
{
  QueueApi *functions;
} *Queue;
typedef struct QueueResult
{
  lboolean success;
  ElementType element;
} QueueResult;

typedef struct QueueApi
{
  /**
   * 创建一个新的Queue对象
   * @return 返回创建的Queue对象指针
   */
  Queue (*create)();
  /**
   * 销毁Queue对象，释放内存
   * @param queue 需要销毁的Queue对象指针
   * @return true成功，false失败
   */
  lboolean (*destroy)(Queue queue);
  /**
   * 入队操作，将元素添加到队列尾部
   * @param queue 输入的Queue对象指针
   * @param element 输入的元素
   * @return true成功，false失败
   */
  lboolean (*push)(Queue queue, ElementType element);
  /**
   * 出队操作，移除并返回队列头部的元素
   * @param queue 输入的Queue对象指针
   * @return 返回队列头部的元素
   */
  QueueResult (*pop)(Queue queue);
  /**
   * 获取队列头部的元素但不移除它
   * @param queue 输入的Queue对象指针
   * @return 返回队列头部的元素
   */
  QueueResult (*front)(Queue queue);
  /**
   * 获取队列中的元素数量
   * @param queue 输入的Queue对象指针
   * @return 返回队列中的元素数量
   */
  ull (*size)(Queue queue);
  /**
   * 判断队列是否为空
   * @param queue 输入的Queue对象指针
   * @return true为空，false不为空
   */
  lboolean (*isEmpty)(Queue queue);
} QueueApi;

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * 创建一个新的Queue对象
   * @return 返回创建的Queue对象指针
   */
  Queue Queue_Create();

#ifdef __cplusplus
}
#endif

#endif // QUEUE_H
