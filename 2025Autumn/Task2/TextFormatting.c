#include "TextFormatting.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "Universal/Array/Array.h"
#include <stdio.h>

static TextFormattingApi _TextFormatting_Global_Api; // 前向声明
typedef struct _TextFormatting
{
  TextFormattingApi *functions;
  int indentationCharCount;    // 缩进字符数
  int maxLinesPerPage;         // 每页最大行数
  int maxCharsPerLine;         // 每行最大字符数
  lboolean wordWrapEnabled;    // 是否启用自动换行
  lboolean mergeSpacesEnabled; // 是否合并多个空格为一个空格
  String formattedString; // 用于存储格式化后的字符串
} *_TextFormatting;

String _TextFormatting_Format_Indentation(_TextFormatting tf)
{
  if (!tf)
    return NULL;
  String indentation = String_Create("", 0, 1);
  if (!indentation)
    return NULL;
  for (int i = 0; i < tf->indentationCharCount; i++)
  {
    if (!indentation->functions->appendChar(indentation, ' '))
    {
      indentation->functions->destroy(indentation);
      return NULL;
    }
  }
  return indentation;
}
// 判断字节是否是 UTF-8 字符的起始字节
bool _TextFormatting_IsUtf8StartByte(unsigned char c)
{
  return (c & 0xC0) != 0x80;
}

// 获取 UTF-8 字符的字节数
int _TextFormatting_GetUtf8CharLength(unsigned char first_byte)
{
  if ((first_byte & 0x80) == 0x00)
    return 1; // 1字节字符: 0xxxxxxx
  if ((first_byte & 0xE0) == 0xC0)
    return 2; // 2字节字符: 110xxxxx
  if ((first_byte & 0xF0) == 0xE0)
    return 3; // 3字节字符: 1110xxxx
  if ((first_byte & 0xF8) == 0xF0)
    return 4; // 4字节字符: 11110xxx
  return 1;   // 无效的 UTF-8，按单字节处理
}

