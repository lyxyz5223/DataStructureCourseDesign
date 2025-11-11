#include "FileSort.h"
#include "../LoserTree/LoserTree.h"
#include "../MemoryMap/MemoryMap.h"
#include "../String/String.h"
#include <assert.h>
#include <stdlib.h>

SortDataElement *_FileSort_ReadMemoryMapData(size_t *dataElemCount, char *base, size_t mapLen)
{
  // 先大致计算一下可能有多少个数据
  // 根据数据类型计算出数据最长字符数，比如int类型4个字节，32位，最大值是2147483647，最长10个字符
  // 保守估计，所以直接包含符号位
  size_t dataElementMaxLen = 0;
  {
    SortDataElement maxVal = ~(SortDataElement)0; // 全1
    if (maxVal < 0)
    {
      dataElementMaxLen++; // 符号位
      maxVal = -maxVal;    // 负数取反
    }
    do
    {
      dataElementMaxLen++;
      maxVal /= 10;
    } while (maxVal != 0);
  }
  size_t dataElementCount = mapLen / dataElementMaxLen;
  // 数据数组
  SortDataElement *data = (SortDataElement *)malloc(dataElementCount * sizeof(SortDataElement));
  if (!data)
    return NULL; // 内存分配失败
  // ZeroMemory
  memset(data, 0, sizeof(SortDataElement) * dataElementCount);
  size_t dataIndex = 0;
  bool hasElementData = false; // 当前是否在读取数据
  bool isNegative = false;     // 当前数据是否为负数
  char *ptr = base;            // 当前解析位置
  char *end = base + mapLen;   // 映射结束位置
  for (; ptr < end; ptr++)
  {
    if (dataIndex >= dataElementCount)
    {
      // 重新分配更大的内存
      size_t newSize = dataElementCount * 2; // 翻倍
      SortDataElement *newData = (SortDataElement *)realloc(data, newSize * sizeof(SortDataElement));
      if (!newData)
        break; // 内存分配失败，跳出循环
      data = newData;
      memset(data + dataElementCount, 0, sizeof(SortDataElement) * dataElementCount);
      dataElementCount = newSize;
    }
    // 如果是数字或者-号，记录下来
    if (*ptr == '-')
      isNegative = true;
    else if (*ptr >= '0' && *ptr <= '9')
    {
      data[dataIndex] = data[dataIndex] * 10 + (*ptr - '0');
      hasElementData = true; // 标记读到了数据
    }
    else
    {
      // 不是数字，写入数据
      if (hasElementData)
      {
        if (isNegative)
          data[dataIndex] = -data[dataIndex];
        dataIndex++;
        // 重置状态
        hasElementData = false;
      }
    }
  }
  if (hasElementData && dataIndex < dataElementCount)
  {
    if (isNegative)
      data[dataIndex] = -data[dataIndex];
    dataIndex++;
    // 重置状态，虽然无用
    hasElementData = false;
  }
  *dataElemCount = dataIndex;
  return data;
}

SortDataElement *_FileSort_GetDataFromMemoryMap(void *base, size_t mapLen, size_t *dataElemCount)
{
  if (!base || mapLen == 0 || !dataElemCount)
  {
    assert(base != NULL && mapLen != 0 && dataElemCount != NULL);
    return 0;
  }
  return _FileSort_ReadMemoryMapData(dataElemCount, base, mapLen);
}

/**
 * 将排序后的数据按照原来的格式写回内存映射区域
 * @param data 排序后的数据数组
 * @param dataElemCount 数据元素数量
 * @param base 内存映射起始地址
 * @param mapLen 内存映射长度
 * @warning Coded by ChatGPT, may contain bugs, please review carefully.
 */
