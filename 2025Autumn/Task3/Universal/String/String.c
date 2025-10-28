#include "String.h"
#include "CharArray.h"
#include <stdlib.h> // malloc, free
#include <string.h> // strlen, strcpy

// 声明全局api接口
static StringApi _String_Global_Api;

typedef struct _String
{
  StringApi *functions;
  // 内部数据结构
  CharArray *charArray; // 字符数组，类似父类
  // char *cachedCString; // 缓存的C字符串，避免重复转换，防止直接修改内部数据，防止内存泄漏
} *_String;

// void _String_CachedCString_Update(_String str)
// {

// }

String String_Create(const char *cstr, ull len, lboolean autoCalculateLength)
{
  if (autoCalculateLength) // 自动计算长度
    len = strlen(cstr);
  // 分配内存
  char *dataSet = malloc(sizeof(char) * (len + 1)); // 用于保存Array的数据
  // 进行深拷贝
  for (ull i = 0; i < len; i++)
    dataSet[i] = cstr[i]; // 保存字符地址到数据集&char -> void*
  dataSet[len] = '\0'; // 末尾空字符
  CharArray *array = CharArray_Create(dataSet, len + 1);
  free(dataSet); // 释放临时数据集内存
  _String str = malloc(sizeof(struct _String));
  str->charArray = array; // 初始化字符数组
  str->functions = &_String_Global_Api;
  // str->cachedCString = NULL; // 初始化缓存的C字符串
  return (String)str;
}

const StringApi* String_GetApi(void)
{
  return &_String_Global_Api;
}

lboolean String_Destroy(String str)
{
  if (!str)
    return false;
  _String _str = (_String)str;
  if (_str->charArray)
    _str->charArray->functions->destroy(_str->charArray);
  // if (_str->cachedCString)
  //   free(_str->cachedCString);
  free(_str);
  return true;
}

/**
 * 获取字符串长度
 * @note 获取到的长度是实际数据结构CharArray获取到的长度-1（减去末尾的'\0'）
 * @note 时间复杂度O(1)
 */
ull String_Length(String str)
{
  if (!str)
    return 0;
  _String _str = (_String)str;
  return _str->charArray->functions->length(_str->charArray) - 1; // 减去末尾的'\0'
}

const char* String_ToCString(String str)
{
  if (!str)
    return NULL;
  _String _str = (_String)str;
  // if (!_str->cachedCString) // 如果没有缓存，那么更新缓存
  //   _String_CachedCString_Update(_str);
  // return (const char*)_str->cachedCString;
  return (const char *)_str->charArray->data;
}

char String_Get(String str, ull pos)
{
  if (!str)
    return '\0';
  _String _str = (_String)str;
  ull len = String_Length(str);
  if (pos < 0 || pos >= len)
    return '\0';
  return _str->charArray->functions->get(_str->charArray, pos);
}

lboolean String_InsertChar(String str, ull pos, char c)
{
  if (!str)
    return false;
  // 检查 pos 边界合法性
  _String _str = (_String)str;
  ull len = String_Length(str);
  if (pos < 0 || pos > len)
    return false;
  return _str->charArray->functions->insert(_str->charArray, pos, c);
}

lboolean String_AppendChar(String str, char c)
{
  return String_InsertChar(str, String_Length(str), c);
}

lboolean String_InsertCString(String str, ull pos, const char* substr, ull len, lboolean autoCalculateLength)
{
  if (!str || !substr)
    return false;
  // 检查 pos 边界合法性
  _String _str = (_String)str;
  ull substrLen = autoCalculateLength ? strlen(substr) : len;
  ull strLen = String_Length(str);
  if (pos < 0 || pos > strLen)
    return false;
  return _str->charArray->functions->insertMultiple(_str->charArray, pos, substr, substrLen);
}

lboolean String_AppendCString(String str, const char* substr, ull len, lboolean autoCalculateLength)
{
  return String_InsertCString(str, String_Length(str), substr, len, autoCalculateLength);
}

lboolean String_InsertString(String str, ull pos, String substr)
{
  if (!str || !substr)
    return false;
  // 检查 pos 边界合法性
  _String _str = (_String)str;
  ull substrLen = String_Length(substr);
  ull strLen = String_Length(str);
  if (pos < 0 || pos > strLen)
    return false;
  _String _substr = (_String)substr;
  return _str->charArray->functions->insertMultiple(_str->charArray, pos, _substr->charArray->data, substrLen);
}

lboolean String_AppendString(String str, String substr)
{
  return String_InsertString(str, String_Length(str), substr);
}

lboolean String_ReplaceChar(String str, ull pos, char c)
{
  if (!str)
    return false;
  // 检查 pos 边界合法性
  _String _str = (_String)str;
  ull len = String_Length(str);
  if (pos < 0 || pos >= len)
    return false;
  return _str->charArray->functions->replace(_str->charArray, pos, c);
}

lboolean String_ReplaceCString(String str, ull pos, ull count, const char* substr, ull substrLen, lboolean autoCalculateLen)
{
  if (!str || !substr)
    return false;
  // 检查 pos 边界合法性
  _String _str = (_String)str;
  ull len = String_Length(str);
  if (pos < 0 || pos >= len)
    return false;
  if (autoCalculateLen)
    substrLen = strlen(substr);
  return _str->charArray->functions->replaceRange(_str->charArray, pos, count, substr, substrLen);
}

lboolean String_ReplaceString(String str, ull pos, ull len, String substr)
{
  if (!str || !substr)
    return false;
  // 检查 pos 边界合法性
  _String _str = (_String)str;
  ull strLen = String_Length(str);
  if (pos < 0 || pos >= strLen)
    return false;
  if (pos + len > strLen)
    len = strLen - pos;
  _String _substr = (_String)substr;
  return _str->charArray->functions->replaceRange(_str->charArray, pos, len, _substr->charArray->data, String_Length(substr));
}