String _TextFormatting_Format_ApplyAll(_TextFormatting tf, String str)
{
  if (!tf || !str)
    return NULL;

  // 获取字符串内容
  const char *oriCStr = str->functions->toCString(str);
  ull oriLen = str->functions->length(str);
  if (!oriCStr)
    return NULL;
  Array* array = Array_Create(0, 0);
  // 创建一个新的字符串用于存储格式化后的内容
  String fstr = tf->formattedString = String_Create("", 0, 1);
  StringApi *fapi = fstr->functions;
  if (!tf->formattedString)
    return NULL;
  char lastCharLineBreakCount = 0; // 用于合并空行
  lboolean lastCharWasSpace = false; // 用于合并空格
  lboolean atLineStart = true;   // 用于处理缩进
  lboolean isLineStartChar = true; // 当前字符是否在行首
  TextIndentationCountType indentationCharCount = 0; // 缩进字符数
  TextCharsPerLineCountType currentLineCharCount = 0;     // 当前行的字符数
  TextLinesPerPageCountType currentPageLineCount = 1;      // 当前页的行数
  String lastWord = String_Create("", 0, 0); // 用于存储当前读取到的最后一个单词
  int utf8CharLengthCount = 0; // 用于处理UTF8字符，防止分割中文字符
  // String debugString = String_Create("12345678901234567890123456789012345678901234567890", 0, 1);
  for (int i = 0; i < oriLen; i++) // 遍历原始字符串的每个字符
  {
    char c = oriCStr[i];
    // 处理字符
    if (c == '\n') // 换行符
    {
      // 重置条件
      indentationCharCount = 0;
      currentLineCharCount = 0;
      atLineStart = true;
      isLineStartChar = true;
      lastCharWasSpace = false;
      lastWord->functions->clear(lastWord);
      if (lastCharLineBreakCount >= 2) // 合并多个换行符
        continue;
      lastCharLineBreakCount++;
      fapi->appendChar(fstr, '\n');
      currentPageLineCount++;
      continue;
    }
    else
      lastCharLineBreakCount = 0; // 重置换行符标志
    if (c == PAGE_BREAK_CHAR) // 直接忽略分页符，因为换行符是本程序产生的，修改格式时无需保留
      continue;
    if (c == ' ' || c == '\t') // 空格或制表符
    {
      if (isLineStartChar) // 行首空格直接忽略
        continue;
      // 如果不是行首空格或制表符，则处理空格合并
      if (c == ' ' && tf->mergeSpacesEnabled)
      {
        if (lastCharWasSpace)
          continue; // 跳过多个连续空格
        lastCharWasSpace = true;
      }
    } else
      lastCharWasSpace = false; // 不合并空格，重置标志

    if (atLineStart) // 处理行首缩进
    {
      int j = 0;
      for (j = 0; j < tf->indentationCharCount; j++)
      {
        fapi->appendChar(fstr, ' ');
        indentationCharCount++;
        currentLineCharCount++;
      }
    }
    // 普通字符，直接添加
    // 记录当前单词
    isLineStartChar = false;
    atLineStart = false; // 非行首
    // 获取当前字符的字节数
    int charLen = _TextFormatting_GetUtf8CharLength(c);

    // 保存当前字符的所有字节
    for (int j = i; i - j < charLen && i < oriLen; i++)
    {
      c = oriCStr[i];
      fapi->appendChar(fstr, c);
      // DEBUG
      // if (currentLineCharCount == 80)
      // {
      //   const char *curWord = lastWord->functions->toCString(lastWord);
      //   lastWord = lastWord;
      // }
      // if (currentPageLineCount == 22)
      // {
      //   const char *curWord = lastWord->functions->toCString(lastWord);
      //   lastWord = lastWord;
      // }
      // debug 字符串，50个字符，调试用
      // debugString->functions->remove(debugString, 0);     // 删除第一个字符
      // debugString->functions->appendChar(debugString, c); // 添加当前字符
      // printf("Debug: %s | LineCharCount: %d | LastWord: %s\n", debugString->functions->toCString(debugString), currentLineCharCount, lastWord->functions->toCString(lastWord));
    }
    i--; // 多字符集便利的时候i多加了1，因此-1
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
     || c == '-' || c == ',' || c == '.'
      || c == ':' || c == '\"' || c == '\'') // 特殊字符纳入单词管辖范围
      lastWord->functions->appendChar(lastWord, c);
    else
      lastWord->functions->clear(lastWord);
    const char* currentFormattedStr = fstr->functions->toCString(fstr);
    currentLineCharCount++;
    // 检查是否需要换行
    if (currentLineCharCount >= tf->maxCharsPerLine)
    {
      lboolean isPageBreak = 0;
      if (currentPageLineCount >= tf->maxLinesPerPage && tf->maxLinesPerPage > 0) // 超过每页最大行数，插入分页符
        isPageBreak = 1;
      char insertChar = isPageBreak ? PAGE_BREAK_CHAR : '\n';
      if (!tf->wordWrapEnabled) // 不启用按照单词换行，直接插入换行符
      {
        fapi->appendChar(fstr, insertChar);
        currentLineCharCount = 0;
      }
      else // 启用按照单词换行
      {
        if (lastWord->functions->length(lastWord) > tf->maxCharsPerLine) // 当前单词长度超过每行最大字符数，直接在当前字符处换行
        {
          fapi->appendChar(fstr, insertChar);
          currentLineCharCount = 0;
        }
        else // 否则在上一个单词处换行
        {
          ull lastWordLen = lastWord->functions->length(lastWord);
          const char *curWord = lastWord->functions->toCString(lastWord);
          // 在上个单词处换行
          fstr->functions->insertChar(fstr, fstr->functions->length(fstr) - lastWordLen, insertChar);
          currentLineCharCount = lastWordLen;
        }
      }
      if (isPageBreak)
        currentPageLineCount = 0;
      currentPageLineCount++;
      lastWord->functions->clear(lastWord);
    }
  }
  // 释放内存
  array->functions->destroy(array);
  lastWord->functions->destroy(lastWord);
  // debugString->functions->destroy(debugString);
  return fstr;
}

