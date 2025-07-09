#ifndef JSON_H
#define JSON_H

#include <stdbool.h>
#include "tokenizer.h"
#include "parser.h"

typedef enum jsonType {
  JSON_NULL,
  JSON_BOOL,
  JSON_NUMBER,
  JSON_STRING,
  JSON_ARRAY,
  JSON_OBJECT,
} JsonType;

typedef struct JsonValue JsonValue;
typedef struct JsonObject JsonObject;
typedef struct JsonArray JsonArray;
typedef struct JsonPair JsonPair;

struct JsonValue {
  JsonType type;
  union {
    bool boolean;
    char* number; // to keep the actual numbers format from the json file
    char* string;
    JsonArray* array;
    JsonObject* object;
  };
};

struct JsonArray {
  JsonValue** elements;
  int count;
};

struct JsonPair {
  char* key;
  JsonValue* value;
};

struct JsonObject {
  JsonPair** pairs;
  int count;
};

void free_json_value(JsonValue* value);
void print_json_value(const JsonValue* value, const int indent, const bool color_enabled);

#endif