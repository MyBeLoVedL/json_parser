#pragma once

void say_hello(const char *);

typedef enum {
  PARSE_SUCCESS,
  PARSE_FAILED,
  PARSE_INVALID_VALUE,
  PARSE_EXPECT_VALUE
} parse_result;

typedef enum {
  JSON_NULL,
  JSON_TRUE,
  JSON_FALSE,
  JSON_NUMBER,
  JSON_STRING,
  JSON_ARRAY,
  JSON_OEJECT
} json_type;

typedef struct {
  json_type type;
} json_node;

typedef struct {
  const char *text;
} json_context;

parse_result parse_value(json_node *, json_context *);

parse_result parse_node(json_node *, json_context *);