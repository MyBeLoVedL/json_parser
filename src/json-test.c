#include "datatype.h"
#include "json.h"
#include <stdio.h>
#include <string.h>

#define EXPECT_INT(expect, actual)                                        \
  do                                                                      \
  {                                                                       \
    test_count++;                                                         \
    if (expect == actual)                                                 \
      test_passed++;                                                      \
    else                                                                  \
    {                                                                     \
      printf("%s -> %d : expect :%d  actual : %d \n", __FILE__, __LINE__, \
             expect, actual);                                             \
    }                                                                     \
  } while (0)

#define EXPECT_DOUBLE(expect, actual)                                     \
  do                                                                      \
  {                                                                       \
    test_count++;                                                         \
    if (expect == actual)                                                 \
      test_passed++;                                                      \
    else                                                                  \
    {                                                                     \
      printf("%s -> %d : expect :%f  actual : %f \n", __FILE__, __LINE__, \
             expect, actual);                                             \
    }                                                                     \
  } while (0)
#define EXPECT_STRING(expect, actual)                                     \
  do                                                                      \
  {                                                                       \
    test_count++;                                                         \
    if (strcmp(expect, actual) == 0)                                      \
      test_passed++;                                                      \
    else                                                                  \
    {                                                                     \
      printf("%s -> %d : expect :%s  actual : %s \n", __FILE__, __LINE__, \
             expect, actual);                                             \
    }                                                                     \
  } while (0)
static int test_count;
static int test_passed;

static json_node test_node;

#define test_value(error, string, json_type)           \
  do                                                   \
  {                                                    \
    test_node.type = JSON_UNKOWN;                      \
    EXPECT_INT(error, parse_node(&test_node, string)); \
    EXPECT_INT(json_type, test_node.type);             \
  } while (0)

// test error cases
void test_invalid_value()
{
  test_value(PARSE_INVALID_VALUE, "  nul", JSON_UNKOWN);
}

void test_expect_value()
{
  test_value(PARSE_EXPECT_VALUE, "    ", JSON_UNKOWN);
}

void test_root_not_singular()
{
  test_value(PARSE_ROOT_NOT_SINGULAR, "  true eea", JSON_UNKOWN);
}

// test correct cases

void test_null()
{
  test_value(PARSE_SUCCESS, "  null  ", JSON_NULL);
}

void test_true()
{
  test_value(PARSE_SUCCESS, "  true  ", JSON_TRUE);
}

void test_false()
{
  test_value(PARSE_SUCCESS, "  false  ", JSON_FALSE);
}

void test_string()
{
  test_value(PARSE_SUCCESS, "\"hello,altair\"", JSON_STRING);
  EXPECT_STRING(test_node.content.string.start, "hello,altair");

  test_value(PARSE_SUCCESS, "\"hello\\naltair\"", JSON_STRING);
  EXPECT_STRING(test_node.content.string.start, "hello\naltair");

  test_value(PARSE_SUCCESS, "\"\\nhelloaltair\"", JSON_STRING);
  EXPECT_STRING(test_node.content.string.start, "\nhelloaltair");

  test_value(PARSE_UNMATCHED_QUATATION_MARK, "\"\\nhelloaltair", JSON_UNKOWN);
  // EXPECT_STRING(test_node.content.string.start, "");

  test_value(PARSE_INVALID_CHAR_ESCAPE, "\"\\nhelloa\\altair\"", JSON_UNKOWN);
  test_value(PARSE_INVALID_CHAR_ESCAPE, "\"\\v\"", JSON_UNKOWN);
  test_value(PARSE_INVALID_CHAR_ESCAPE, "\"\\x12\"", JSON_UNKOWN);
  test_value(PARSE_INVALID_CHAR, "\"\x01\"", JSON_UNKOWN);
  test_value(PARSE_INVALID_CHAR, "\"\x1F\"", JSON_UNKOWN);
}

void test_number()
{
  test_value(PARSE_SUCCESS, "3.14159", JSON_NUMBER);
  EXPECT_DOUBLE(3.14159, test_node.content.numeric);
  test_value(PARSE_SUCCESS, "-3.14159", JSON_NUMBER);
  EXPECT_DOUBLE(-3.14159, test_node.content.numeric);
  test_value(PARSE_SUCCESS, "0", JSON_NUMBER);
  EXPECT_DOUBLE(0.0, test_node.content.numeric);

  test_value(PARSE_SUCCESS, "-0.0", JSON_NUMBER);
  EXPECT_DOUBLE(0.0, test_node.content.numeric);

  test_value(PARSE_SUCCESS, "  1  ", JSON_NUMBER);
  EXPECT_DOUBLE(1.0, test_node.content.numeric);

  test_value(PARSE_SUCCESS, "-1", JSON_NUMBER);
  EXPECT_DOUBLE(-1.0, test_node.content.numeric);

  test_value(PARSE_SUCCESS, "  1E10", JSON_NUMBER);
  EXPECT_DOUBLE(1E10, test_node.content.numeric);

  test_value(PARSE_SUCCESS, "0", JSON_NUMBER);
  EXPECT_DOUBLE(0.0, test_node.content.numeric);

  test_value(PARSE_SUCCESS, "1E+10", JSON_NUMBER);
  EXPECT_DOUBLE(1E+10, test_node.content.numeric);

  test_value(PARSE_SUCCESS, "1E-10", JSON_NUMBER);
  EXPECT_DOUBLE(1E-10, test_node.content.numeric);

  test_value(PARSE_SUCCESS, "-1E-10", JSON_NUMBER);
  EXPECT_DOUBLE(-1E-10, test_node.content.numeric);

  test_value(PARSE_SUCCESS, "1.234E+10", JSON_NUMBER);
  EXPECT_DOUBLE(1.234E+10, test_node.content.numeric);

  test_value(PARSE_SUCCESS, "-1.23E+10", JSON_NUMBER);
  EXPECT_DOUBLE(-1.23E+10, test_node.content.numeric);

  test_value(PARSE_INVALID_VALUE, "+0", JSON_UNKOWN);
  test_value(PARSE_INVALID_VALUE, ".123", JSON_UNKOWN);
  test_value(PARSE_INVALID_VALUE, "1.", JSON_UNKOWN);
  test_value(PARSE_INVALID_VALUE, "inf", JSON_UNKOWN);
  test_value(PARSE_INVALID_VALUE, "007", JSON_UNKOWN);
}

void test_parse()
{
  test_true();
  test_false();
  test_null();
  test_invalid_value();
  test_expect_value();
  test_root_not_singular();
  test_number();
  test_string();
}

int main()
{
  test_parse();
  printf("total tests -> %d\npassed -> %d\npass rate -> (%3.2f%%)\n",
         test_count, test_passed, (test_passed * 100.0) / test_count);
}

// 17752254783@163.com