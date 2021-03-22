#include "datatype.h"
#include "json.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
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

void test_array()
{
  test_value(PARSE_SUCCESS, "[1,2]", JSON_ARRAY);
  EXPECT_INT(JSON_ARRAY, test_node.type);
  EXPECT_DOUBLE(1.0, test_node.arr_start->numeric);
  EXPECT_DOUBLE(2.0, (test_node.arr_start + 1)->numeric);

  free_node(&test_node);
  test_value(PARSE_SUCCESS, "[1,  \"hello\"]", JSON_ARRAY);
  // EXPECT_INT(JSON_ARRAY, test_node.type);
  EXPECT_INT(JSON_NUMBER, test_node.arr_start->type);
  EXPECT_DOUBLE(1.0, test_node.arr_start->numeric);
  EXPECT_STRING("hello", (test_node.arr_start + 1)->start);

  free_node(&test_node);
  test_value(PARSE_SUCCESS, "[1,  2,false]", JSON_ARRAY);
  EXPECT_INT(JSON_ARRAY, test_node.type);
  EXPECT_DOUBLE(1.0, test_node.arr_start->numeric);
  EXPECT_DOUBLE(2.0, get_array_element(&test_node, 1)->numeric);
  EXPECT_INT(JSON_FALSE, get_array_element(&test_node, 2)->type);

  free_node(&test_node);
  test_value(PARSE_SUCCESS, "[1,[true,2],false]", JSON_ARRAY);
  EXPECT_INT(JSON_ARRAY, test_node.type);
  EXPECT_DOUBLE(1.0, test_node.arr_start->numeric);
  EXPECT_INT(JSON_ARRAY, (test_node.arr_start + 1)->type);

  EXPECT_INT(JSON_FALSE, (test_node.arr_start + 2)->type);

  EXPECT_INT(JSON_ARRAY, (test_node.arr_start + 1)->type);

  EXPECT_INT(JSON_TRUE, (test_node.arr_start + 1)->arr_start->type);
  EXPECT_INT(JSON_NUMBER, ((test_node.arr_start + 1)->arr_start + 1)->type);

  free_node(&test_node);
  test_value(PARSE_SUCCESS, "[ 1 , 2 ]", JSON_ARRAY);
  EXPECT_INT(JSON_ARRAY, test_node.type);
  EXPECT_DOUBLE(1.0, test_node.arr_start->numeric);
  EXPECT_DOUBLE(2.0, (test_node.arr_start + 1)->numeric);

  // free_node(&test_node);
  // test_value(PARSE_SUCCESS, "[ null , false , true , 123 , \"abc\" ]", JSON_ARRAY);
  // EXPECT_INT(JSON_ARRAY, test_node.type);
  // EXPECT_INT(JSON_NULL, get_array_element(&test_node, 0)->type);
  // EXPECT_INT(JSON_TRUE, get_array_element(&test_node, 2)->type);
  // EXPECT_DOUBLE(123.0, get_array_element(&test_node, 3)->numeric);
  // EXPECT_STRING("abc", get_array_element(&test_node, 4)->start);
}