lboolean String_Remove(String str, ull pos)
{
  if (!str)
    return false;
  _String _str = (_String)str;
  ull len = String_Length(str);
  if (pos < 0 || pos >= len)
    return false;
  return _str->charArray->functions->remove(_str->charArray, pos);
}

lboolean String_RemoveRange(String str, ull pos, ull len)
{
  if (!str)
    return false;
  _String _str = (_String)str;
  ull strLen = String_Length(str);
  if (pos < 0 || pos >= strLen)
    return false;
  if (pos + len > strLen) // 防止删除末尾空字符
    len = strLen - pos;
  return _str->charArray->functions->removeRange(_str->charArray, pos, len);
}

ull String_FindChar(String str, char c)
{
  if (!str)
    return false;
  ull strLen = String_Length(str);
  _String _str = (_String)str;
  for (ull i = 0; i < strLen; i++)
  {
    if (*_str->charArray->functions->at(_str->charArray, i) == c)
      return i;
  }
  return strLen;
}

ull String_FindCString(String str, const char *substr, ull substrLen, lboolean autoCalculateLength)
{
  if (!str || !substr)
    return false;
  if (autoCalculateLength)
    substrLen = strlen(substr);
  if (substrLen == 0)
    return 0; // 空字符串总是匹配成功，返回0
  ull strLen = String_Length(str);
  _String _str = (_String)str;
  for (ull i = 0; i + substrLen <= strLen; i++) // 注意 <= 符号，防止越界
  {
    lboolean match = true;
    for (ull j = 0; j < substrLen; j++)
    {
      if (*_str->charArray->functions->at(_str->charArray, i + j) != substr[j])
      {
        match = false;
        break;
      }
    }
    if (match)
      return i;
  }
  return strLen;
}

ull String_FindString(String str, String substr)
{
  return String_FindCString(str, String_ToCString(substr), String_Length(substr), false);
}

lboolean String_Clear(String str)
{
  if (!str)
    return false;
  _String _str = (_String)str;
  lboolean result = _str->charArray->functions->clear(_str->charArray);
  if (result)
  {
    // 清空成功后，重新添加末尾的'\0'
    _str->charArray->functions->append(_str->charArray, '\0');
  }
  return result;
}

lboolean String_IsEmpty(String str)
{
  if (!str)
    return true;
  return String_Length(str) == 0;
}

String String_Copy(String str)
{
  if (!str)
    return NULL;
  _String _str = (_String)str;
  return String_Create(_str->charArray->data, String_Length(str), false);
}

String String_Concat(String str1, String str2)
{
  if (!str1 || !str2)
    return NULL;
  _String _str1 = (_String)str1;
  _String _str2 = (_String)str2;
  ull len1 = _str1->charArray->functions->length(_str1->charArray) - 1; // 减去末尾的'\0'
  ull len2 = _str2->charArray->functions->length(_str2->charArray) - 1; // 减去末尾的'\0'
  char *dataSet = malloc(sizeof(char) * (len1 + len2 + 1));
  for (ull i = 0; i < len1; i++)
    dataSet[i] = _str1->charArray->data[i];
  for (ull i = 0; i < len2; i++)
    dataSet[len1 + i] = _str2->charArray->data[i];
  dataSet[len1 + len2] = '\0';
  String newStr = String_Create(dataSet, len1 + len2, false);
  free(dataSet);
  return newStr;
}

String String_Substring(String str, ull start, ull length)
{
  if (!str)
    return NULL;
  _String _str = (_String)str;
  ull strLen = String_Length(str);
  if (start < 0 || start >= strLen)
    return NULL;
  if (start + length > strLen) // 防止越界
    length = strLen - start;
  char *dataSet = malloc(sizeof(char) * (length + 1));
  for (ull i = 0; i < length; i++)
    dataSet[i] = _str->charArray->data[start + i];
  dataSet[length] = '\0';
  String newStr = String_Create(dataSet, length, false);
  free(dataSet);
  return newStr;
}

int String_Compare(String str1, String str2)
{
  if (!str1 || !str2)
    return 0; // NULL视为相等
  _String _str1 = (_String)str1;
  _String _str2 = (_String)str2;
  ull len1 = String_Length(str1);
  ull len2 = String_Length(str2);
  ull minLen = len1 < len2 ? len1 : len2;
  for (ull i = 0; i < minLen; i++)
  {
    char c1 = *_str1->charArray->functions->at(_str1->charArray, i);
    char c2 = *_str2->charArray->functions->at(_str2->charArray, i);
    if (c1 != c2)
      return c1 - c2;
  }
  return len1 - len2; // 长度不同则视为不同
}

static StringApi _String_Global_Api = {
    .create = String_Create,
    .destroy = String_Destroy,
    .length = String_Length,
    .toCString = String_ToCString,
    .get = String_Get,
    .insertChar = String_InsertChar,
    .appendChar = String_AppendChar,
    .insertCString = String_InsertCString,
    .appendCString = String_AppendCString,
    .insertString = String_InsertString,
    .appendString = String_AppendString,
    .replaceChar = String_ReplaceChar,
    .replaceCString = String_ReplaceCString,
    .replaceString = String_ReplaceString,
    .remove = String_Remove,
    .removeRange = String_RemoveRange,
    .findChar = String_FindChar,
    .findCString = String_FindCString,
    .findString = String_FindString,
    .clear = String_Clear,
    .isEmpty = String_IsEmpty,

    // 对象层面操作
    .copy = String_Copy,
    .concat = String_Concat,
    .substring = String_Substring,
    .compare = String_Compare
};