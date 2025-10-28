#ifndef TEXT_FORMATTING_H
#define TEXT_FORMATTING_H
#include "Universal/types.h"
#include "Universal/String/String.h"
// 类型定义，便于替换
typedef char TextCharType;  // 定义 文本字符 类型
typedef int TextIndentationCountType;       // 定义 文本缩进数 类型
typedef int TextLinesPerPageCountType;        // 定义 每页文本行数 类型
typedef int TextCharsPerLineCountType; // 定义 每行字符数 类型
// typedef int TextPagesCountType;        // 定义 文本页数 类型

// 定义 分页符 符号
#define PAGE_BREAK_CHAR '\f'
#define DEFAULT_INDENTATION_CHAR_COUNT 4
#define DEFAULT_MAX_LINES_PER_PAGE_COUNT 30
#define DEFAULT_MAX_CHARS_PER_LINE_COUNT 80
#define DEFAULT_ENABLE_WORD_WRAP 1
#define DEFAULT_ENABLE_MERGE_SPACES 1

typedef struct TextFormattingApi TextFormattingApi;
typedef struct TextFormatting *TextFormatting;

typedef struct TextFormatting
{
  TextFormattingApi *functions;
  int indentationCharCount;    // 缩进字符数
  int maxLinesPerPage;    // 每页最大行数
  int maxCharsPerLine;    // 每行最大字符数
  lboolean wordWrapEnabled; // 是否启用自动换行
  lboolean mergeSpacesEnabled; // 是否合并多个空格为一个空格
} *TextFormatting;

/**
 * TextFormatting API 接口
 */
typedef struct TextFormattingApi
{
  /**
   * 销毁 TextFormatting 对象
   * @param tf 需要销毁的 TextFormatting 对象
   * @return 成功返回1，失败返回0
   */
  lboolean (*destroy)(TextFormatting tf);

  /**
   * 执行设定好的格式化操作
   * @param tf TextFormatting 对象
   * @param str 目标字符串
   * @return 成功返回字符串对象，失败返回NULL
   * @note 应用格式化的时候务必确保 String 对象生命周期足够进行操作
   */
  String (*applyFormatting)(TextFormatting tf, String str);
  /**
   * 设置缩进级别
   * @param tf TextFormatting 对象
   * @param indentationCharCount 缩进字符数
   * @return 成功返回1，失败返回0
   * @note 缩进字符大小是空格大小，并非中文字符大小
   */
  lboolean (*setIndentation)(TextFormatting tf, TextIndentationCountType indentationCharCount);
  /**
   * 设置每页最大行数
   * @param tf TextFormatting 对象
   * @param maxLines 每页最大行数
   * @return 成功返回1，失败返回0
   * @note 该设置会影响分页符的插入位置，但不会影响原有的分页符
   */
  lboolean (*setPageMaxLines)(TextFormatting tf, TextLinesPerPageCountType maxLines);
  /**
   * 设置每行最大字符数
   * @param tf TextFormatting 对象
   * @param maxCharsPerLine 每行最大字符数
   * @return 成功返回1，失败返回0
   * @note 该设置会影响换行符的插入位置，但不会影响原有的换行符
   */
  lboolean (*setLineMaxChars)(TextFormatting tf, TextCharsPerLineCountType maxCharsPerLine);
  /**
   * 设置是否启用自动换行
   * @param tf TextFormatting 对象
   * @param enable 是否启用自动换行
   * @return 成功返回1，失败返回0
   */
  lboolean (*setWordWrap)(TextFormatting tf, lboolean enable);
  /**
   * 多个空格是否合并为一个空格
   * @param tf TextFormatting 对象
   * @param enable 是否启用合并
   * @return 成功返回1，失败返回0
   */
  lboolean (*setMergeSpaces)(TextFormatting tf, lboolean enable);
  /**
   * 设置默认格式化
   * @param tf TextFormatting 对象
   * @return 成功返回1，失败返回0
   */
  lboolean (*resetFormatting)(TextFormatting tf);

} TextFormattingApi;

TextFormatting TextFormatting_CreateWithDefaults(); // 创建一个带有默认设置的 TextFormatting 对象
TextFormatting TextFormatting_Create(TextIndentationCountType indentationCharCount, TextLinesPerPageCountType maxLinesPerPage, TextCharsPerLineCountType maxCharsPerLine, lboolean wordWrapEnabled, lboolean mergeSpacesEnabled);             // 创建一个 TextFormatting 对象
lboolean TextFormatting_Destroy(TextFormatting tf); // 销毁一个 TextFormatting 对象
String TextFormatting_ApplyFormatting(TextFormatting tf, String str);
lboolean TextFormatting_SetIndentation(TextFormatting tf, TextIndentationCountType indentationCharCount);
lboolean TextFormatting_SetPageMaxLines(TextFormatting tf, TextLinesPerPageCountType maxLines);
lboolean TextFormatting_SetLineMaxChars(TextFormatting tf, TextCharsPerLineCountType maxCharsPerLine);
lboolean TextFormatting_SetWordWrap(TextFormatting tf, lboolean enable);
lboolean TextFormatting_SetMergeSpaces(TextFormatting tf, lboolean enable);
lboolean TextFormatting_ResetFormatting(TextFormatting tf);
#endif // TEXT_FORMATTING_H