void _FileSort_WriteMemoryMapData(SortDataElement *data, size_t dataElemCount, char *base, size_t mapLen)
{
  if (!data || dataElemCount == 0 || !base || mapLen == 0)
    return;
  char *ptr = base;
  char *end = base + mapLen;

  while (ptr < end)
  {
    // 读取一个数据元素
    SortDataElement val = 0;
    bool isNegative = false;
    if (*ptr == '-')
    {
      isNegative = true;
      ptr++;
    }
    while (ptr < end && *ptr >= '0' && *ptr <= '9')
    {
      val = val * 10 + (*ptr - '0');
      ptr++;
    }
    if (isNegative)
      val = -val;
    // 写入数据
    if (dataElemCount < mapLen / sizeof(SortDataElement))
    {
      data[dataElemCount++] = val;
    }
    ptr++; // 跳过换行符
  }
}

/**
 * 将单个数据元素写入文件
 * @param fs 文件系统对象
 * @param dataElement 数据元素
 */
void _FileSort_WriteFileDataElement(FileSystem fs, SortDataElement dataElement)
{
  // 计算写入长度
  // int numberLen = 0;
  // {
  //   SortDataElement val = dataElement;
  //   if (val < 0)
  //   {
  //     numberLen++; // 符号位
  //     val = -val;
  //   }
  //   do
  //   {
  //     numberLen++;
  //     val /= 10;
  //   } while (val != 0);
  // }
  // char *buffer = malloc(sizeof(char) * (numberLen + 2)); // 多分配2个字节用于换行符和结束符
  char buffer[32 + 2];            // 足够存放int类型数据及换行符
  int numberLen = sizeof(buffer); // 预估长度
  int len = snprintf(buffer, numberLen, "%d\n", dataElement);
  if (len > 0)
    fs->functions->write(fs, buffer, sizeof(char), len);
  // free(buffer);
}
/**
 * 将数据数组写入文件
 * @param fs 文件系统对象
 * @param data 数据数组
 * @param dataElemCount 数据元素数量
 */
void _FileSort_WriteFileData(FileSystem fs, SortDataElement *data, ull dataElemCount)
{
  if (!data || dataElemCount == 0)
  {
    fs->functions->truncate(fs); // 写入内容为空，清空文件
    return;
  }
  fs->functions->seekLargeFile(fs, 0, SEEK_SET);
  for (size_t i = 0; i < dataElemCount; i++)
  {
    _FileSort_WriteFileDataElement(fs, data[i]);
  }
}

/**
 * 每行数据为一个SortDataElement类型的文件读取函数
 * @param fs 文件系统对象
 * @param bufferSize 读取缓冲区大小，单位为字节
 * @param dataElemCount 输出参数，实际读取到的数据元素数量
 */
