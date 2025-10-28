#ifndef STRING_H
#define STRING_H
#include "../types.h"

// 声明
typedef struct String *String;
typedef struct StringApi StringApi;

// 定义

typedef struct String
{
  StringApi *functions;
} *String;

typedef struct StringApi
{
  /**
   * 从C字符串创建一个新的String对象
   * @param cstr 输入的C字符串，为空时返回空字符串对象指针（非NULL）
   * @param len 输入的C字符串长度
   * @param autoCalculateLength 是否自动计算长度（true则忽略len参数）
   * @return 返回创建的String对象指针
   */
  String (*create)(const char *cstr, ull len, lboolean autoCalculateLength);

  /**
   * 销毁String对象，释放内存
   * @param str 需要销毁的String对象指针
   * @return true成功，false失败
   */
  lboolean (*destroy)(String str);
  // 对象内部操作
  /**
   * 获取String对象的长度
   * @param str 输入的String对象指针
   * @return 返回String对象的字符长度
   */
  ull (*length)(String str);
  /**
   * 将String对象转换为C字符串
   * @param str 输入的String对象指针
   * @return 返回对应的C字符串指针（注意内存管理）
   */
  const char *(*toCString)(String str);
  /**
   * 获取某个位置的字符
   * @param str 输入的String对象指针
   * @param pos 输入的位置（0-based索引）
   * @return 返回该位置的字符副本，如果pos越界返回'\0'
   */
  char (*get)(String str, ull pos);
  /**
   * 插入字符到String对象的指定位置
   * @param str 输入的String对象指针
   * @param pos 插入位置（0-based索引）
   * @param c 插入的字符
   * @return true成功，false失败
   */
  lboolean (*insertChar)(String str, ull pos, char c);
  /**
   * 在String对象的末尾添加字符
   * @param str 输入的String对象指针
   * @param c 添加的字符
   * @return true成功，false失败
   */
  lboolean (*appendChar)(String str, char c);
  /**
   * 插入C字符串到String对象的指定位置
   * @param str 输入的String对象指针
   * @param pos 插入位置（0-based索引）
   * @param substr 插入的C字符串
   * @param len 插入的C字符串长度
   * @param autoCalculateLength 是否自动计算长度（true则忽略len参数）
   * @return true成功，false失败
   */
  lboolean (*insertCString)(String str, ull pos, const char *substr, ull len, lboolean autoCalculateLength);
  /**
   * 在String对象的末尾添加C字符串
   * @param str 输入的String对象指针
   * @param substr 添加的C字符串
   * @param len 添加的C字符串长度
   * @param autoCalculateLength 是否自动计算长度（true则忽略len参数）
   * @return true成功，false失败
   */
  lboolean (*appendCString)(String str, const char *substr, ull len, lboolean autoCalculateLength);
  /**
   * 插入字符串到String对象的指定位置
   * @param str 输入的String对象指针
   * @param pos 插入位置（0-based索引）
   * @param substr 插入的字符串
   */
  lboolean (*insertString)(String str, ull pos, String substr);
  /**
   * 在String对象的末尾添加字符串
   * @param str 输入的String对象指针
   * @param substr 添加的字符串
   */
  lboolean (*appendString)(String str, String substr);
  /**
   * 替换String对象的指定位置的字符
   * @param str 输入的String对象指针
   * @param pos 替换位置（0-based索引）
   * @param c 替换的字符
   */
  lboolean (*replaceChar)(String str, ull pos, char c);
  /**
   * 替换String对象的指定位置的子串
   * @param str 输入的String对象指针
   * @param pos 替换位置（0-based索引）
   * @param len 替换长度
   * @param substr 替换的子串
   * @param substrLen 替换的子串长度
   * @param autoCalculateLen 是否自动计算长度（true则忽略len参数）
   */
  lboolean (*replaceCString)(String str, ull pos, ull len, const char *substr, ull substrLen, lboolean autoCalculateLen);
  /**
   * 替换String对象的指定位置的字符串
   * @param str 输入的String对象指针
   * @param pos 替换位置（0-based索引）
   * @param len 替换长度
   * @param substr 替换的字符串
   */
  lboolean (*replaceString)(String str, ull pos, ull len, String substr);
  /**
   * 删除String对象的指定位置的字符
   * @param str 输入的String对象指针
   * @param pos 删除位置（0-based索引）
   */
  lboolean (*remove)(String str, ull pos);
  /**
   * 删除String对象的指定位置的多个字符
   * @param str 输入的String对象指针
   * @param pos 删除位置（0-based索引）
   * @param len 删除长度
   */
  lboolean (*removeRange)(String str, ull pos, ull len);
  /**
   * 查找字符在String对象中出现的第一个位置
   * @param str 输入的String对象指针
   * @param c 输入的字符
   * @return 返回字符在String对象中的位置（0-based索引），如果未找到返回StringApi->length()值
   */
  ull (*findChar)(String str, char c);
  /**
   * 查找C样式子串在String对象中出现的第一个位置
   * @param str 输入的String对象指针
   * @param substr 输入的子串
   * @param substrLen 输入的子串长度
   * @param autoCalculateLength 是否自动计算长度（true则忽略substrLen参数
   * @return 返回子串在String对象中的位置（0-based索引），如果未找到返回StringApi->length()值
   */
  ull (*findCString)(String str, const char *substr, ull substrLen, lboolean autoCalculateLength);
  /**
   * 查找字符串在String对象中出现的第一个位置
   * @param str 输入的String对象指针
   * @param substr 输入的字符串
   * @return 返回字符串在String对象中的位置（0-based索引），如果未找到返回StringApi->length()值
   */
  ull (*findString)(String str, String substr);

  /**
   * 清空String对象的内容
   * @param str 输入的String对象指针
   */
  lboolean (*clear)(String str);
  /**
   * 判断String对象是否为空
   * @param str 输入的String对象指针
   * @return true为空，false不为空
   */
  lboolean (*isEmpty)(String str);

  // 对象层面操作

  /**
   * 从一个String对象创建一个新的String对象
   * @param str 输入的String对象指针
   * @return 返回创建的新的String对象指针
   */
  String (*copy)(String str);
  /**
   * 连接两个String对象，返回一个新的String对象
   * @param str1 第一个String对象指针
   * @param str2 第二个String对象指针
   * @return 返回连接后的新String对象指针
   */
  String (*concat)(String str1, String str2);
  /**
   * 获取子串
   * @param str 原始String对象指针
   * @param start 子串起始位置（0-based索引）
   * @param length 子串长度
   * @return 返回子串的String对象指针
   */
  String (*substring)(String str, ull start, ull length);
  /**
   * 比较两个String对象的内容
   * @param str1 第一个String对象指针
   * @param str2 第二个String对象指针
   * @return 如果str1 < str2返回负值，str1 == str2返回0，str1 > str2返回正值
   */
  int (*compare)(String str1, String str2);
} StringApi;

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * 从C字符串创建一个新的String对象
   * @param cstr 输入的C字符串，为空时返回空字符串对象指针（非NULL）
   * @param len 输入的C字符串长度
   * @param autoCalculateLength 是否自动计算长度（true则忽略len参数）
   * @return 返回创建的String对象指针
   */
  String String_Create(const char *cstr, ull len, lboolean autoCalculateLength);
  /**
   * 获取String对象的API指针
   * @return 返回String对象的API指针
   */
  const StringApi* String_GetApi(void);
#ifdef __cplusplus
}
#endif

#endif // STRING_H