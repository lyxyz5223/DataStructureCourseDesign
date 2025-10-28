#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include "../types.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct ThreadPool {
  
} *ThreadPool; // 不透明指针

typedef struct ThreadPoolTask
{

} *ThreadPoolTask; // 不透明指针

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * 创建线程池
   * @param threadCount 线程数量
   * @return 返回创建的线程池指针，失败返回NULL
   */
  ThreadPool ThreadPool_Create(int threadCount);

  /**
   * 销毁线程池
   * @param pool 线程池指针
   */
  void ThreadPool_Destroy(ThreadPool pool);

  /**
   * 创建任务结构体
   * @param function 任务函数指针
   * @param arg 任务参数指针
   * @return 返回创建的任务结构体，失败返回NULL
   */
  ThreadPoolTask ThreadPool_CreateTask(void (*function)(void *), void *arg);

  /**
   * 向线程池提交任务
   * @param pool 线程池指针
   * @param task 任务
   * @return true表示成功，false表示失败
   * @warning 提交任务后，任务将由线程池管理，调用者不需要也不应该释放任务所占内存
   * @warning 请不要试图free任务，线程池会自动释放任务内存
   */
  lboolean ThreadPool_Submit(ThreadPool pool, ThreadPoolTask task);

  /**
   * 等待所有任务完成
   * @param pool 线程池指针
   */
  void ThreadPool_Wait(ThreadPool pool);

#ifdef __cplusplus
}
#endif

#endif // THREAD_POOL_H