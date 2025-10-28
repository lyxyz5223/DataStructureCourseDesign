#include "FileSplitting.h"
#include "../ThreadPool/ThreadPool.h" // 线程池
#include "../Queue/Queue.h" // 任务队列
#include "../FileSystem/FileSystem.h" // 文件系统

// 多线程库
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>


// 线程参数结构体
typedef struct ThreadParam
{
  FileSystem fs;    // 大文件的FileSystem对象
  String outputDir; // 分割后的文件目录
  String outputPartPrefix; // 分割后文件的前缀
  String outputPartExtension; // 分割后文件的扩展名
  ull partIndex;    // 当前分割文件的索引
  ull startOffset;  // 当前分割文件的起始偏移
  ull partSize;     // 当前分割文件的大小
} ThreadParam;

/**
 * 设置线程参数结构体
 * @param paramPtr 线程参数结构体指针
 * @param fs 大文件的FileSystem对象
 * @param outputDir 分割后的文件目录
 * @param outputPartPrefix 分割后文件的前缀
 * @param outputPartExtension 分割后文件的扩展名
 * @param partIndex 当前分割文件的索引
 * @param startOffset 当前分割文件的起始偏移
 * @param partSize 当前分割文件的大小
 */
void _FileSplitting_SetThreadParam(ThreadParam *paramPtr, FileSystem fs, String outputDir, String outputPartPrefix, String outputPartExtension, ull partIndex, ull startOffset, ull partSize)
{
  if (!paramPtr) return;
  paramPtr->fs = fs;
  paramPtr->outputDir = outputDir;
  paramPtr->outputPartPrefix = outputPartPrefix;
  paramPtr->outputPartExtension = outputPartExtension;
  paramPtr->partIndex = partIndex;
  paramPtr->startOffset = startOffset;
  paramPtr->partSize = partSize;
}

/**
 * 创建线程参数结构体
 * @param fs 大文件的FileSystem对象
 * @param outputDir 分割后的文件目录
 * @param outputPartPrefix 分割后文件的前缀
 * @param outputPartExtension 分割后文件的扩展名
 * @param partIndex 当前分割文件的索引
 * @param startOffset 当前分割文件的起始偏移
 * @param partSize 当前分割文件的大小
 * @return 返回创建的线程参数结构体指针，失败返回NULL
 */
ThreadParam* _FileSplitting_CreateThreadParam(FileSystem fs, String outputDir, String outputPartPrefix, String outputPartExtension, ull partIndex, ull startOffset, ull partSize)
{
  ThreadParam* param = malloc(sizeof(ThreadParam));
  if (!param) return NULL;
  _FileSplitting_SetThreadParam(param, fs, outputDir, outputPartPrefix, outputPartExtension, partIndex, startOffset, partSize);
  return param;
}

/**
 * 创建线程参数结构体数组
 * @param count 需要创建的线程参数结构体数量
 * @return 返回创建的线程参数结构体数组指针，失败返回NULL
 */
ThreadParam* _FileSplitting_CreateThreadParamArray(ull count)
{
  ThreadParam* array = malloc(sizeof(ThreadParam) * count);
  if (!array) return NULL;
  memset(array, 0, sizeof(ThreadParam) * count); // 初始化为0
  return array;
}


/**
 * 销毁线程参数结构体
 * @param param 线程参数结构体指针
 */
void _FileSplitting_DestroyThreadParam(ThreadParam* param)
{
  if (param)
    free(param);
}

/**
 * 销毁线程参数结构体数组
 * @param array 线程参数结构体数组指针
 */
void _FileSplitting_DestroyThreadParamArray(ThreadParam* array)
{
  if (array) 
    free(array);
}

/**
 * 内部线程处理函数
 * @param arg 线程参数
 * @return NULL
 */
