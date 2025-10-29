#include <stdio.h>
#include <stdlib.h>
#include "Universal/FileSystem/FileSystem.h"
#include "Universal/FileSplitting/FileSplitting.h"
#include "Universal/String/String.h"
#include "Universal/FileSort/FileSort.h"
#include "Universal/ThreadPool/ThreadPool.h"
#include <io.h>
#include <time.h>
#include "Universal/Array/Array.h"

int SortPartComparator(const void* a, const void* b)
{
  const SortDataElement elemA = *(const SortDataElement*)a;
  const SortDataElement elemB = *(const SortDataElement*)b;
  return elemA - elemB;
}

void SortPart(String filePath)
{
  if (!filePath)
    return;
  SortSmallFile(filePath, SortPartComparator);
}
int MergeSortedFilesComparator(const void *a, const void *b)
{
  SortDataElement valA = *(const SortDataElement *)a;
  SortDataElement valB = *(const SortDataElement *)b;
  if (valA < valB)
    return -1;
  else if (valA > valB)
    return 1;
  else
    return 0;
}

void PathEnsureEndsWithSeparator(String path)
{
  StringApi* strApi = path->functions;
  ull len = strApi->length(path);
  if (len == 0)
    return;
  char lastChar = strApi->get(path, len - 1);
  if (lastChar != '/' && lastChar != '\\')
  {
    strApi->appendChar(path, FileSystem_GetApi()->getPathSeparator());
  }
}

ull ReadConsoleLine(String output)
{
  if (!output)
    return 0;
  StringApi* strApi = output->functions;
  char tmpChar = EOF;
  while ((tmpChar = getchar()) != '\n' && tmpChar != EOF)
  {
    if (!strApi->appendChar(output, tmpChar))
      return 0; // 内存分配失败
  }
  return strApi->length(output);
}