void test_string()
{
  test_value(PARSE_SUCCESS, "\"hello,altair\"", JSON_STRING);
  EXPECT_STRING(test_node.start, "hello,altair");

  free_node(&test_node);
  test_value(PARSE_SUCCESS, "\"hello\\naltair\"", JSON_STRING);
  EXPECT_STRING(test_node.start, "hello\naltair");

  free_node(&test_node);
  test_value(PARSE_SUCCESS, "\"\\nhelloaltair\"", JSON_STRING);
  EXPECT_STRING(test_node.start, "\nhelloaltair");

  test_value(PARSE_UNMATCHED_QUATATION_MARK, "\"\\nhelloaltair", JSON_UNKOWN);
  // EXPECT_STRING(test_node.start, "");

  test_value(PARSE_INVALID_CHAR_ESCAPE, "\"\\nhelloa\\altair\"", JSON_UNKOWN);
  test_value(PARSE_INVALID_CHAR_ESCAPE, "\"\\v\"", JSON_UNKOWN);
  test_value(PARSE_INVALID_CHAR_ESCAPE, "\"\\x12\"", JSON_UNKOWN);
  test_value(PARSE_INVALID_CHAR, "\"\x01\"", JSON_UNKOWN);
  test_value(PARSE_INVALID_CHAR, "\"\x1F\"", JSON_UNKOWN);
  test_value(PARSE_SUCCESS, "\"\\u20ac\"", JSON_STRING);
  EXPECT_STRING(test_node.start, "\u20ac");

  // EXPECT_STRING(test_node.start, "\ud834");
  test_value(PARSE_SUCCESS, "\"\\u0024\"", JSON_STRING);
  EXPECT_STRING(test_node.start, "\x24");

  test_value(PARSE_SUCCESS, "\"\\u00a2\"", JSON_STRING);
  EXPECT_STRING(test_node.start, "\xc2\xa2");

  test_value(PARSE_SUCCESS, "\"\\u20ac\"", JSON_STRING);
  EXPECT_STRING(test_node.start, "\xe2\x82\xac");

  test_value(PARSE_SUCCESS, "\"\\ud834\\udd1e\"", JSON_STRING);
  EXPECT_STRING(test_node.start, "\xf0\x9d\x84\x9e");
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

void test_object()
{
  test_value(PARSE_SUCCESS, "{\"age\":12}", JSON_OEJECT);
  EXPECT_STRING("age", get_obj_key(&test_node, 0));
  EXPECT_DOUBLE(12.0, get_obj_value(&test_node, 0)->numeric);

  test_value(PARSE_SUCCESS, "{\"age\":12, \"name\" : \"altair Lee\"}", JSON_OEJECT);
  EXPECT_STRING("age", get_obj_key(&test_node, 0));
  EXPECT_DOUBLE(12.0, get_obj_value(&test_node, 0)->numeric);

  EXPECT_STRING("name", get_obj_key(&test_node, 1));
  EXPECT_STRING("altair Lee", get_obj_value(&test_node, 1)->start);

  test_value(PARSE_SUCCESS, "{\"age\":12, \"name\" : \"altair Lee\",\"properties\" : {\"Chinese\": 130,\"Math\":150}}", JSON_OEJECT);

  EXPECT_DOUBLE(150.0, get_obj_value(get_obj_value(&test_node, 2), 1)->numeric);

  EXPECT_STRING("altair Lee", get_value_by_key(&test_node, "name")->start);
  EXPECT_DOUBLE(130.0, get_value_by_key(get_value_by_key(&test_node, "properties"), "Chinese")->numeric);

  test_value(PARSE_MISSING_SEMI_COLON, "{\"age\" 12}", JSON_UNKOWN);

  test_value(PARSE_MISSING_KEY, "{: 12}", JSON_UNKOWN);

  test_value(PARSE_MISSING_VALUE, "{\"age \": }", JSON_UNKOWN);
  // EXPECT_INT(0, get_value_by_key(get_value_by_key(&test_node, "properties"), "Cinese")->numeric);
  int fd = open("../test_json.txt", O_RDONLY);
  if (fd < 0)
  {
    perror("file open error");
    exit(EXIT_FAILURE);
  }
  void *buf = malloc(10240);
  u32 size = read(fd, buf, 10240);
  if (size <= 0)
  {
    perror("file read error");
    exit(EXIT_FAILURE);
  }
  *((char *)buf + size) = '\0';
  test_value(PARSE_SUCCESS, buf, JSON_OEJECT);
  EXPECT_STRING("altair", get_value_by_key(&test_node, "name")->start);
  EXPECT_DOUBLE(150.0, get_value_by_key(get_value_by_key(&test_node, "grades"), "Math")->numeric);
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
  test_array();
  test_object();
}

int main()
{
  test_parse();
  printf("total tests -> %d\npassed -> %d\npass rate -> (%3.2f%%)\n",
         test_count, test_passed, (test_passed * 100.0) / test_count);
}

// 17752254783@163.com