void _FileSplitting_FileSplittingThreadProc(void *param)
{
  ThreadParam* arg = (ThreadParam*)param;
  // 每个线程单独处理一个分割文件
  FileSystem fs = arg->fs;
  String outputDir = arg->outputDir;
  String outputPartPrefix = arg->outputPartPrefix;
  String outputPartExtension = arg->outputPartExtension;
  StringApi* strApi = outputDir->functions;
  ull partIndex = arg->partIndex;
  ull startOffset = arg->startOffset;
  ull partSize = arg->partSize;
  // 创建分割文件路径
  String partFilePath = strApi->copy(outputDir);
  if (strApi->length(partFilePath) > 0
    && strApi->get(partFilePath, strApi->length(partFilePath) - 1) != '/'
    && strApi->get(partFilePath, strApi->length(partFilePath) - 1) != '\\'
  ) {
    strApi->appendChar(partFilePath, '/');
  }
  // 添加前缀
  strApi->appendString(partFilePath, outputPartPrefix);
  // 添加索引
  char indexBuffer[128];
  snprintf(indexBuffer, sizeof(indexBuffer), "%llu", partIndex);
  strApi->appendCString(partFilePath, indexBuffer, 0, 1);
  // 添加扩展名
  strApi->appendString(partFilePath, outputPartExtension);

  // 先打开需要分割的文件，获取描述符
  const char* originFilePath = fs->functions->getFilePath(fs);
  fd_t fd = pOpenReadOnly(originFilePath);
  if (fd == FD_INVALID)
  {
    _FileSplitting_DestroyThreadParam(arg);
    return;
  }
  // 创建并打开分割文件
  FileSystem partFs = FileSystem_Create(strApi->toCString(partFilePath), "wb");
  if (!partFs) {
    pClose(fd);
    _FileSplitting_DestroyThreadParam(arg);
    return;
  }
  // 创建缓冲区
  const ull bufferSize = 1024 * 1024; // 1MB缓冲区
  char* buffer = malloc(bufferSize);
  if (!buffer) {
    pClose(fd);
    partFs->functions->close(partFs);
    _FileSplitting_DestroyThreadParam(arg);
    return;
  }
  ull elemSize = sizeof(char);
  // 读取和写入数据
  FILE *partFileHandle = partFs->functions->getFilePointer(partFs);
  int loopCount = 0;
  while (partSize > 0) {
    int bytesRead = pRead(fd, buffer, bufferSize, startOffset); // 多线程同时读取
    if (bytesRead <= 0) break;
    ull bytesWritten = 0;
    ull bytesToWrite = (bytesRead > partSize) ? partSize : bytesRead;
    bytesWritten = fwrite(buffer, elemSize, bytesToWrite, partFileHandle); // 写入新文件，不会多线程冲突
    if (bytesWritten == 0) break;
    startOffset += bytesToWrite;
    partSize -= bytesToWrite;
    loopCount++;
    // printf("Part %llu: Read %d bytes, Wrote %llu bytes, %llu bytes left.\n", partIndex, bytesRead, bytesWritten, partSize);
  }
  printf("Part %llu finished after %d loops.\n", partIndex, loopCount);
  // 清理资源
  _FileSplitting_DestroyThreadParam(arg);
  pClose(fd);
  free(buffer);
  partFs->functions->close(partFs);
}

/**
 * 寻找切割点，确保在数字后面切割
 * @warning 单线程使用，不要多线程调用
 * @param fs 文件系统
 * @param pos 初始位置
 * @return 返回找到的切割点位置
 */
ull _FileSplitting_Split_FindCuttingPoint(FileSystem fs, ull pos)
{
  FILE* fp = fs->functions->getFilePointer(fs);
  ull elemSize = sizeof(char);

  char buf[128]; // 每个整数的最大长度不可能超过unsigned long long的字符串长度，否则溢出
  fs->functions->seekLargeFile(fs, pos, FileSystem_SeekStart); // 先定位
  while (1) // 循环读取，直到找到切割点
  {
    ull got = fread(buf, elemSize, sizeof(buf), fp);
    if (got <= 0)
      return pos; // EOF
    for (ull i = 0; i < got; ++i)
      if (buf[i] == '\n' || buf[i] == ' ' || buf[i] == '\t')
        return pos + i + 1; // 找到切割点
    pos += got;
  }
}
/**
 * 多线程文件分割线程控制器，管理线程池
 * @param fs 需要分割的文件系统对象
 * @param outputDirectory 分割后文件的输出目录
 * @param outputPartPrefix 分割后文件的前缀
 * @param outputPartExtension 分割后文件的扩展名
 * @param startIndex 分割后文件的起始索引
 * @param partMaxSize 每个分割文件的最大大小（近似值，实际大小可能略有偏差）
 * @param threadCount 线程数量
 * @return 返回线程数组，全部失败返回NULL，部分失败返回部分线程，全部成功返回全部线程，可通过遍历线程数组获取成功数量进行比较判断
 * @note 返回的线程数组需要调用者手动释放
 * @warning 请确保fs对象在所有线程结束前不会被销毁
 */
