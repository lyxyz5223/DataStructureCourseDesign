#include "ThreadPool.h"
#include "../Queue/Queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct _ThreadPool
{
  pthread_t *threads;
  int threadCount;
  Queue taskQueue;
  lboolean stop;
  // 队列互斥锁
  pthread_mutex_t queueMutex;
  // 条件变量，通知有新任务到来
  pthread_cond_t condition;
  // 等待所有任务完成的条件变量
  pthread_cond_t allTaskDoneCondition;
  // 统计正在进行的任务数
  int activeTaskCount;
} *_ThreadPool;
typedef struct _ThreadPoolTask
{
  void (*function)(void *);
  void *arg;
} *_ThreadPoolTask;


ThreadPoolTask ThreadPool_CreateTask(void (*function)(void *), void *arg)
{
  _ThreadPoolTask task = malloc(sizeof(struct _ThreadPoolTask));
  task->function = function;
  task->arg = arg;
  return (ThreadPoolTask)task;
}

/**
 * 销毁任务
 * @param task 任务指针
 */
void _ThreadPool_DestroyTask(_ThreadPoolTask task)
{
  if (task)
    free(task);
}

/**
 * 线程池工作线程函数
 * @param arg 线程池指针
 * @return NULL
 */
void *_ThreadPool_Worker(void *arg)
{
  _ThreadPool pool = (_ThreadPool)arg;
  while (1)
  {
    pthread_mutex_lock(&pool->queueMutex);
    while (!pool->stop && pool->taskQueue->functions->isEmpty(pool->taskQueue))
    {
      // 等待任务到来
      pthread_cond_wait(&pool->condition, &pool->queueMutex);
    }
    if (pool->stop)
    {
      pthread_mutex_unlock(&pool->queueMutex);
      break; // 退出线程
    }
    QueueResult taskRst = pool->taskQueue->functions->pop(pool->taskQueue);
    pthread_mutex_unlock(&pool->queueMutex);
    if (taskRst.success)
    {
      _ThreadPoolTask task = (_ThreadPoolTask)taskRst.element;
      if (pthread_mutex_lock(&pool->queueMutex) == 0) {
        pool->activeTaskCount++;
        pthread_mutex_unlock(&pool->queueMutex);
      }
      task->function(task->arg);
      // 任务完成，释放任务内存
      _ThreadPool_DestroyTask(task);
      if (pthread_mutex_lock(&pool->queueMutex) == 0) {
        pool->activeTaskCount--;
        if (pool->activeTaskCount == 0 && pool->taskQueue->functions->isEmpty(pool->taskQueue))
          pthread_cond_broadcast(&pool->allTaskDoneCondition);
        pthread_mutex_unlock(&pool->queueMutex);
      }
      else
      {
        // 无法锁定互斥锁，严重错误，直接退出线程
        break;
      }
    }
  }
  return NULL;
}

ThreadPool ThreadPool_Create(int threadCount)
{
  if (threadCount <= 0)
    return NULL;
  _ThreadPool pool = malloc(sizeof(struct _ThreadPool));
  if (!pool)
    return NULL;
  pool->threads = malloc(sizeof(pthread_t) * threadCount);
  if (!pool->threads)
  {
    free(pool);
    return NULL;
  }
  pool->threadCount = threadCount;
  pool->taskQueue = Queue_Create();
  if (!pool->taskQueue)
  {
    free(pool->threads);
    free(pool);
    return NULL;
  }
  memset(pool->threads, 0, sizeof(pthread_t) * threadCount);
  pool->stop = false;
  pool->activeTaskCount = 0;
  // 初始化互斥锁和条件变量
  pthread_mutex_init(&pool->queueMutex, NULL);
  pthread_cond_init(&pool->condition, NULL);
  pthread_cond_init(&pool->allTaskDoneCondition, NULL);
  for (int i = 0; i < threadCount; i++)
  {
    if (pthread_create(&pool->threads[i], NULL, _ThreadPool_Worker, pool) != 0)
    {
      // 创建线程失败，销毁已经创建的线程
      pool->stop = true;
      for (int j = 0; j < i; j++)
      {
        pthread_join(pool->threads[j], NULL);
      }
      pool->taskQueue->functions->destroy(pool->taskQueue);
      free(pool->threads);
      free(pool);
      return NULL;
    }
  }
  return (ThreadPool)pool;
}

void ThreadPool_Destroy(ThreadPool threadPool)
{
  if (!threadPool)
    return;
  _ThreadPool pool = (_ThreadPool)threadPool;
  // 设置停止标志并唤醒所有线程
  pthread_mutex_lock(&pool->queueMutex);
  pool->stop = true;
  pthread_cond_broadcast(&pool->condition); // 唤醒所有等待的线程
  pthread_mutex_unlock(&pool->queueMutex);
  // 等待所有线程退出
  for (int i = 0; i < pool->threadCount; i++)
  {
    pthread_join(pool->threads[i], NULL);
  }
  // 销毁任务队列
  pool->taskQueue->functions->destroy(pool->taskQueue);
  // 释放线程数组
  free(pool->threads);
  // 销毁互斥锁和条件变量
  pthread_mutex_destroy(&pool->queueMutex);
  pthread_cond_destroy(&pool->condition);
  pthread_cond_destroy(&pool->allTaskDoneCondition);
  // 释放线程池结构体
  free(pool);
}

lboolean ThreadPool_Submit(ThreadPool threadPool, ThreadPoolTask task)
{
  if (!threadPool || !task)
    return false;
  _ThreadPool pool = (_ThreadPool)threadPool;
  pthread_mutex_lock(&pool->queueMutex);
  if (pool->stop)
  {
    pthread_mutex_unlock(&pool->queueMutex);
    return false; // 线程池已停止，拒绝新任务
  }
  lboolean rst = pool->taskQueue->functions->push(pool->taskQueue, (ElementType)task);
  if (rst)
  {
    // 成功添加任务，通知一个等待的线程
    pthread_cond_signal(&pool->condition);
  }
  else
  {
    // 添加任务失败，释放任务内存
    _ThreadPool_DestroyTask((_ThreadPoolTask)task);
  }
  pthread_mutex_unlock(&pool->queueMutex);
  return rst;
}

void ThreadPool_Wait(ThreadPool threadPool)
{
  if (!threadPool)
    return;
  _ThreadPool pool = (_ThreadPool)threadPool;
  pthread_mutex_lock(&pool->queueMutex);
  while (pool->activeTaskCount > 0 || !pool->taskQueue->functions->isEmpty(pool->taskQueue))
  {
    pthread_cond_wait(&pool->allTaskDoneCondition, &pool->queueMutex);
  }
  pthread_mutex_unlock(&pool->queueMutex);
}