SortDataElement *_FileSort_ReadFileSystemData_String(FileSystem fs, ull bufferSize, ull *dataElemCount)
{
  if (!fs || bufferSize <= 0)
  {
    assert(fs != NULL && bufferSize > 0);
    return NULL;
  }
  // 读取数据
  const FileSystemApi *fsApi = FileSystem_GetApi();
  // 先大致计算一下可能有多少个数据
  // 根据数据类型计算出数据最长字符数，比如int类型4个字节，32位，最大值是2147483647，最长10个字符
  // 保守估计，所以直接包含符号位
  size_t dataElementMaxLen = 0;
  {
    SortDataElement maxVal = ~(SortDataElement)0; // 全1
    if (maxVal < 0)
    {
      dataElementMaxLen++; // 符号位
      maxVal = -maxVal;    // 负数取反
    }
    do
    {
      dataElementMaxLen++;
      maxVal /= 10;
    } while (maxVal != 0);
  }
  size_t dataElementCount = (bufferSize + dataElementMaxLen - 1) / dataElementMaxLen;
  SortDataElement *data = malloc(sizeof(SortDataElement) * dataElementCount);
  data = memset(data, 0, sizeof(SortDataElement) * dataElementCount);
  if (!data)
  {
    printf("Error: 内存分配失败\n");
    return NULL;
  }
  // 读取数据
  ull charCount = fsApi->read(fs, data, sizeof(char), bufferSize);
  ull dataCount = 0;
  // 解析数据
  char *ptr = (char *)data;
  char *endPtr = (char *)data + charCount;
  ull lastElemLen = 0;
  while (ptr < endPtr)
  {
    lastElemLen = 0; // 重置数字状态
    if (*ptr != '-' && !(*ptr >= '0' && *ptr <= '9'))
      continue;
    SortDataElement val = 0;
    bool isNegative = false;
    if (*ptr == '-')
    {
      isNegative = true;
      ptr++;
      lastElemLen++;
    }
    while (ptr < endPtr && *ptr >= '0' && *ptr <= '9')
    {
      val = val * 10 + (*ptr - '0');
      ptr++;
      lastElemLen++;
    }
    if (isNegative)
      val = -val;
    if (dataCount >= dataElementCount)
    {
      // 重新分配更大的内存
      size_t newSize = dataElementCount * 2; // 翻倍
      SortDataElement *newData = (SortDataElement *)realloc(data, newSize * sizeof(SortDataElement));
      if (!newData)
        break; // 内存分配失败，跳出循环
      data = newData;
      memset(data + dataElementCount, 0, sizeof(SortDataElement) * dataElementCount);
      dataElementCount = newSize;
    }
    data[dataCount++] = val;
  }
  // 检查当前位置下一个元素是否数字
  // 如果不是或读取失败，则说明最后一个元素刚好读取结束，
  // 否则需要切断上一个元素
  char oneChar = 0;
  ull readOneCharCount = fsApi->read(fs, &oneChar, sizeof(char), 1);
  if (readOneCharCount == 1)
  {
    if (oneChar >= '0' && oneChar <= '9') // 如果是数字
    {
      data[--dataCount] = 0;
    }
    fsApi->seekLargeFile(fs, -1, FileSystem_SeekCurrent); // 回溯上一个位置
  }
  *dataElemCount = dataCount;
  return data;
}
SortDataElement *_FileSort_GetDataFromFileSystem(FileSystem fs, ull bufferSize, ull *dataElemCount)
{
  return _FileSort_ReadFileSystemData_String(fs, bufferSize, dataElemCount);
}

typedef struct SortDataElementArray
{
  SortDataElement *array; // 某个文件的数据数组
  ull size;               // 数据元素数量
} SortDataElementArray;

typedef struct SortDataArray
{
  SortDataElementArray *elements; // 文件数组
  ull size;                       // 文件数量
} SortDataArray;

typedef void (*_FileSort_Merge_KWayMerge_Callback)(const SortDataElement *loser, LoserTree_SizeType fileIndex, void *userData);

void _FileSort_Merge_KWayMerge_Data(SortDataArray sourceArray, ConstComparator cmp, _FileSort_Merge_KWayMerge_Callback callback, void *userData)
{
  // 文件个数
  ull k = sourceArray.size;
  SortDataElementArray *sourceFiles = sourceArray.elements;
  assert(k > 0 && "Error: k路归并的k值必须>0");
  // 每个文件的指针
  ull *indices = malloc(sizeof(ull) * k);
  memset(indices, 0, sizeof(ull) * k); // 初始化为0
  // 元素数组
  LoserTree_ElementType *elements = malloc(sizeof(LoserTree_ElementType) * k);
  for (ull i = 0; i < k; i++)
  {
    if (sourceFiles[i].size > 0)
      elements[i] = sourceFiles[i].array[0]; // 每个文件的第一个元素
    else
      elements[i] = 0; // 文件为空，设置为0
    assert(sourceFiles[i].size > 0 && "Error: 参与k路归并的文件不能为空");
    // printf("Initial Element from File %llu: %d\n", i, elements[i]);
  }
  {
    // 创建Loser Tree
    LoserTree loserTree = LoserTree_Create(elements, sizeof(LoserTree_ElementType), k, cmp);
    do
    {
      LoserTree_SizeType loserIndex = 0; // 失败者索引，代表文件索引
      const LoserTree_ElementType *loser = LoserTree_GetLoserPtr(loserTree, &loserIndex);
      if (!loser)
        break; // 没有失败者，结束
      if (callback)
        callback(loser, loserIndex, userData); // 回调处理当前最小元素
      ull idx = ++indices[loserIndex];
      if (idx < sourceFiles[loserIndex].size)
        LoserTree_ReplaceLoser(loserTree, sourceFiles[loserIndex].array[indices[loserIndex]]); // 用下一个元素替换失败者
      else
        LoserTree_PopLoser(loserTree, NULL); // 该文件数据用完，弹出失败者
    } while (1);
    // 清理资源
    LoserTree_Destroy(loserTree);
  }
  free(elements);
  free(indices);
}

