#include "FileSystem.h"
#include "FileSplitting.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>

int main()
{
  // 创建文件夹
  int createDirResult = mkdir(".\\TestSplit");
  FileSystem fs = FileSystem_Create(".\\BigData.txt", "rb");
  String outputDir = String_Create(".\\TestSplit\\", 0, 1);
  String outputPartPrefix = String_Create("part_", 0 ,1);
  String outputPartExtension = String_Create(".txt", 0, 1);
  ull startIndex = 1;
  // 分割后每个小文件的最大大小
  ull partMaxSize = 1024 * 1024 * 3; // 按照字节计算， 1024 * 1024 * 3 = 3MB
  ThreadCount threadCount = 64; // 线程数量

  FileSplitting_SplitSync(fs, outputDir, outputPartPrefix, outputPartExtension, startIndex, partMaxSize, threadCount);
  printf("文件分割完成!\n");
  // 清理资源
  StringApi* strApi = outputDir->functions;
  strApi->destroy(outputDir);
  strApi->destroy(outputPartPrefix);
  strApi->destroy(outputPartExtension);
  fs->functions->close(fs);
  return 0;
}