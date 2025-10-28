#include <stdio.h>
#include "Universal/String/String.h"

void test_create_and_destroy(StringApi *api)
{
  String s1 = api->create("", 0, 1);
  String s2 = api->create("hello", 5, 0);
  String s3 = api->create("world", 0, 1);
  printf("Create: %s, %s, %s\n", api->toCString(s1), api->toCString(s2), api->toCString(s3));
  api->destroy(s1);
  api->destroy(s2);
  api->destroy(s3);
}

void test_insert_append(StringApi *api)
{
  String s = api->create("abc", 0, 1);
  api->insertChar(s, 1, 'X');
  api->appendChar(s, 'Y');
  api->insertCString(s, 2, "123", 3, false);
  api->appendCString(s, "456", 0, true);
  printf("Insert/Append: %s\n", api->toCString(s));
  api->destroy(s);
}

void test_replace_remove(StringApi *api)
{
  String s = api->create("abcdef", 0, 1);
  api->replaceChar(s, 2, 'Z');
  printf("Replace/Remove: %s\n", api->toCString(s));
  api->replaceCString(s, 1, 2, "XY", 2, 0);
  printf("Replace/Remove: %s\n", api->toCString(s));
  api->remove(s, 0);
  printf("Replace/Remove: %s\n", api->toCString(s));
  api->removeRange(s, 1, 2);
  printf("Replace/Remove: %s\n", api->toCString(s));
  api->destroy(s);
}

void test_find(StringApi *api)
{
  String s = api->create("abcabc", 0, 1);
  ull pos1 = api->findChar(s, 'b');
  ull pos2 = api->findCString(s, "ca", 2, 0);
  ull pos3 = api->findString(s, s);
  ull pos4 = api->findCString(s, "d", 0, 1);
  printf("Find: %llu, %llu, %llu, %llu\n", pos1, pos2, pos3, pos4);
  api->destroy(s);
}

void test_clear_isEmpty(StringApi *api)
{
  String s = api->create("notempty", 0, 1);
  api->clear(s);
  printf("IsEmpty after clear: %d, contents: %s\n", api->isEmpty(s), api->toCString(s));
  api->destroy(s);
}

void test_copy_concat_substring_compare(StringApi *api)
{
  String s1 = api->create("foo", 0, 1);
  String s2 = api->copy(s1); // print
  String s3 = api->create("bar", 0, 1);
  String s4 = api->concat(s1, s3); // print
  String s5 = api->substring(s4, 1, 3); // print
  int cmp = api->compare(s1, s3); // return s1 第一个与 s2 不同的字符 - 同位置 s2 的字符
  printf("Copy: %s, Concat: %s, Substring: %s, Compare: %d\n", api->toCString(s2), api->toCString(s4), api->toCString(s5), cmp);
  api->destroy(s1);
  api->destroy(s2);
  api->destroy(s3);
  api->destroy(s4);
  api->destroy(s5);
}

int main()
{
  String str = String_Create("example", 7, 0);
  StringApi *api = str->functions;

  test_create_and_destroy(api);
  test_insert_append(api);
  test_replace_remove(api);
  test_find(api);
  test_clear_isEmpty(api);
  test_copy_concat_substring_compare(api);

  return 0;
}