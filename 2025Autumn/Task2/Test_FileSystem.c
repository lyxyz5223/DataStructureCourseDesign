#include "FileSystem.h"
#include <stdio.h>
#include <string.h>

int main()
{
  const char *filename = "test_fs.txt";
  const char *test_str = "Hello, FileSystem!";
  char buffer[64] = {0};
  FileSystemApi* api = FileSystem_GetApi();
  if (!api) {
    printf("Failed to get FileSystem API\n");
    return 1;
  }
  // if (api->fileExists(filename))
  //   remove(filename); // 删除旧文件，确保测试环境干净
  // 1. 创建并写入
  FileSystem fs = FileSystem_Create(filename, "a+"); // 拼接方式写入文件
  if (!fs)
  {
    printf("Create failed\n");
    return 1;
  }
  ull written = fs->functions->write(fs, test_str, 1, strlen(test_str));
  printf("Written: %llu\n", written);

  // 2. 定位到文件头，读取
  fs->functions->seekSmallFile(fs, 0, FileSystem_SeekStart);
  ull read = fs->functions->read(fs, buffer, 1, strlen(test_str));
  printf("Read: %llu, Content: %s\n", read, buffer);

  // 3. 测试 tell/seek/eof
  long pos = fs->functions->tellSmallFile(fs);
  printf("Tell: %ld\n", pos);
  fs->functions->seekSmallFile(fs, 0, FileSystem_SeekStart);
  printf("EOF: %d\n", fs->functions->eof(fs));
  fs->functions->read(fs, buffer, 1, 1);
  printf("EOF: %d\n", fs->functions->eof(fs));

  // 4. 错误处理
  fs->functions->seekSmallFile(fs, 1000, FileSystem_SeekStart); // 越界
  printf("Error: %d\n", fs->functions->error(fs));
  fs->functions->clearError(fs);

  // 5. 关闭和销毁
  fs->functions->close(fs);
  fs->functions->destroy(fs);

  // 6. 只读模式测试
  fs = FileSystem_Create(filename, "r");
  if (!fs)
  {
    printf("Reopen failed\n");
    return 1;
  }
  memset(buffer, 0, sizeof(buffer));
  read = fs->functions->read(fs, buffer, 1, strlen(test_str));
  printf("Read again: %llu, Content: %s\n", read, buffer);
  fs->functions->close(fs);
  fs->functions->destroy(fs);

  return 0;
}