// 主函数
int main()
{
  const StringApi* strApi = String_GetApi();
  const FileSystemApi* fsApi = FileSystem_GetApi();
  const ArrayApi *arrApi = Array_GetApi();
#ifdef _DEBUG
  // 指定一个大文件路径
  // String bigFilePath = String_Create("E:\\C++\\BigData3 (2).txt", 0, 1);
  String bigFilePath = String_Create("E:\\C++\\BigData.txt", 0, 1);
  // 指定一个临时文件目录
  String tempDir = String_Create("E:\\C++\\temp\\", 0, 1);
  // 指定一个输出文件名
  String outputFilePath = String_Create("E:\\C++\\output.txt", 0, 1);
#else
  String bigFilePath = String_Create("", 0, 0);
  printf("请指定一个大文件路径: ");
  if (ReadConsoleLine(bigFilePath) == 0)
  {
    printf("文件路径不能为空!\n");
    strApi->destroy(bigFilePath);
    return 1;
  }
  // 指定一个临时文件目录
  String tempDir = String_Create("", 0, 0);
  printf("请指定一个临时文件目录: ");
  if (ReadConsoleLine(tempDir) == 0)
  {
    printf("临时文件目录不能为空!\n");
    strApi->destroy(tempDir);
    return 2;
  }
  PathEnsureEndsWithSeparator(tempDir); // 确保目录路径以分隔符结尾
  // 指定一个输出文件名
  String outputFilePath = String_Create("", 0, 0);
  printf("请指定一个输出文件名: ");
  if (ReadConsoleLine(outputFilePath) == 0)
  {
    printf("输出文件名不能为空!\n");
    strApi->destroy(outputFilePath);
    return 3;
  }
#endif
  // 创建临时目录（如果不存在的话）
  mkdir(strApi->toCString(tempDir));
  // 打开大文件
  FileSystem fs = FileSystem_Create(strApi->toCString(bigFilePath), "r");
  if (!fs)
  {
    printf("无法打开文件: %s\n", strApi->toCString(bigFilePath));
    strApi->destroy(bigFilePath);
    strApi->destroy(tempDir);
    return 4;
  }
  // 获取文件大小
  if (!fs->functions->seekLargeFile(fs, 0, SEEK_END))
  {
    printf("无法定位文件末尾!\n");
    fs->functions->close(fs);
    strApi->destroy(bigFilePath);
    strApi->destroy(tempDir);
    return 5;
  }
  ull fileSize = fs->functions->tellLargeFile(fs);
  if (fileSize == (ull)-1)
  {
    printf("无法获取文件大小!\n");
    fs->functions->close(fs);
    strApi->destroy(bigFilePath);
    strApi->destroy(tempDir);
    return 6;
  }
  // TODO: 删除：计算元素个数--------------------------------------
  printf("[Debug] Test: 通过读取行数计算元素个数... wait...\n");
  MemoryMap mm = MemoryMap_Open(strApi->toCString(bigFilePath), 0);
  MemoryMap_Map(&mm, 0);
  if (mm.fd == MEMORY_MAP_FILE_INVALID)
  {
    printf("内存映射创建失败!\n");
    fs->functions->close(fs);
    strApi->destroy(bigFilePath);
    strApi->destroy(tempDir);
    return 7;
  }
  
  ull elemCount = 0;
  char *endPtr = mm.base + mm.length;
  for (char *p = mm.base; p < endPtr; p++)
  {
    if (*p == '\n')
      elemCount++;
  }
  MemoryMap_Unmap(&mm);
  MemoryMap_Close(&mm);


  // 多线程分割文件
  // const ull MAX_PART_SIZE = 100 * 1024 * 1024; // 每个小文件最大100MB
  const ull MAX_PART_SIZE = 128; // 每个小文件最大128字节
  const int THREAD_COUNT = 8; // 多线程
  const int PART_START_INDEX = 1; // 分割文件起始索引

  // 可有可无，这里仅仅用于提供信息给用户
  {                                              // 利用大文件大小计算分割数量
    ull partCount = fileSize / MAX_PART_SIZE;    // 计算整除部分
    if (fileSize % MAX_PART_SIZE != 0)           // 有余数则需要多一个文件
      partCount++;
    // 计算每个小文件的大小，直接均分
    ull splittedFileSize = fileSize / partCount;
    printf("文件大小: %llu 字节, 预计分割为 %llu 个小文件, 每个小文件大小: %llu 字节.\n", fileSize, partCount, splittedFileSize);
    // 重置文件指针到开头
    if (!fs->functions->seekLargeFile(fs, 0, SEEK_SET))
    {
      printf("无法重置文件指针到开头!\n");
      fs->functions->close(fs);
      bigFilePath->functions->destroy(bigFilePath);
      tempDir->functions->destroy(tempDir);
      return 7;
    }
  }

  String outputFilePrefix = String_Create("part_", 0, 1);
  String outputFileExtension = String_Create(".txt", 0, 1);
  FileSplitting_SplitSync(fs, tempDir, outputFilePrefix, outputFileExtension, PART_START_INDEX, MAX_PART_SIZE, THREAD_COUNT);
  printf("文件分割完成!\n");
  // 对每个小文件进行排序
  int partIndex = PART_START_INDEX;
  ThreadPool sortPool = ThreadPool_Create(THREAD_COUNT);
  // 统计开始时间
  clock_t startTime = clock();
  // 记录每个小文件路径的数组
  Array* arrSmallFilePathsString = Array_Create(NULL, 0);
  Array* arrSmallFilePathsPtr = Array_Create(NULL, 0);
  while (1) // 无限循环，直到找不到下一个分割文件
  {
    // 构造小文件路径
    String partFilePath = strApi->copy(tempDir);
    strApi->appendString(partFilePath, outputFilePrefix);
    // 添加索引
    char indexBuffer[64];
    snprintf(indexBuffer, sizeof(indexBuffer), "%d", partIndex);
    strApi->appendCString(partFilePath, indexBuffer, 0, 1);
    strApi->appendString(partFilePath, outputFileExtension);
    // 检查文件是否存在
    if (!fs->functions->fileExists(strApi->toCString(partFilePath)))
    {
      // 文件不存在，说明已经处理完所有分割文件
      strApi->destroy(partFilePath);
      break;
    }
    // 提交排序任务到线程池
    ThreadPoolTask sortingTask = ThreadPool_CreateTask((void (*)(void *))SortPart, partFilePath);
    ThreadPool_Submit(sortPool, sortingTask);
    printf("已提交小文件排序任务: %s\n", strApi->toCString(partFilePath));
    partIndex++;
    // 记录小文件路径
    arrApi->append(arrSmallFilePathsString, (UniversalData)partFilePath);              // 保存的是指针，String本身也是指针
    arrApi->append(arrSmallFilePathsPtr, (UniversalData)strApi->toCString(partFilePath)); // 保存的是C字符串指针
  }
  // 等待所有排序任务完成
  ThreadPool_Wait(sortPool);
  ThreadPool_Destroy(sortPool);
  // 统计结束时间
  clock_t endTime = clock();
  double duration = (double)(endTime - startTime) / CLOCKS_PER_SEC;
  printf("所有小文件排序任务完成! 耗时: %.2f 秒\n", duration);
  // 合并排序后的小文件
  String tempDirKWay = strApi->copy(tempDir);
  strApi->appendCString(tempDirKWay, "kway_part", 0, 1);
  PathEnsureEndsWithSeparator(tempDirKWay);
  mkdir(strApi->toCString(tempDirKWay)); // 创建用于k路归并的临时目录（如果不存在的话）
  const char **paths = (const char **)arrApi->data(arrSmallFilePathsPtr);
  ull len = arrApi->length(arrSmallFilePathsPtr);
  ull bufferSize = 16 * 1024 * 1024; // 4MB缓冲区
  ull availableMemory = MAX_PART_SIZE * 3;
  if (bufferSize * 2 > availableMemory)
    bufferSize = availableMemory / 2 / 2 * 0.8;
  // ull bufferSize = 1024; // 1KB缓冲区
  // ull availableMemory = 4 * 1024;
  MergeSortedFiles(paths, len, outputFilePath, tempDirKWay, availableMemory, bufferSize, MergeSortedFilesComparator);
  printf("完成：合并排序后的小文件到: %s\n", strApi->toCString(outputFilePath));
  // 清理资源
  strApi->destroy(outputFilePrefix);
  strApi->destroy(outputFileExtension);
  fs->functions->close(fs);
  strApi->destroy(bigFilePath);
  strApi->destroy(tempDir);
  strApi->destroy(tempDirKWay);
  arrApi->destroy(arrSmallFilePathsPtr);
  for (Iterator it = arrApi->begin(arrSmallFilePathsString); !it->equals(it, arrApi->end(arrSmallFilePathsString)); it->next(it))
  {
    strApi->destroy(*it->dataPtr);
  }
  arrApi->destroy(arrSmallFilePathsString);
  // 统计结束时间
  endTime = clock();
  duration = (double)(endTime - startTime) / CLOCKS_PER_SEC;
  printf("文件排序任务完成! 耗时: %.2f 秒\n", duration);
  // TODO: 删除：输出统计数据------------------------------------------
  printf("实际需要排序的元素数: %llu", elemCount);
  // TODO: 删除：读取输出的结果文件用于统计---------------------------
  ull outputElemCount = 0;
  FileSystem outputFs = fsApi->create(strApi->toCString(outputFilePath), "r");
  if (outputFs)
  {
    char buffer[1024];
    ull readCount = 0;
    while ((readCount = outputFs->functions->read(outputFs, buffer, sizeof(char), 1024)) > 0)
    {
      for (ull i = 0; i < readCount; i++)
      {
        if (buffer[i] == '\n')
          outputElemCount++;
      }
    }
    outputFs->functions->close(outputFs);
    printf(", 输出结果文件中的元素数: %llu\n", outputElemCount);
  }
  return 0;
}