void _FileSort_Merge_KWayMerge_FileSystem(FileSystem *fss, ull fsCount, ull bufferSizePerFile, ConstComparator cmp, _FileSort_Merge_KWayMerge_Callback callback, void *userData)
{
  if (!fss || fsCount == 0)
  {
    assert(fss != NULL && fsCount > 0);
    return;
  }
  const FileSystemApi *fsApi = FileSystem_GetApi();
  // 准备数据
  ull k = fsCount;
  assert(k > 0 && "Error: k路归并的k值必须>0");
  // 文件内容缓冲区
  SortDataElementArray *sourceFiles = malloc(sizeof(SortDataElementArray) * k);
  // 每个文件的指针
  ull *indices = malloc(sizeof(ull) * k);
  memset(indices, 0, sizeof(ull) * k); // 初始化为0
  // 元素数组
  LoserTree_ElementType *elements = malloc(sizeof(LoserTree_ElementType) * k);
  for (ull i = 0; i < k; i++)
  {
    if (!fss[i])
    {
      printf("Error: 参与k路归并的文件系统对象不能为空\n");
      for (ull j = 0; j < i; j++)
        free(sourceFiles[j].array);
      assert(fss[i] != NULL && "Error: 参与k路归并的文件系统对象不能为空");
      return;
    }
    // 读取文件内容到缓冲区
    sourceFiles[i].array = _FileSort_GetDataFromFileSystem(fss[i], bufferSizePerFile, &sourceFiles[i].size);
    if (!sourceFiles[i].array)
    {
      printf("Error: 读取文件系统数据失败\n");
      for (ull j = 0; j < i; j++)
        free(sourceFiles[j].array);
      assert(sourceFiles[i].array != NULL && "Error: 读取文件系统数据失败");
      return;
    }
  }
  {
    // 创建Loser Tree
    LoserTree loserTree = LoserTree_Create(elements, sizeof(LoserTree_ElementType), k, cmp);
    do
    {
      LoserTree_SizeType loserIndex = 0; // 失败者索引，代表文件索引
      const LoserTree_ElementType *loser = LoserTree_GetLoserPtr(loserTree, &loserIndex);
      if (!loser)
        break; // 没有失败者，结束
      if (callback)
        callback(loser, loserIndex, userData); // 回调处理当前最小元素
      ull idx = ++indices[loserIndex];
      if (idx < sourceFiles[loserIndex].size)
        LoserTree_ReplaceLoser(loserTree, sourceFiles[loserIndex].array[indices[loserIndex]]); // 用下一个元素替换失败者
      else
        LoserTree_PopLoser(loserTree, NULL); // 该文件数据用完，弹出失败者
    } while (1);
    // 清理资源
    LoserTree_Destroy(loserTree);
  }
  for (ull i = 0; i < k; i++)
    free(sourceFiles[i].array);
  free(sourceFiles);
  free(elements);
  free(indices);
}