TextFormatting TextFormatting_Create(TextIndentationCountType indentationCharCount, TextLinesPerPageCountType maxLinesPerPage, TextCharsPerLineCountType maxCharsPerLine, lboolean wordWrapEnabled, lboolean mergeSpacesEnabled)
{
  _TextFormatting tf = (_TextFormatting)malloc(sizeof(struct _TextFormatting));
  if (!tf)
    return NULL;
  tf->functions = &_TextFormatting_Global_Api;
  tf->indentationCharCount = indentationCharCount;
  tf->maxLinesPerPage = maxLinesPerPage;
  tf->maxCharsPerLine = maxCharsPerLine;
  tf->wordWrapEnabled = wordWrapEnabled;
  tf->mergeSpacesEnabled = mergeSpacesEnabled;
  return (TextFormatting)tf;
}

TextFormatting TextFormatting_CreateWithDefaults()
{
  return TextFormatting_Create(
    DEFAULT_INDENTATION_CHAR_COUNT,
    DEFAULT_MAX_LINES_PER_PAGE_COUNT,
    DEFAULT_MAX_CHARS_PER_LINE_COUNT,
    DEFAULT_ENABLE_WORD_WRAP,
    DEFAULT_ENABLE_MERGE_SPACES
  );
}

lboolean TextFormatting_Destroy(TextFormatting tf)
{
  if (!tf)
    return false;
  free(tf);
  return true;
}

String TextFormatting_ApplyFormatting(TextFormatting tf, String str)
{
  if (!tf)
    return false;
  return _TextFormatting_Format_ApplyAll(((_TextFormatting)tf), str);
}


lboolean TextFormatting_SetIndentation(TextFormatting tf, TextIndentationCountType indentationCharCount)
{
  if (!tf)
    return false;
  tf->indentationCharCount = indentationCharCount;
  return true;
}
lboolean TextFormatting_SetPageMaxLines(TextFormatting tf, TextLinesPerPageCountType maxLines)
{
  if (!tf)
    return false;
  tf->maxLinesPerPage = maxLines;
  return true;
}

lboolean TextFormatting_SetLineMaxChars(TextFormatting tf, TextCharsPerLineCountType maxCharsPerLine)
{
  if (!tf)
    return false;
  tf->maxCharsPerLine = maxCharsPerLine;
  return true;
}
lboolean TextFormatting_SetWordWrap(TextFormatting tf, lboolean enable)
{
  if (!tf)
    return false;
  tf->wordWrapEnabled = enable;
  return true;
}
lboolean TextFormatting_SetMergeSpaces(TextFormatting tf, lboolean enable)
{
  if (!tf)
    return false;
  tf->mergeSpacesEnabled = enable;
  return true;
}
lboolean TextFormatting_ResetFormatting(TextFormatting tf)
{
  if (!tf)
    return false;
  tf->indentationCharCount = DEFAULT_INDENTATION_CHAR_COUNT;
  tf->maxLinesPerPage = DEFAULT_MAX_LINES_PER_PAGE_COUNT;
  tf->maxCharsPerLine = DEFAULT_MAX_CHARS_PER_LINE_COUNT;
  tf->wordWrapEnabled = DEFAULT_ENABLE_WORD_WRAP;
  tf->mergeSpacesEnabled = DEFAULT_ENABLE_MERGE_SPACES;
  return true;
}
static TextFormattingApi _TextFormatting_Global_Api = {
    .applyFormatting = TextFormatting_ApplyFormatting,
    .setIndentation = TextFormatting_SetIndentation,
    .setPageMaxLines = TextFormatting_SetPageMaxLines,
    .setLineMaxChars = TextFormatting_SetLineMaxChars,
    .setWordWrap = TextFormatting_SetWordWrap,
    .setMergeSpaces = TextFormatting_SetMergeSpaces,
    .resetFormatting = TextFormatting_ResetFormatting,
    .destroy = TextFormatting_Destroy
};