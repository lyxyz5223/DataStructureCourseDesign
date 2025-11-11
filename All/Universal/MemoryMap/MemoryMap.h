#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include "../types.h"
#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
typedef HANDLE MemoryMapFile;
#define MEMORY_MAP_FILE_INVALID 0
#else
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
typedef int MemoryMapFile;
#define MEMORY_MAP_FILE_INVALID -1
#endif

typedef struct MemoryMap
{
  MemoryMapFile fd; // 文件句柄
  void *base; // 映射基址
  size_t length; // map的时候会设置这个值，表示映射长度
} MemoryMap;

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * 打开一个文件并返回文件句柄
   * @param path 文件路径
   * @param write 是否以写模式打开
   * @return 返回文件句柄，失败返回MEMORY_MAP_FILE_INVALID
   */
  MemoryMapFile MemoryMap_Base_Open(const char *path, lboolean write);

  /**
   * 映射文件到内存
   * @param fd MemoryMap结构体
   * @param len 输出参数，返回映射长度
   * @param write 是否启用写模式（读写映射）
   * @return 返回映射基址，失败返回NULL
   */
  void *MemoryMap_Base_Map(MemoryMapFile fd, size_t *len, lboolean write);

  /**
   * 解除映射
   * @param base 映射基址
   * @param len 映射长度
   */
  void MemoryMap_Base_Unmap(void *base, size_t len);

  /**
   * 关闭文件句柄
   * @param fd 文件句柄
   */
  void MemoryMap_Base_Close(MemoryMapFile fd);

  /**
   * 同步内存到文件
   * @param fd 文件句柄
   * @param base 映射基址
   * @param len 映射长度
   */
  void MemoryMap_Base_Sync(MemoryMapFile fd, void *base, size_t len);



  // 包装函数

  /**
   * 打开一个文件并返回文件句柄
   * @param path 文件路径
   * @param write 是否以写模式打开
   * @return 返回MemoryMap, 失败设置结构中的fd为MEMORY_MAP_FILE_INVALID
   */
  MemoryMap MemoryMap_Open(const char *path, lboolean write);

  /**
   * 映射文件到内存
   * @param mm MemoryMap结构体指针
   * @param write 是否启用写模式（读写映射）
   * @return 返回映射基址，失败返回NULL
   */
  void *MemoryMap_Map(MemoryMap* mm, lboolean write);
  /**
   * 解除映射
   * @param mm MemoryMap结构体指针
   */
  void MemoryMap_Unmap(MemoryMap* mm);

  /**
   * 包装函数：同步内存到文件
   * @param mm MemoryMap结构体
   */
  void MemoryMap_Sync(MemoryMap mm);
  /**
   * 解除映射并关闭文件
   * @param mm MemoryMap结构体指针
   */
  void MemoryMap_Close(MemoryMap* mm);

#ifdef __cplusplus
}
#endif
#endif // MEMORY_MAP_H
