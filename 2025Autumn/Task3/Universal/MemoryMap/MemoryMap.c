#include "MemoryMap.h"
#include <stdio.h>
#include <string.h>

MemoryMapFile MemoryMap_Base_Open(const char *path, lboolean write)
{
#ifdef _WIN32
  DWORD access = write ? GENERIC_READ | GENERIC_WRITE : GENERIC_READ;
  DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;
  HANDLE h = CreateFileA(path, access, share, NULL,
                         OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  return (h == INVALID_HANDLE_VALUE) ? MEMORY_MAP_FILE_INVALID : h;
#else
  int flags = write ? O_RDWR : O_RDONLY;
  return open(path, flags);
#endif
}

void *MemoryMap_Base_Map(MemoryMapFile fd, size_t *len, lboolean write)
{
  if (fd == MEMORY_MAP_FILE_INVALID)
    return NULL;
#ifdef _WIN32
  LARGE_INTEGER li;
  if (!GetFileSizeEx(fd, &li))
    return NULL;
  *len = (size_t)li.QuadPart;
  HANDLE mapping = CreateFileMappingW(fd, NULL,
                                      write ? PAGE_READWRITE : PAGE_READONLY,
                                      0, 0, NULL);
  if (!mapping)
    return NULL;
  void *base = MapViewOfFile(mapping,
                             write ? FILE_MAP_WRITE : FILE_MAP_READ,
                             0, 0, *len);
  CloseHandle(mapping); // 句柄可立即关闭
  return base;
#else
  *len = lseek(fd, 0, SEEK_END);
  int prot = write ? PROT_READ | PROT_WRITE : PROT_READ;
  return mmap(NULL, *len, prot, MAP_PRIVATE, fd, 0);
#endif
}

void MemoryMap_Base_Unmap(void *base, size_t len)
{
  if (!base)
    return;
#ifdef _WIN32
  UnmapViewOfFile(base);
#else
  munmap(base, len);
#endif
}

void MemoryMap_Base_Close(MemoryMapFile fd)
{
  if (fd == MEMORY_MAP_FILE_INVALID)
    return;
#ifdef _WIN32
  CloseHandle(fd);
#else
  close(fd);
#endif
}

void MemoryMap_Base_Sync(MemoryMapFile fd, void *base, size_t len)
{
  if (!base)
    return;
#ifdef _WIN32
  FlushViewOfFile(base, len);
  FlushFileBuffers(fd);
#else
  msync(base, len, MS_SYNC);
#endif
}



// 包装函数

MemoryMap MemoryMap_Open(const char *path, lboolean write)
{
  MemoryMap mm;
  mm.fd = MemoryMap_Base_Open(path, write);
  mm.base = 0;
  mm.length = 0;
  return mm;
}

void *MemoryMap_Map(MemoryMap* mm, lboolean write)
{
  if (!mm)
    return NULL;
  void* base = MemoryMap_Base_Map(mm->fd, &mm->length, write);
  mm->base = base;
  return base;
}

void MemoryMap_Unmap(MemoryMap* mm)
{
  if (!mm)
    return;
  MemoryMap_Base_Unmap(mm->base, mm->length);
  mm->base = NULL;
  mm->length = 0;
}

void MemoryMap_Sync(MemoryMap mm)
{
  MemoryMap_Base_Sync(mm.fd, mm.base, mm.length);
}

void MemoryMap_Close(MemoryMap* mm)
{
  if (!mm)
    return;
  MemoryMap_Base_Unmap(mm->base, mm->length);
  MemoryMap_Base_Close(mm->fd);
  mm->base = NULL;
  mm->length = 0;
  mm->fd = MEMORY_MAP_FILE_INVALID;
}