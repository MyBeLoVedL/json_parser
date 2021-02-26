#include "json.h"
#include <stdio.h>

int main() {
  json_node node;
  json_context context;
  context.text = " flse  ";
  switch (parse_node(&node, &context)) {
  case PARSE_SUCCESS:
    printf("node type -> %d", node.type);
    break;
  case PARSE_INVALID_VALUE:
    printf("invalid value detected ");
    break;
  default:
    break;
  }
}
