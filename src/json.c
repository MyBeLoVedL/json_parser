#include "json.h"
#include "datatype.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(context, ch) assert(*context->text == (ch))

void say_hello(const char *name) { printf("hello,%s", name); }

#define is_char(ch) (((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z'))
#define is_space(ch) ((ch) == ' ' || (ch) == '\n' || ch == '\r' || (ch) == '\t')
#define is_digit(d) ((d) >= '0' && (d) <= '9')
#define is_natural(d) ((d) >= '1' && (d) <= '9')

bool validate_number(const char *src)
{
  if (*src == '-')
    src++;
  if (*src == '0')
    src++;
  else if (is_natural(*src))
  {
    while (is_digit(*src))
      src++;
  }
  else
    return F;

  if (*src == '.')
  {
    src++;
    if (is_digit(*src))
      src++;
    else
      return F;
    while (is_digit(*src))
      src++;
  }

  if (*src == 'e' || *src == 'E')
  {
    src++;
    if (*src == '+' || *src == '-')
      src++;
    if (is_digit(*src))
      src++;
    else
      return F;
    while (is_digit(*src))
      src++;
  }
  while (is_space(*src))
    src++;
  if (*src == 0)
    return T;
  return F;
}

void parse_space(json_context *context)
{
  char *cur = context->text;
  while (*cur == ' ' || *cur == '\n' || *cur == '\t' || *cur == '\r' ||
         *cur == '\r')
    cur++;
  context->text = cur;
}

parse_result parse_literal(json_node *node, json_context *context,
                           const char *target)
{
  int len = strlen(target);
  if (strncmp(context->text, target, len) || is_char(context->text[len]))
    return PARSE_INVALID_VALUE;
  char *cur = context->text + len;
  while (is_space(*cur))
    cur++;
  if (*cur != 0)
    return PARSE_ROOT_NOT_SINGULAR;
  context->text += len;
  return PARSE_SUCCESS;
}

parse_result parse_null(json_node *node, json_context *context)
{
  parse_result res = parse_literal(node, context, "null");
  if (res == PARSE_SUCCESS)
    node->type = JSON_NULL;
  return res;
}

parse_result parse_true(json_node *node, json_context *context)
{
  parse_result res = parse_literal(node, context, "true");
  if (res == PARSE_SUCCESS)
    node->type = JSON_TRUE;
  return res;
}

parse_result parse_false(json_node *node, json_context *context)
{
  parse_result res = parse_literal(node, context, "false");
  if (res == PARSE_SUCCESS)
    node->type = JSON_FALSE;
  return res;
}

parse_result parse_number(json_node *node, json_context *context)
{
  bool valid = validate_number(context->text);
  if (!valid)
    return PARSE_INVALID_VALUE;
  node->type = JSON_NUMBER;
  char *end;
  node->numeric = strtod(context->text, &end);
  assert(end != context->text);
  context->text = end;
  return PARSE_SUCCESS;
}

parse_result parse_value(json_node *node, json_context *context)
{
  parse_result res;
  switch ((context->text)[0])
  {
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
    res = parse_number(node, context);
    break;
  }
  return res;
}

parse_result parse_node(json_node *node, json_context *context)
{
  parse_space(context);
  return parse_value(node, context);
}
