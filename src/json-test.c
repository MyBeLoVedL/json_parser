#include "json.h"
#include <stdio.h>

#define EXPECT_INT(expect, actual)                                             \
  do {                                                                         \
    test_count++;                                                              \
    if (expect == actual)                                                      \
      test_passed++;                                                           \
    else {                                                                     \
      printf("%s -> %d : expect :%d  actual : %d \n", __FILE__, __LINE__,      \
             expect, actual);                                                  \
    }                                                                          \
  } while (0)

static int test_count;
static int test_passed;

void test_null() {
  json_node node;
  json_context context;
  context.text = "   ";
  EXPECT_INT(PARSE_EXPECT_VALUE, parse_node(&node, &context));
  EXPECT_INT(JSON_NULL, node.type);

  context.text = " nul";
  EXPECT_INT(PARSE_INVALID_VALUE, parse_node(&node, &context));
  EXPECT_INT(JSON_NULL, node.type);
}

void test_true() {
  json_node node;
  json_context context;
  context.text = " true  ";
  EXPECT_INT(PARSE_SUCCESS, parse_node(&node, &context));
  EXPECT_INT(JSON_TRUE, node.type);
}

void test_false() {
  json_node node;
  json_context context;
  context.text = " false  ";
  EXPECT_INT(PARSE_SUCCESS, parse_node(&node, &context));
  EXPECT_INT(JSON_FALSE, node.type);
}

void test_parse() {
  test_true();
  test_false();
  test_null();
}
int main() {
  test_parse();
  printf("total tests -> %d\npassed -> %d\npass rate -> (%3.2f%%)\n",
         test_count, test_passed, (test_passed * 100.0) / test_count);
}
