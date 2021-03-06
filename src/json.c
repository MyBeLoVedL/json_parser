#include "json.h"
#include "datatype.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(context, ch) assert(*context->text == (ch))

#ifndef INIT_STACK_SIZE
#define INIT_STACK_SIZE 256
#endif

void say_hello(const char *name) { printf("hello,%s", name); }

#define is_char(ch) (((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z'))
#define is_space(ch) ((ch) == ' ' || (ch) == '\n' || ch == '\r' || (ch) == '\t')
#define is_digit(d) ((d) >= '0' && (d) <= '9')
#define is_natural(d) ((d) >= '1' && (d) <= '9')

void stack_push(json_context *c, const char *src, u32 push_len)
{
  if (c->top + push_len >= c->size)
  {
    c->size += c->size >> 1;
    c->stack = realloc(c->stack, c->size);
  }
  memcpy(c->stack + c->top, src, push_len);
  c->top += push_len;
}

char *stack_pop(json_context *c, u32 pop_len)
{
  assert(c->top >= pop_len);
  c->top -= pop_len;
  return c->stack + c->top;
}

void free_node_string(json_node *node)
{
  assert(node->type == JSON_STRING);
  free((void *)node->content.string.start);
  node->type = JSON_UNKOWN;
}

json_type node_get_type(json_node *node)
{
  return node->type;
}

bool node_get_bool(json_node *node)
{
  assert(node->type == JSON_TRUE || node->type == JSON_FALSE);
  if (node->type == JSON_TRUE)
    return T;
  else
    return F;
}

void node_set_bool(json_node *node, bool t)
{
  assert(node->type == JSON_TRUE || node->type == JSON_FALSE);
  if (t)
    node->type = JSON_TRUE;
  else
    node->type = JSON_FALSE;
}

double node_get_numeric(json_node *node)
{
  assert(node->type == JSON_NUMBER);
  return node->content.numeric;
}

void node_set_numeric(json_node *node, double n)
{
  assert(node->type == JSON_NUMBER);
  node->content.numeric = n;
}

char *node_get_string(json_node *node)
{
  assert(node->type == JSON_STRING);
  return node->content.string.start;
}

u32 node_get_string_len(json_node *node)
{
  assert(node->type == JSON_STRING);
  return node->content.string.len;
}

void set_node_string(json_node *node, const char *src, u32 len)
{
  assert(node != NULL && (src != NULL || len == 0));
  node->content.string.start = malloc(len + 1);
  memcpy(node->content.string.start, src, len);
  *(node->content.string.start + len) = 0;
  node->type = JSON_STRING;
}

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
  const char *cur = context->text;
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
  const char *cur = context->text + len;
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
  node->content.numeric = strtod(context->text, &end);
  assert(end != context->text);
  context->text = end;
  return PARSE_SUCCESS;
}

parse_result parse_string(json_node *node, json_context *context)
{
  const char *p = context->text;
  assert(*p == '\"');
  p++;
  u32 len;
  const char *head = p;
  char explicit_char[9] = {'\"', '\\', '/', 'b', 'f', 'n', 'r', 't', 'u'};
  char escape_char[9] = {0x22, 0x5c, 0x2f, 0x08, 0x0c, 0x0a, 0x0d, 0x09, 0x75};
  char tmp_char;
  char tmp;
  u32 cur_len;
  while (1)
  {
    switch (*p++)
    {
    case '\"':
      stack_push(context, head, p - head - 1);
      u32 top = context->top;
      const char *start = stack_pop(context, top);
      set_node_string(node, start, top);
      context->text = p;
      return PARSE_SUCCESS;
    case '\0':
      context->top = 0;
      return PARSE_UNMATCHED_QUATATION_MARK;
      break;
    case '\\':
      // int str_seg_len = //p - head - 1;
      cur_len = p - head - 1;
      tmp_char = *(p);
      p++;
      int i;
      for (i = 0; i < 9; i++)
        if (explicit_char[i] == tmp_char)
        {
          tmp_char = escape_char[i];
          break;
        }
      if (i == 9)
      {
        context->top = 0;
        return PARSE_INVALID_CHAR_ESCAPE;
      }
      stack_push(context, head, cur_len);
      stack_push(context, &tmp_char, 1);
      head = p;
      break;
    default:
      tmp = *(p - 1);
      if (!((tmp >= 0x20 && tmp <= 0x21) || (tmp >= 0x23 && tmp <= 0x5b) || (tmp >= 0x5d)))
        return PARSE_INVALID_CHAR;
      break;
    }
  }
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
  case '\"':
    res = parse_string(node, context);
    break;
  default:
    res = parse_number(node, context);
    break;
  }
  return res;
}

parse_result parse_node(json_node *node, const char *src)
{
  json_context context;
  context.text = src;
  context.stack = malloc(INIT_STACK_SIZE);
  context.size = INIT_STACK_SIZE;
  context.top = 0;
  parse_space(&context);
  u32 res = parse_value(node, &context); //TODO: stack free
  assert(context.top == 0);
  free(context.stack);
  if (res == PARSE_SUCCESS)
  {
    parse_space(&context);
    if (*context.text != '\0')
    {
      node->type = JSON_UNKOWN;
      return PARSE_ROOT_NOT_SINGULAR;
    }
  }
  return res;
}