void SortSmallFile(String filePath, ConstComparator cmp)
{
  if (!filePath)
  {
    assert(filePath != NULL);
    return;
  }
  FileSystem fs = FileSystem_Create(filePath->functions->toCString(filePath), "r+");
  if (!fs)
  {
    assert(fs != NULL);
    return;
  }
  FileSystemApi *fsApi = fs->functions;
  // 获取文件大小
  if (!fsApi->seekLargeFile(fs, 0, SEEK_END))
  {
    int seekLargeFile = 0;
    assert(seekLargeFile != 0);
    return;
  }
  ull fileSize = fsApi->tellLargeFile(fs);
  if (fileSize == (ull)-1)
  {
    assert(fileSize != (ull)-1);
    return;
  }
  // 映射文件到内存
  MemoryMap mm = MemoryMap_Open(fsApi->getFilePath(fs), 1);
  if (mm.fd == MEMORY_MAP_FILE_INVALID)
  {
    assert(mm.fd != MEMORY_MAP_FILE_INVALID);
    return;
  }
  char *base = (char *)MemoryMap_Map(&mm, 1);
  if (!base || mm.length != fileSize)
  {
    MemoryMap_Close(&mm);
    assert(base != NULL && mm.length == fileSize);
    return; // 映射失败或映射长度不匹配
  }
  size_t dataElementCount = 0;
  SortDataElement *data = _FileSort_GetDataFromMemoryMap(base, mm.length, &dataElementCount);
  if (!data)
  {
    MemoryMap_Unmap(&mm);
    MemoryMap_Close(&mm);
    assert(data != NULL);
    return; // 读取数据失败
  }
  printf("读取小文件 %s 中 %llu 个数据进行排序...\n", fsApi->getFilePath(fs), dataElementCount);
  // 对数据进行排序
  qsort(data, dataElementCount, sizeof(SortDataElement), cmp);
  printf("排序 %s 完成，正在写回文件...\n", fsApi->getFilePath(fs));
  // 暂时使用普通文件读写方式写回排序后的数据，暂不使用内存映射写回
  // 解除映射并关闭文件
  MemoryMap_Unmap(&mm);
  MemoryMap_Close(&mm);
  // 将排序后的数据按照原来的格式写回文件
  fsApi->truncate(fs);                                 // 清空文件内容
  _FileSort_WriteFileData(fs, data, dataElementCount); // 写入排序后数据
  printf("写回 %s 完成。\n", fsApi->getFilePath(fs));
  // 写入完成，关闭文件
  fsApi->close(fs);
  free(data);
}

typedef struct _FileSort_Merge_KWayMerge_Callback_Data
{
  FileSystem outputFile;
  char *buffer;
  ull bufferSize;
  ull count;
  ull totalWritten;
} _FileSort_Merge_KWayMerge_Callback_Data;

void _FileSort_Merge_KWayMerge_Callback_Impl(const SortDataElement *loser, LoserTree_SizeType fileIndex, void *userData)
{
  _FileSort_Merge_KWayMerge_Callback_Data *callbackData = (_FileSort_Merge_KWayMerge_Callback_Data *)userData;
  // printf("File: %llu, Element: %llu\n", fileIndex, *loser); // 输出当前最小元素
  // callbackData->outputFile->functions->write(callbackData->outputFile, loser, sizeof(SortDataElement), 1);
  if (callbackData->buffer && callbackData->bufferSize > 0)
  {
    char buffer[32 + 2];            // 足够存放int类型数据及换行符
    int numberLen = sizeof(buffer); // 预估长度
    int len = snprintf(buffer, numberLen, "%d\n", *loser);
    if (len > 0)
    {
      if (callbackData->count + len > callbackData->bufferSize)
      {
        callbackData->outputFile->functions->write(callbackData->outputFile, callbackData->buffer, sizeof(char), callbackData->count);
        callbackData->count = 0;
        memcpy(callbackData->buffer + callbackData->count, buffer, len);
      }
      if (callbackData->count + len <= callbackData->bufferSize)
      {
        memcpy(callbackData->buffer + callbackData->count, buffer, len);
        callbackData->count += len;
      }
      else
      {
        _FileSort_WriteFileDataElement(callbackData->outputFile, *loser);
        printf("缓冲区大小不足以容纳当前元素，当前元素: %llu", *loser);
        assert(len > 0 && "缓冲区大小不足以容纳当前元素");
      }
      return;
    }
    else
    {
      printf("元素数字转字符串失败，失败元素: %llu", *loser);
      assert(len > 0 && "元素数字转字符串失败");
    }
  }
  _FileSort_WriteFileDataElement(callbackData->outputFile, *loser);
}

