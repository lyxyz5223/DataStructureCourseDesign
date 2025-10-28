#include "FileSystem.h"
#include "TextFormatting.h"
#include <string.h>
#include <time.h>

int main()
{
  // const char *test = "你好";
  // printf("strlen(\"%s\"): %llu\n", test, strlen(test));
  FileSystem fs = FileSystem_Create("input.txt", "r");
  if (!fs)
    return 1;
  TextFormatting tf = TextFormatting_CreateWithDefaults();
  if (!tf)
  {
    fs->functions->destroy(fs);
    return 1;
  }
  String str = String_Create(NULL, 0, 0);
  if (!str)
  {
    tf->functions->destroy(tf);
    fs->functions->destroy(fs);
    return 1;
  }
  // 记录开始时间
  clock_t startTime = clock();
  char buffer[1024];
  size_t bytesRead;
  while ((bytesRead = fs->functions->read(fs, buffer, sizeof(char), sizeof(buffer) / sizeof(char))) > 0)
  {
    str->functions->appendCString(str, buffer, bytesRead, 0);
  }
  String formattedText = TextFormatting_ApplyFormatting(tf, str);
  // 记录结束时间
  clock_t endTime = clock();
  double timeSpent = (double)(endTime - startTime) / CLOCKS_PER_SEC;
  printf("Text formatting took %.6f seconds.\n", timeSpent);
  FileSystem fsOut = FileSystem_Create("output.txt", "w");
  if (!fsOut)
  {
    str->functions->destroy(str);
    tf->functions->destroy(tf);
    fs->functions->destroy(fs);
    return 1;
  }
  const char *formattedContent = formattedText->functions->toCString(formattedText);
  if (formattedContent)
  {
    // 打印格式化后的内容
    printf("%s\n", formattedContent);
    // 写入文件
    fsOut->functions->write(fsOut, formattedContent, sizeof(char), strlen(formattedContent));
    // 销毁格式化后的字符串对象
    formattedText->functions->destroy(formattedText);
  }
  printf("Text formatting took %.6f seconds.\n", timeSpent);
  str->functions->destroy(str);
  tf->functions->destroy(tf);
  fs->functions->destroy(fs);
  return 0;
}