ThreadPool _FileSplitting_SplitThreadController(FileSystem fs, String outputDirectory, String outputPartPrefix, String outputPartExtension, ull startIndex, ull partMaxSize, ThreadCount threadCount)
{
  // 计算每个分割文件的大小
  fs->functions->seekLargeFile(fs, 0, FileSystem_SeekEnd);
  ull fileSize = fs->functions->tellLargeFile(fs);
  fs->functions->seekLargeFile(fs, 0, FileSystem_SeekStart); // 回到文件开头
  // 计算每个线程大概需要处理的文件大小
  ull splittedFileSize = fileSize / threadCount;
  if (splittedFileSize > partMaxSize)
    splittedFileSize = partMaxSize;
  if (splittedFileSize == 0) // 文件太小，没必要使用那么多线程
    splittedFileSize = fileSize;
  // 计算实际需要的任务个数
  int taskCount = (fileSize + splittedFileSize - 1) / splittedFileSize;
  // 创建线程池
  ThreadPool pool = ThreadPool_Create(threadCount);
  if (!pool) {
    free(pool);
    return NULL;
  }
  // 计算每个任务需要分割的文件位置以及大小等参数信息
  ull partStart = 0;
  for (int i = 0; i < taskCount; i++)
  {
    if (fileSize <= partStart)
      break; // 没有更多数据需要处理
    // 计算每个线程的起始位置和大小
    ull partSize = (i == taskCount - 1) ? (fileSize - partStart) : splittedFileSize;
    // 寻找切割点
    if (i != taskCount - 1) // 最后一个线程不需要寻找切割点
      partSize = _FileSplitting_Split_FindCuttingPoint(fs, partStart + partSize) - partStart;
    // 如果分割大小为0，说明没有数据需要处理，跳过
    if (partSize == 0)
      continue;
    // 创建线程参数
    ThreadParam *param = _FileSplitting_CreateThreadParam(fs, outputDirectory, outputPartPrefix, outputPartExtension, startIndex + i, partStart, partSize);
    if (!param) {
      ThreadPool_Destroy(pool);
      return NULL;
    }
    // 创建任务
    ThreadPoolTask task = ThreadPool_CreateTask(_FileSplitting_FileSplittingThreadProc, param);
    if (!task) {
      ThreadPool_Destroy(pool);
      return NULL;
    }
    // 提交任务到线程池
    ThreadPool_Submit(pool, task);
    // 更新下一个线程的起始位置
    partStart += partSize;
  }
  return pool;
}

typedef struct _FileSplitting_SplitAsyncWorkerParams
{
  FileSystem fs;    // 大文件的FileSystem对象
  String outputDir; // 分割后的文件目录
  String outputPartPrefix; // 分割后文件的前缀
  String outputPartExtension; // 分割后文件的扩展名
  ull startIndex;    // 当前分割文件的索引
  ull partSize;     // 当前分割文件的大小
  ThreadCount threadCount;
} _FileSplitting_SplitAsyncWorkerParams;

void* _FileSplitting_SplitAsyncWorker(void *arg)
{
  _FileSplitting_SplitAsyncWorkerParams *params = (_FileSplitting_SplitAsyncWorkerParams *)arg;
  ThreadPool pool = _FileSplitting_SplitThreadController(
    params->fs,
    params->outputDir,
    params->outputPartPrefix,
    params->outputPartExtension,
    params->startIndex,
    params->partSize,
    params->threadCount
  );
  if (pool) {
    ThreadPool_Wait(pool);
    ThreadPool_Destroy(pool);
  }
  free(params);
  return NULL;
}
void FileSplitting_SplitAsync(FileSystem fs, String outputDirectory, String outputPartPrefix, String outputPartExtension, ull startIndex, ull partMaxSize, ThreadCount threadCount)
{
  // 启动分离线程执行任务
  pthread_t thread;
  // 创建线程参数
  _FileSplitting_SplitAsyncWorkerParams *params = malloc(sizeof(_FileSplitting_SplitAsyncWorkerParams));
  if (!params) {
    // 处理内存分配失败
    assert(0 && "Memory allocation failed");
    return;
  }
  memset(params, 0, sizeof(_FileSplitting_SplitAsyncWorkerParams));
  // 初始化线程参数
  params->fs = fs;
  params->outputDir = outputDirectory;
  params->outputPartPrefix = outputPartPrefix;
  params->outputPartExtension = outputPartExtension;
  params->startIndex = startIndex;
  params->partSize = partMaxSize;
  params->threadCount = threadCount;
  pthread_create(&thread, NULL, _FileSplitting_SplitAsyncWorker, (void*)params);
  pthread_detach(thread); // 分离线程，自动回收资源
}

void FileSplitting_SplitSync(FileSystem fs, String outputDirectory, String outputPartPrefix, String outputPartExtension, ull startIndex, ull partMaxSize, ThreadCount threadCount)
{
  // 启动分割任务
  ThreadPool pool = _FileSplitting_SplitThreadController(fs, outputDirectory, outputPartPrefix, outputPartExtension, startIndex, partMaxSize, threadCount);
  if (!pool)
    return;
  // 等待所有线程完成
  ThreadPool_Wait(pool);
  // 释放线程池
  ThreadPool_Destroy(pool);
}