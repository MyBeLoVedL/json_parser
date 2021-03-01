#include "datatype.h"
#include "json.h"
#include <stdio.h>

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
static int test_count;
static int test_passed;

static json_node test_node;
static json_context test_context;

#define test_value(error, string, json_type)                  \
  do                                                          \
  {                                                           \
    test_node.type = JSON_UNKOWN;                             \
    test_context.text = string;                               \
    EXPECT_INT(error, parse_node(&test_node, &test_context)); \
    EXPECT_INT(json_type, test_node.type);                    \
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

void test_parse()
{
  test_true();
  test_false();
  test_null();
  test_invalid_value();
  test_expect_value();
  test_root_not_singular();
}

void test_number()
{
  test_value(PARSE_SUCCESS, "3.14159", JSON_NUMBER);
  EXPECT_DOUBLE(3.14159, test_node.numeric);
  test_value(PARSE_SUCCESS, "-3.14159", JSON_NUMBER);
  EXPECT_DOUBLE(-3.14159, test_node.numeric);

  test_value(PARSE_SUCCESS, "0", JSON_NUMBER);
  EXPECT_DOUBLE(0.0, test_node.numeric);

  test_value(PARSE_SUCCESS, "-0.0", JSON_NUMBER);
  EXPECT_DOUBLE(0.0, test_node.numeric);

  test_value(PARSE_SUCCESS, "  1  ", JSON_NUMBER);
  EXPECT_DOUBLE(1.0, test_node.numeric);

  test_value(PARSE_SUCCESS, "-1", JSON_NUMBER);
  EXPECT_DOUBLE(-1.0, test_node.numeric);

  test_value(PARSE_SUCCESS, "  1E10", JSON_NUMBER);
  EXPECT_DOUBLE(1E10, test_node.numeric);

  test_value(PARSE_SUCCESS, "0", JSON_NUMBER);
  EXPECT_DOUBLE(0.0, test_node.numeric);

  test_value(PARSE_SUCCESS, "1E+10", JSON_NUMBER);
  EXPECT_DOUBLE(1E+10, test_node.numeric);

  test_value(PARSE_SUCCESS, "1E-10", JSON_NUMBER);
  EXPECT_DOUBLE(1E-10, test_node.numeric);

  test_value(PARSE_SUCCESS, "-1E-10", JSON_NUMBER);
  EXPECT_DOUBLE(-1E-10, test_node.numeric);

  test_value(PARSE_SUCCESS, "1.234E+10", JSON_NUMBER);
  EXPECT_DOUBLE(1.234E+10, test_node.numeric);

  test_value(PARSE_SUCCESS, "-1.23E+10", JSON_NUMBER);
  EXPECT_DOUBLE(-1.23E+10, test_node.numeric);

  test_value(PARSE_INVALID_VALUE, "+0", JSON_UNKOWN);
  test_value(PARSE_INVALID_VALUE, ".123", JSON_UNKOWN);
  test_value(PARSE_INVALID_VALUE, "1.", JSON_UNKOWN);
  test_value(PARSE_INVALID_VALUE, "inf", JSON_UNKOWN);
  test_value(PARSE_INVALID_VALUE, "007", JSON_UNKOWN);
}

int main()
{
  // test_parse();
  test_number();
  printf("total tests -> %d\npassed -> %d\npass rate -> (%3.2f%%)\n",
         test_count, test_passed, (test_passed * 100.0) / test_count);
}

// 17752254783@163.com