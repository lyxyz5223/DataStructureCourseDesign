#ifndef FILE_SORT_H
#define FILE_SORT_H
#include "../Types.h"
#include "../FileSystem/FileSystem.h"
#include "../String/String.h"
#include "../MemoryMap/MemoryMap.h"
#include "../Array/Array.h"

typedef int SortDataElement; // 单个数据大小，目前是整型

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * 对小文件进行排序
   * @param filePath 小文件路径
   * @param cmp 比较器函数
   */
  void SortSmallFile(String filePath, ConstComparator cmp);

  /**
   * 合并多个已排序的小文件为一个大文件
   * @param partFilePaths 已排序的小文件路径数组，元素是const char*
   * @param partFileCount 小文件数量
   * @param outputFilePath 合并后大文件的路径
   * @param tempFileFolder 临时文件夹路径
   * @param availableMemory 可用内存大小（字节）
   * @param bufferSize 分块缓冲区大小（字节），一般取每个小文件大小
   * @param cmp 比较器函数
   */
  void MergeSortedFiles(const char** partFilePaths, ull partFileCount, String outputFilePath, String tempFileFolder, ull availableMemory, ull bufferSize, ConstComparator cmp);

#ifdef __cplusplus
}
#endif
#endif // FILE_SORT_H