void MergeSortedFiles(const char **partFilePaths, ull partFileCount, String outputFilePath, String tempFileFolder, ull availableMemory, ull bufferSize, ConstComparator cmp)
{
  if (!partFilePaths || !outputFilePath)
  {
    assert(partFilePaths != NULL && outputFilePath != NULL);
    return;
  }
  const StringApi *strApi = outputFilePath->functions;
  if (partFileCount <= 0 || !partFilePaths || strApi->isEmpty(outputFilePath))
  {
    assert(partFileCount > 0 && partFilePaths && !strApi->isEmpty(outputFilePath));
    return;
  }
  const FileSystemApi *fsApi = FileSystem_GetApi();
  // 临时文件夹路径末尾添加斜杠
  String _outputFilePath = strApi->copy(outputFilePath);
  String _tempFileFolder = strApi->copy(tempFileFolder);
  if (strApi->get(_tempFileFolder, strApi->length(_tempFileFolder) - 1) != '/' &&
      strApi->get(_tempFileFolder, strApi->length(_tempFileFolder) - 1) != '\\')
  {
    strApi->appendChar(_tempFileFolder, fsApi->getPathSeparator());
  }
  String _tempFilePathPrefix = strApi->copy(_tempFileFolder);
  strApi->appendCString(_tempFilePathPrefix, "merge_temp_", 0, 1);

  // 输出所有文件路径用于测试
  for (ull i = 0; i < partFileCount; i++)
  {
    printf("Part File %llu: %s\n", i + 1, partFilePaths[i]);
  }

  // 计算k值: k = floor(0.8 * M / (2 * B))
  // M: 剩余可用内存
  // B: 每次读写的块大小（字节），一般取磁盘块或自定义的缓冲区页大小
  LoserTree_SizeType k = 5; // TODO: 暂时假定
  k = availableMemory / (2 * bufferSize);
  if (k > partFileCount)
    k = partFileCount; // k过大，紧缩为小文件个数
  char *outputBuffer = malloc(sizeof(char) * bufferSize);
  printf("当前缓冲区大小: %llu 字节, 使用 %llu 路归并排序...\n", bufferSize, k);
#ifndef _DEBUG
#endif
  Array *newPartFilePaths = Array_Create(NULL, 0);
  const ArrayApi *arrApi = Array_GetApi();
  for (ull i = 0; i < partFileCount; i++)
  {
    ull len = strlen(partFilePaths[i]);
    char *path = malloc(sizeof(char) * (len + 1));
    memcpy(path, partFilePaths[i], sizeof(char) * len); // 深拷贝
    path[len] = 0;                                      // 设置最后一个字符为空字符，防止越界
    arrApi->append(newPartFilePaths, path);             // 浅拷贝
  }

  // TODO: 删除统计数据-------------------------------------
  ull statisticsElemCount = 0;

  // 归并x层
  for (ull remainingFiles = partFileCount; remainingFiles > 1; remainingFiles = (remainingFiles + k - 1) / k)
  {
    // TODO: 删除统计数据-------------------------------------
    ull statisticsLayerElemCount = 0;

    // 计算循环次数（k路归并的轮数）
    ull totalRounds = (remainingFiles + k - 1) / k;
    lboolean isLastRound = (totalRounds <= 1);
    // 进行多次k路归并
    for (ull round = 0; round < totalRounds; round++)
    {
      ull fileCountOnce = k;
      if (round == totalRounds - 1)
      {
        // 最后一轮，可能文件数不足k个
        fileCountOnce = remainingFiles - round * k;
      }
      MemoryMap *arrMM = malloc(sizeof(MemoryMap) * fileCountOnce);
      SortDataElementArray *partFileDataArr = malloc(sizeof(SortDataElementArray) * fileCountOnce);
      SortDataArray sourceArray; // 文件数组
      sourceArray.size = fileCountOnce;
      sourceArray.elements = partFileDataArr;
      // 计算数组偏移
      ull sourceArrayOffset = round * k;
      // 读取每个文件的数据到内存
      for (ull i = 0; i < fileCountOnce; i++)
      {
        const char *partFilePath = arrApi->get(newPartFilePaths, i + sourceArrayOffset);
        // 先确保文件存在
        FileSystem fs = FileSystem_Create(partFilePath, "a+");
        fsApi->close(fs);
        fsApi->destroy(fs);
        // 再打开文件
        // TODO: 当前映射整个文件用于测试，实际只能映射bufferSize的大小，或者使用FileSystem打开并读取文件
        {
          // 再进行内存映射
          arrMM[i] = MemoryMap_Open(partFilePath, 0);
          void *base = MemoryMap_Map(&arrMM[i], 0); // 只读映射
          if (!base)
          {
            printf("Error: 内存映射文件 %s 失败。\n", partFilePath);
            MemoryMap_Close(&arrMM[i]);
            return; // 映射失败
          }
          assert(base != NULL && "Error: 内存映射失败");
          ull dataElemCount = 0;
          SortDataElement *data = _FileSort_GetDataFromMemoryMap(base, arrMM[i].length, &dataElemCount);
          assert(data != NULL && "Error: 读取内存映射数据失败");
          partFileDataArr[i].array = data;
          partFileDataArr[i].size = dataElemCount;
          printf("读取待归并文件 %s 中 %llu 个数据完成。\n", partFilePath, dataElemCount);
          if (remainingFiles == partFileCount)
            statisticsElemCount += dataElemCount;
          statisticsLayerElemCount += dataElemCount;
        }
      }
      // 进行K路归并排序
      String tempFilePath = NULL;
      // 文件序号
      if (!isLastRound)
      {
        tempFilePath = strApi->copy(_tempFilePathPrefix);
        ull tempFileIndex = round;
        char indexBuffer[66];
        snprintf(indexBuffer, sizeof(indexBuffer), "%llu_%llu", remainingFiles, tempFileIndex);
        strApi->appendCString(tempFilePath, indexBuffer, 0, 1);
        strApi->appendCString(tempFilePath, ".tmp", 4, 0);
        // 先释放之前的内存
        free(arrApi->get(newPartFilePaths, round));
        ull nLen = strApi->length(tempFilePath);
        char *nextLayerPath = malloc(sizeof(char) * (nLen + 1));
        memcpy(nextLayerPath, strApi->toCString(tempFilePath), sizeof(char) * nLen);
        nextLayerPath[nLen] = 0; // 结尾空字符
        arrApi->replace(newPartFilePaths, round, nextLayerPath);
      }
      else
      {
        tempFilePath = strApi->copy(_outputFilePath);
      }
      FileSystem outputFile = FileSystem_Create(strApi->toCString(tempFilePath), "w+");
      _FileSort_Merge_KWayMerge_Callback_Data callbackData = {
          .outputFile = outputFile,
          .buffer = outputBuffer,
          .bufferSize = bufferSize,
          .count = 0};
      _FileSort_Merge_KWayMerge_Data(sourceArray, cmp, _FileSort_Merge_KWayMerge_Callback_Impl, &callbackData);
      // 如果缓冲区还有数据，写入文件
      if (callbackData.count > 0)
      {
        fsApi->write(outputFile, callbackData.buffer, sizeof(char), callbackData.count);
        callbackData.count = 0;
      }
      for (ull i = 0; i < fileCountOnce; i++)
      {
        free(partFileDataArr[i].array);
        MemoryMap_Unmap(&arrMM[i]);
        MemoryMap_Close(&arrMM[i]);
      }
      free(partFileDataArr);
      free(arrMM);
      strApi->destroy(tempFilePath);
      fsApi->close(outputFile);
      fsApi->destroy(outputFile);
    }

    // 打印统计数据
    printf("归并层剩余文件数: %llu 中共处理元素 %llu 个。\n", remainingFiles, statisticsLayerElemCount);
  }
  // 打印统计数据
  printf("共处理元素 %llu 个。\n", statisticsElemCount);
  free(outputBuffer); // 释放输出缓冲区
  strApi->destroy(_outputFilePath);
  strApi->destroy(_tempFileFolder);
  strApi->destroy(_tempFilePathPrefix);
  for (ull i = 0; i < partFileCount; i++)
  {
    free(arrApi->get(newPartFilePaths, i)); // 释放深拷贝所创建的内存
  }
  arrApi->destroy(newPartFilePaths);
}
