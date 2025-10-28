#include "FileSystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

typedef struct _FileSystem
{
  FileSystemApi *functions;
  FILE *filePtr;
  char *filePath;
  char *mode;
} *_FileSystem;
static FileSystemApi _FileSystem_Global_Api;


lboolean FileSystem_Open(FileSystem fs, const char *path, const char *mode);
lboolean FileSystem_Close(FileSystem fs);

FileSystem FileSystem_Create(const char *path, const char *mode)
{
  _FileSystem fs = malloc(sizeof(struct _FileSystem));
  if (!fs) return NULL;
  fs->filePtr = NULL;
  fs->filePath = NULL;
  fs->mode = NULL;
  fs->functions = &_FileSystem_Global_Api;
  if (path && !FileSystem_Open((FileSystem)fs, path, mode ? mode : "r"))
  {
    free(fs);
    return NULL;
  }
  return (FileSystem)fs;
}

FileSystemApi* FileSystem_GetApi()
{
  return &_FileSystem_Global_Api;
}

lboolean FileSystem_Destroy(FileSystem fs)
{
  if (!fs)
    return false;
  _FileSystem _fs = (_FileSystem)fs;
  if (_fs->filePtr) // 如果有文件打开，则关闭文件
    FileSystem_Close(fs);
  if (_fs->filePath) // 释放路径字符串内存
    free(_fs->filePath);
  if (_fs->mode) // 释放模式字符串内存
    free(_fs->mode);
  // 指针不需要释放，因为是全局对象，静态分配内存
  // if (_fs->functions) // 释放函数指针内存
  //   free(_fs->functions);
  free(_fs); // 释放对象内存
  return true;
}

lboolean FileSystem_Open(FileSystem fs, const char *path, const char *mode)
{
  if (!fs || !path)
    return false;
  _FileSystem _fs = (_FileSystem)fs;
  // 如果已经有文件打开，则先关闭文件
  if (_fs->filePtr)
    FileSystem_Close(fs);
  _fs->filePtr = fopen(path, mode);
  // 如果内部数据存在，则先释放内存
  if (_fs->filePath)
    free(_fs->filePath);
  if (_fs->mode)
    free(_fs->mode);
  // 分配内存并保存路径和模式字符串
  _fs->filePath = strcpy(malloc(strlen(path) + 1), path);
  _fs->mode = strcpy(malloc(strlen(mode) + 1), mode);
  return _fs->filePtr ? true : false; // 返回是否成功打开文件
}


lboolean FileSystem_Close(FileSystem fs)
{
  if (!fs)
    return false;
  // 仅关闭文件，不释放内部数据内存
  int res = fclose(((_FileSystem)fs)->filePtr);
  return res ? false : true;
}

ull FileSystem_Read(FileSystem fs, void *buffer, ull size, ull count)
{
  if (!fs || !buffer || size == 0 || count == 0)
    return 0;
  _FileSystem _fs = (_FileSystem)fs;
  if (!_fs->filePtr)
    return 0;
  return fread(buffer, size, count, _fs->filePtr);
}
ull FileSystem_Write(FileSystem fs, const void *buffer, ull size, ull count)
{
  if (!fs || !buffer || size == 0 || count == 0)
    return 0;
  _FileSystem _fs = (_FileSystem)fs;
  if (!_fs->filePtr)
    return 0;
  return fwrite(buffer, size, count, _fs->filePtr);
}
#ifdef _WIN32
lboolean FileSystem_FileExists(const char *filePath)
{
  DWORD attr = GetFileAttributesA(filePath);
  return attr != INVALID_FILE_ATTRIBUTES &&
         !(attr & FILE_ATTRIBUTE_DIRECTORY);
}
lboolean FileSystem_SeekLargeFile(FileSystem fs, ll offset, FileSystemSeekConstant origin)
{
  if (!fs)
    return false;
  _FileSystem _fs = (_FileSystem)fs;
  if (!_fs->filePtr)
    return false;
  return _fseeki64(_fs->filePtr, offset, origin) ? false : true;
}
ull FileSystem_TellLargeFile(FileSystem fs)
{
  if (!fs)
    return (ull)-1;
  _FileSystem _fs = (_FileSystem)fs;
  if (!_fs->filePtr)
    return (ull)-1;
  return (ull)_ftelli64(_fs->filePtr);
}
#else
lboolean FileSystem_FileExists(const char *filePath)
{
  return access(filePath, F_OK) == 0;
}
lboolean FileSystem_SeekLargeFile(FileSystem fs, ll offset, FileSystemSeekConstant origin)
{
  if (!fs)
    return false;
  _FileSystem _fs = (_FileSystem)fs;
  if (!_fs->filePtr)
    return false;
  return fseeko(_fs->filePtr, offset, origin) ? false : true;
}
ull FileSystem_TellLargeFile(FileSystem fs)
{
  if (!fs)
    return (ull)-1;
  _FileSystem _fs = (_FileSystem)fs;
  if (!_fs->filePtr)
    return (ull)-1;
  return (ull)ftello(_fs->filePtr);
}
#endif // _WIN32

lboolean FileSystem_SeekSmallFile(FileSystem fs, long offset, FileSystemSeekConstant origin)
{
  if (!fs)
    return false;
  _FileSystem _fs = (_FileSystem)fs;
  if (!_fs->filePtr)
    return false;
  return fseek(_fs->filePtr, offset, origin) ? false : true;
}
long FileSystem_TellSmallFile(FileSystem fs)
{
  if (!fs)
    return -1;
  _FileSystem _fs = (_FileSystem)fs;
  if (!_fs->filePtr)
    return -1;
  return ftell(_fs->filePtr);
}
lboolean FileSystem_Eof(FileSystem fs)
{
  if (!fs)
    return false;
  _FileSystem _fs = (_FileSystem)fs;
  if (!_fs->filePtr)
    return false;
  return feof(_fs->filePtr);
}
lboolean FileSystem_Truncate(FileSystem fs)
{
  // 清空文件内容
  _FileSystem _fs = (_FileSystem)fs;
  _fs->filePtr = freopen(_fs->filePath, "w", _fs->filePtr);
  if (_fs->filePtr)
  {
    // 成功清空文件内容
    // 重新用原来的模式打开文件
    _fs->filePtr = freopen(_fs->filePath, _fs->mode, _fs->filePtr);
    return _fs->filePtr ? true : false;
  }
  else
    return false;
}

lboolean FileSystem_Error(FileSystem fs)
{
  if (!fs)
    return false;
  _FileSystem _fs = (_FileSystem)fs;
  if (!_fs->filePtr)
    return false;
  return ferror(_fs->filePtr) ? true : false;
}
void FileSystem_ClearError(FileSystem fs)
{
  if (!fs)
    return;
  _FileSystem _fs = (_FileSystem)fs;
  if (!_fs->filePtr)
    return;
  clearerr(_fs->filePtr);
}

static FileSystemApi _FileSystem_Global_Api = {
  .create = FileSystem_Create,
  .destroy = FileSystem_Destroy,
  .open = FileSystem_Open,
  .close = FileSystem_Close,
  .read = FileSystem_Read,
  .write = FileSystem_Write,
  .fileExists = FileSystem_FileExists,
  .seekSmallFile = FileSystem_SeekSmallFile,
  .tellSmallFile = FileSystem_TellSmallFile,
  .seekLargeFile = FileSystem_SeekLargeFile,
  .tellLargeFile = FileSystem_TellLargeFile,
  .eof = FileSystem_Eof,
  .truncate = FileSystem_Truncate,
  .error = FileSystem_Error,
  .clearError = FileSystem_ClearError
};