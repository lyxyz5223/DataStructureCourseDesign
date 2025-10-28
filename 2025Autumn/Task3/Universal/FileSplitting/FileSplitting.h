#ifndef FILE_SPLITTING_H
#define FILE_SPLITTING_H

#include "../FileSystem/FileSystem.h"
#include "../String/String.h"

typedef int ThreadCount;
/**
 * 异步多线程文件分割
 * @param fs 需要分割的文件系统对象
 * @param outputDirectory 分割后文件的输出目录
 * @param outputPartPrefix 分割后文件的前缀
 * @param outputPartExtension 分割后文件的扩展名
 * @param startIndex 分割后文件的起始索引
 * @param partMaxSize 每个分割文件的最大大小（近似值，实际大小可能略有偏差）
 * @param threadCount 线程数量
 * @note 请注意fs对象的生命周期，确保在所有线程结束前不会被销毁
 */
void FileSplitting_SplitAsync(FileSystem fs,
  String outputDirectory,
  String outputPartPrefix,
  String outputPartExtension,
  ull startIndex,
  ull partMaxSize, // 近似值，实际大小可能略有偏差
  ThreadCount threadCount
);

/**
 * 同步多线程文件分割
 * @param fs 需要分割的文件系统对象
 * @param outputDirectory 分割后文件的输出目录
 * @param outputPartPrefix 分割后文件的前缀
 * @param outputPartExtension 分割后文件的扩展名
 * @param startIndex 分割后文件的起始索引
 * @param partMaxSize 每个分割文件的最大大小（近似值，实际大小可能略有偏差）
 * @param threadCount 线程数量
 * @note 请注意fs对象的生命周期，确保在所有线程结束前不会被销毁
 */
void FileSplitting_SplitSync(
  FileSystem fs,
  String outputDirectory,
  String outputPartPrefix,
  String outputPartExtension,
  ull startIndex,
  ull partMaxSize, // 近似值，实际大小可能略有偏差
  ThreadCount threadCount
);

#endif // FILE_SPLITTING_H