#pragma once
#include "datatype.h"

void say_hello(const char *);

typedef enum
{
  PARSE_SUCCESS,
  PARSE_FAILED,
  PARSE_INVALID_VALUE,
  PARSE_EXPECT_VALUE,
  PARSE_ROOT_NOT_SINGULAR,
  PARSE_UNMATCHED_QUATATION_MARK,
  PARSE_INVALID_CHAR_ESCAPE,
  PARSE_INVALID_CHAR,
  PARSE_INVALID_HEX,
  PARSE_INVALID_UNICODE_SURROGATE,
  PARSE_ARRAY_UNMATCHED_SQUARE_BRACKET
} parse_result;

typedef enum
{
  JSON_NULL,
  JSON_TRUE,
  JSON_FALSE,
  JSON_NUMBER,
  JSON_STRING,
  JSON_ARRAY,
  JSON_OEJECT,
  JSON_UNKOWN
} json_type;

typedef struct node
{
  json_type type;
  union
  {
    double numeric;
    struct
    {
      char *start;
      u32 len;
    };
    struct
    {
      struct node *arr_start;
      u32 arr_len;
    };
  };
} json_node;

typedef struct
{
  const char *text;
  char *stack;
  u32 top, size;
} json_context;

parse_result parse_value(json_node *, json_context *);

parse_result parse_node(json_node *, const char *);

json_node *get_array_element(json_node *node, u32 index);

bool validate_number(const char *);


void free_node(json_node *node);