#include "json.h"
#include "datatype.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define EXPECT(context, ch) assert(*context->text == (ch))

void say_hello(const char *name) { printf("hello,%s", name); }

bool is_char(char ch) {
  if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
    return T;
  return F;
}

bool is_space(char ch) {
  if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t')
    return T;
  return F;
}

void parse_space(json_context *context) {
  char *cur = context->text;
  while (*cur == ' ' || *cur == '\n' || *cur == '\t' || *cur == '\r' ||
         *cur == '\r')
    cur++;
  context->text = cur;
}

parse_result parse_null(json_node *node, json_context *context) {
  if (strncmp(context->text, "null", 4) || is_char(context->text[4]))
    return PARSE_INVALID_VALUE;
  char *cur = context->text + 4;
  while (is_space(*cur))
    cur++;
  if (*cur != 0)
    return PARSE_ROOT_NOT_SINGULAR;
  context->text += 4;
  node->type = JSON_NULL;
  return PARSE_SUCCESS;
}

parse_result parse_true(json_node *node, json_context *context) {
  if (strncmp(context->text, "true", 4) || is_char(context->text[4]))
    return PARSE_INVALID_VALUE;
  char *cur = context->text + 4;
  while (is_space(*cur))
    cur++;
  if (*cur != 0)
    return PARSE_ROOT_NOT_SINGULAR;
  context->text += 4;
  node->type = JSON_TRUE;
  return PARSE_SUCCESS;
}

parse_result parse_false(json_node *node, json_context *context) {
  if (strncmp(context->text, "false", 5) || is_char(context->text[5]))
    return PARSE_INVALID_VALUE;
  char *cur = context->text + 5;
  while (is_space(*cur))
    cur++;
  if (*cur != 0)
    return PARSE_ROOT_NOT_SINGULAR;
  context->text += 5;
  node->type = JSON_FALSE;
  return PARSE_SUCCESS;
}

parse_result parse_value(json_node *node, json_context *context) {
  parse_result res;
  switch ((context->text)[0]) {
  case 0:
    return PARSE_EXPECT_VALUE;
  case 'n':
    res = parse_null(node, context);
    break;
  case 't':
    res = parse_true(node, context);
    break;

  case 'f':
    res = parse_false(node, context);
    break;
  default:
    return PARSE_INVALID_VALUE;
    break;
  }
  return res;
}

parse_result parse_node(json_node *node, json_context *context) {
  parse_space(context);
  return parse_value(node, context);
}
