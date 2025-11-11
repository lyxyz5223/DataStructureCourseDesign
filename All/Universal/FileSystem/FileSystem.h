#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
#include "../types.h"
#include "../Array/Array.h"
#include <stdio.h>

// 系统调用:
// 跨平台pread和pwrite
#ifdef _WIN32
#include <windows.h>
#include <inttypes.h>
typedef HANDLE fd_t;
#define FD_INVALID INVALID_HANDLE_VALUE
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
typedef int fd_t;
#define FD_INVALID -1
#endif
int pRead(fd_t h, void *buf, int len, int64_t off);
int pWrite(fd_t h, const void *buf, int len, int64_t off);
fd_t pOpenReadOnly(const char *path);
void pClose(fd_t fd);


// typedef struct FILE FILE;
typedef struct FileSystemApi FileSystemApi;
typedef struct FileSystem *FileSystem;
typedef struct FileSystem
{
  FileSystemApi *functions;
} *FileSystem;
typedef enum FileSystemSeekConstant
{
  FileSystem_SeekStart = SEEK_SET,
  FileSystem_SeekCurrent = SEEK_CUR,
  FileSystem_SeekEnd = SEEK_END
} FileSystemSeekConstant;

typedef struct FileSystemApi
{
  /**
   * 创建一个FileSystem对象并打开指定路径的文件
   * @param path 文件路径，如果为空则不会默认打开一个文件
   * @param mode 打开模式，参考fopen的mode参数，如果为空则默认为"r"
   * @return 返回创建的FileSystem对象指针，失败返回NULL
   */
  FileSystem (*create)(const char* path, const char* mode);
  /**
   * 销毁FileSystem对象，释放内存
   * @param fs 需要销毁的FileSystem对象指针
   * @return true成功，false失败
   */
  lboolean (*destroy)(FileSystem fs);
  /**
   * 获取FILE指针
   * @param fs FileSystem对象
   * @return 返回对应的FILE指针
   * @note 请不要直接操作FILE指针，除非你非常清楚自己在做什么
   */
  FILE* (*getFilePointer)(FileSystem fs);
  /**
   * 获取文件路径
   * @param fs FileSystem对象
   * @return 返回文件路径字符串
   */
  const char* (*getFilePath)(FileSystem fs);
  /**
   * 打开一个文件
   * @param path 文件路径
   * @param mode 打开模式，参考fopen的mode参数，如果为空则默认为"r"
   * @return true成功，false失败
   * @note fopen的mode参数说明:
          "r"  打开一个用于读取的文件。该文件必须存在。
          "w"  创建一个用于写入的空文件。如果文件名称与已存在的文件相同，则会删除已有文件的内容，文件被视为一个新的空文件。
          "a"  追加到一个文件。写操作向文件末尾追加数据。如果文件不存在，则创建文件。
          "r+" 打开一个用于更新的文件，可读取也可写入。该文件必须存在。
          "w+" 创建一个用于读写的空文件。
          "a+" 打开一个用于读取和追加的文件。
   */
  lboolean (*open)(FileSystem fs, const char* path, const char* mode);
  /**
   * 关闭文件
   * @param fs FileSystem对象
   * @return true 成功，false 失败
   */
  lboolean (*close)(FileSystem fs);
  /**
   * 读取文件内容
   * @param fs FileSystem对象
   * @param buffer 缓冲区指针，用于保存读取到的数据
   * @param size 每个元素的字节数
   * @param count 读取的元素个数
   * @return 实际读取到的元素个数，如果读取完成，返回值 < count
   * @note 最后一次读取可能出现: 返回值 == count 的情况，只需再读取一次，将返回0
   */
  ull (*read)(FileSystem fs, void* buffer, ull size, ull count);
  /**
   * 写入文件
   * @param fs FileSystem对象
   * @param buffer 写入的数据缓冲区指针
   * @param size 每个元素的字节数
   * @param count 写入的元素个数
   * @return 实际写入的元素个数，写入失败返回0
   */
  ull (*write)(FileSystem fs, const void* buffer, ull size, ull count);
  /**
   * 判断文件是否存在
   * @param filePath 需要判断的文件路径（含文件名）
   * @return true 文件存在，false 文件不存在
   */
  lboolean (*fileExists)(const char* filePath);
  /**
   * 移动小文件指针
   * @param fs FileSystem对象
   * @param offset 偏移量
   * @param origin 起始位置
   * @return true 成功，false 失败
   */
  lboolean (*seekSmallFile)(FileSystem fs, long offset, FileSystemSeekConstant origin);
  /**
   * 移动大文件的文件指针
   * @param fs FileSystem对象
   * @param offset 偏移量
   * @param origin 起始位置
   * @return true 成功，false 失败
   */
  lboolean (*seekLargeFile)(FileSystem fs, ll offset, FileSystemSeekConstant origin);
  /**
   * 获取当前小文件指针距离文件开头的字节数
   * @param fs FileSystem对象
   * @return 返回当前指针位置
   */
  long (*tellSmallFile)(FileSystem fs);
  /**
   * 获取当前大文件指针距离文件开头的字节数
   * @param fs FileSystem对象
   * @return 返回当前指针位置
   */
  ull (*tellLargeFile)(FileSystem fs);
  /**
   * 判断文件是否到达了末尾(end of file)
   * @return true 文件指针到达文件末尾，false 文件指针未到达文件末尾
   * @note 只有文件指针在FileSystem_SeekEnd并再次读取才会为true
   */
  lboolean (*eof)(FileSystem fs);
  /**
   * 清空文件内容
   * @note 清空文件内容与打开文件的方式无关，届时将重新打开文件
   * @param fs FileSystem对象
   * @return true 成功，false 失败
   */
  lboolean (*truncate)(FileSystem fs);
  /**
   * 文件操作过程是否发生了错误
   * @param fs FileSystem对象
   * @return true 发生错误，false 没有发生错误
   */
  lboolean (*error)(FileSystem fs);
  /**
   * 清除错误标志
   * @param fs FileSystem对象
   */
  void (*clearError)(FileSystem fs);
  /**
   * 获取路径分隔符
   */
  char (*getPathSeparator)(void);
} FileSystemApi;

/**
 * 创建一个FileSystem对象并打开指定路径的文件
 * @param path 文件路径，如果为空则不会默认打开一个文件
 * @param mode 打开模式，参考fopen的mode参数，如果为空则默认为"r"
 * @return 返回创建的FileSystem对象指针，失败返回NULL
 * @note fopen的mode参数说明:
          "r"  打开一个用于读取的文件。该文件必须存在。
          "w"  创建一个用于写入的空文件。如果文件名称与已存在的文件相同，则会删除已有文件的内容，文件被视为一个新的空文件。
          "a"  追加到一个文件。写操作向文件末尾追加数据。如果文件不存在，则创建文件。
          "r+" 打开一个用于更新的文件，可读取也可写入。该文件必须存在。
          "w+" 创建一个用于读写的空文件。
          "a+" 打开一个用于读取和追加的文件。
 */
FileSystem FileSystem_Create(const char* path, const char* mode);

/**
 * 获取FileSystem的全局API接口
 * @return 返回FileSystemApi指针
 * @warning 请不要修改返回的接口内容或者释放内存
 */
const FileSystemApi* FileSystem_GetApi();

#endif // FILE_SYSTEM_H
