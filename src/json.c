#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "json.h"

// ANSI color codes
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define RED     "\e[0;31m"

void free_json_value(JsonValue* value) {
  if (!value) {
    return;
  }

  switch (value->type) {
    case JSON_STRING:
      free(value->string);
      break;

    case JSON_ARRAY: {
      for (int i = 0; i < value->array->count; ++i) {
        free_json_value(value->array->elements[i]);
      }
      free(value->array->elements);
      free(value->array);
      break;
    }

    case JSON_OBJECT: {
      if (value->object && value->object->pairs) {
        for (int i = 0; i < value->object->count; ++i) {
          if (value->object->pairs[i]) {
            free(value->object->pairs[i]->key);
            free_json_value(value->object->pairs[i]->value);
            free(value->object->pairs[i]);
          }
        }
        free(value->object->pairs);
      }
      free(value->object);
      break;
    }
  }

  free(value);
}

void print_indent(int indent) {
  for (int i = 0; i < indent; ++i) {
    printf("  ");
  }
}

void print_json_value(const JsonValue* value, const int indent, const bool color_enabled) {
  if (!value) {
    printf("NULL VALUE\n");
    return;
  }

  if (color_enabled) {
    switch (value->type) {
      case JSON_STRING: {
        printf("%sSTRING%s(%s\"%s\"%s)\n", YELLOW, RESET, GREEN, value->string, RESET);
        break;
      }

      case JSON_NUMBER: {
        printf("%sNUMBER%s(%s%s%s)\n", YELLOW, RESET, RED, value->number, RESET);
        break;
      }

      case JSON_BOOL: {
        printf("%sBOOLEAN%s(%s%s%s)\n", RED, RESET, CYAN, value->boolean ? "true" : "false", RESET);
        break;
      }

      case JSON_NULL: {
        printf("%sNULL%s\n", CYAN, RESET);
        break;
      }

      case JSON_ARRAY: {
        printf("%sARRAY%s [%s", CYAN, RESET, value->array->count > 0 ? "\n" : "");
        for (int i = 0; i < value->array->count; ++i) {
          print_indent(indent + 1);
          print_json_value(value->array->elements[i], indent + 1, color_enabled);
        }
        if (value->object->count > 0) {
          print_indent(indent);
        }
        printf("]\n");
        break;
      }
      
      case JSON_OBJECT: {
        printf("%sOBJECT%s {%s", CYAN, RESET, value->object->count > 0 ? "\n" : "");
        for (int i = 0; i < value->object->count; ++i) {
          print_indent(indent + 1);
          printf("\"%s\": ", value->object->pairs[i]->key);
          print_json_value(value->object->pairs[i]->value, indent + 1, color_enabled);
        }
        if (value->object->count > 0) {
          print_indent(indent);
        }
        printf("}\n");
        break;
      }
    }
  } else {
    switch (value->type) {
      case JSON_STRING: {
        printf("STRING(\"%s\")\n", value->string);
        break;
      }

      case JSON_NUMBER: {
        printf("NUMBER(%s)\n", value->number);
        break;
      }

      case JSON_BOOL: {
        printf("BOOLEAN(%s)\n", value->boolean ? "true" : "false");
        break;
      }

      case JSON_NULL: {
        printf("NULL\n");
        break;
      }

      case JSON_ARRAY: {
        printf("ARRAY [%s", value->array->count > 0 ? "\n" : "");
        for (int i = 0; i < value->array->count; ++i) {
          print_indent(indent + 1);
          print_json_value(value->array->elements[i], indent + 1, color_enabled);
        }
        if (value->object->count > 0) {
          print_indent(indent);
        }
        printf("]\n");
        break;
      }
      
      case JSON_OBJECT: {
        printf("OBJECT {%s", value->object->count > 0 ? "\n" : "");
        for (int i = 0; i < value->object->count; ++i) {
          print_indent(indent + 1);
          printf("\"%s\": ", value->object->pairs[i]->key);
          print_json_value(value->object->pairs[i]->value, indent + 1, color_enabled);
        }
        if (value->object->count > 0) {
          print_indent(indent);
        }
        printf("}\n");
        break;
      }
    }
  }
}