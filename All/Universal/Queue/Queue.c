#include "Queue.h"
#include "../Linklist/Linklist.h"
#include <stdlib.h>
// 声明api结构体全局变量
QueueApi _Queue_Global_Api;

// 定义队列结构体，包含链表用于存储元素
typedef struct _Queue
{
  QueueApi *functions;
  Linklist* list; // 使用链表实现队列
} *_Queue;

Queue Queue_Create()
{
  Linklist* ll = Linklist_Create(NULL, 0);
  if (!ll) return NULL;
  _Queue queue = malloc(sizeof(struct _Queue));
  if (!queue) return NULL;
  queue->list = ll;
  queue->functions = &_Queue_Global_Api;
  return (Queue)queue;
}

lboolean Queue_Destroy(Queue queue)
{
  if (!queue) return false;
  _Queue q = (_Queue)queue;
  if (q->list)
    q->list->functions->destroy(q->list);
  free(q);
  return true;
}


lboolean Queue_Push(Queue queue, ElementType element)
{
  if (!queue) return false;
  _Queue q = (_Queue)queue;
  // 采用尾插法，时间复杂度为O(1)
  return q->list->functions->append(q->list, element);
}

QueueResult Queue_Pop(Queue queue)
{
  QueueResult result = {
    .success = false,
    .element = QUEUE_ELEMENT_NULL
  };
  if (!queue)
    return result;
  _Queue q = (_Queue)queue;
  if (q->list->functions->isEmpty(q->list))
    return result;
  result.success = true;
  result.element = q->list->functions->get(q->list, 0); // 获取头元素
  q->list->functions->remove(q->list, 0); // 头删法，时间复杂度为O(1)
  return result;
}

QueueResult Queue_Front(Queue queue)
{
  QueueResult rst = {
    .success = false,
    .element = QUEUE_ELEMENT_NULL
  };
  if (!queue)
    return rst;
  _Queue q = (_Queue)queue;
  if (q->list->functions->isEmpty(q->list))
    return rst;
  rst.success = true;
  rst.element = q->list->functions->get(q->list, 0); // 获取头元素
  return rst;
}

ull Queue_Size(Queue queue)
{
  if (!queue) return 0;
  _Queue q = (_Queue)queue;
  return q->list->functions->length(q->list);
}

lboolean Queue_IsEmpty(Queue queue)
{
  if (!queue) return true;
  _Queue q = (_Queue)queue;
  return q->list->functions->isEmpty(q->list);
}

// 定义全局api结构体变量
QueueApi _Queue_Global_Api = {
  .create = Queue_Create,
  .destroy = Queue_Destroy,
  .push = Queue_Push,
  .pop = Queue_Pop,
  .front = Queue_Front,
  .size = Queue_Size,
  .isEmpty = Queue_IsEmpty
};
