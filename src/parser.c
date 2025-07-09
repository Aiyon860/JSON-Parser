#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "json.h"

#define BUFFER_SIZE 128

JsonValue* parse_json_value(ParserState* state, ParseError* error) {
  Token token = parser_peek(state);

  if (token.type == TOKEN_EOF) {
    set_error(error, "Empty input - expected a JSON value", token.line, token.column);
    return NULL;
  }

  switch (token.type) {
    case TOKEN_NULL: {
      return parse_null(state, error);
    }

    case TOKEN_TRUE:  
    case TOKEN_FALSE: {
      return parse_bool(state, &token, error);
    }
    
    case TOKEN_NUMBER: {
      return parse_number(state, &token, error);
    }

    case TOKEN_STRING: {
      return parse_string(state, &token, error);
    }

    case TOKEN_RBRACE:
    case TOKEN_LBRACE: {
      return parse_object(state, error);
    }

    case TOKEN_RBRACKET:
    case TOKEN_LBRACKET: {
      return parse_array(state, error);
    }

    default: {
      switch (token.type) {
        case TOKEN_INVALID_LEADING_ZEROES: {
          set_error(error, "Numbers cannot have leading zeroes", token.line, token.column);
          break;
        }

        case TOKEN_INVALID_HEX: {
          set_error(error, "Numbers cannot be hex", token.line, token.column);
          break;
        }

        case TOKEN_INVALID_ESCAPE: {
          set_error(error, "Invalid escape sequence", token.line, token.column);
          break;
        }

        case TOKEN_INVALID_CONTROL_CHARACTERS: {
          set_error(error, "Control characters must be escaped", token.line, token.column);
          break;
        }

        case TOKEN_INVALID_UNEXPECTED_END_OF_NUMBER: {
          set_error(error, "Unexpected end of number.", token.line, token.column);
          break;
        }

        default: {
          set_error(error, "Value expected", token.line, token.column);
          break;
        }
      }

      return NULL;
    }
  }

  return NULL;
}

JsonValue* parse_null(ParserState* state, ParseError* error) {
  parser_advance(state);
  JsonValue* null_value = malloc(sizeof(JsonValue));
  if (null_value == NULL) {
    fprintf(stderr, "Error: Can't allocate memory for JsonValue when parsing 'null'!\n");
    return NULL;
  }

  null_value->type = JSON_NULL;
  return null_value;
}

JsonValue* parse_bool(ParserState* state, Token* token, ParseError* error) {
  parser_advance(state);
  JsonValue* bool_value = malloc(sizeof(JsonValue));
  if (bool_value == NULL) {
    fprintf(stderr, "Error: Can't allocate memory for JsonValue when parsing '%s'!\n", token->type == TOKEN_TRUE ? "true" : "false");
    return NULL;
  }

  bool_value->type = JSON_BOOL;
  bool_value->boolean = token->type == TOKEN_TRUE;
  return bool_value;
}

JsonValue* parse_number(ParserState* state, Token* token, ParseError* error) {
  parser_advance(state);
  JsonValue* number_value = malloc(sizeof(JsonValue));
  if (number_value == NULL) {
    fprintf(stderr, "Error: Can't allocate memory for JsonValue when parsing '%s'!\n", token->value);
    return NULL;
  }

  number_value->type = JSON_NUMBER;
  number_value->number = strdup(token->value);
  return number_value;
}

JsonValue* parse_string(ParserState* state, Token* token, ParseError* error) {
  parser_advance(state);
  JsonValue* string_value = malloc(sizeof(JsonValue));
  if (string_value == NULL) {
    fprintf(stderr, "Error: Can't allocate memory for JsonValue when parsing '%s'!\n", token->value);
    return NULL;
  }

  string_value->type = JSON_STRING;
  string_value->string = strdup(token->value);
  return string_value;
}

JsonValue* parse_object(ParserState* state, ParseError* error) {
  if (!parser_match(state, TOKEN_LBRACE)) {
    set_error(error, "Expected '{' at start of object", parser_peek(state).line, parser_peek(state).column);
    return NULL;
  }

  JsonValue* object = malloc(sizeof(JsonValue));
  if (!object) {
    fprintf(stderr, "Error: Can't allocate memory for JsonValue when parsing object!\n");
    return NULL;
  }

  object->type = JSON_OBJECT;

  JsonObject* obj = malloc(sizeof(JsonObject));
  if (!obj) {
    fprintf(stderr, "Error: Can't allocate memory for JsonObject when parsing object!\n");
    free(object);
    return NULL;
  }

  obj->pairs = NULL;
  obj->count = 0;

  object->object = obj;

  if (parser_peek(state).type == TOKEN_EOF) {
    Token eof = parser_peek(state);
    set_error(error, "Expected comma or closing brace", eof.line, eof.column);
    free_json_value(object);
    return NULL;
  }

  if (parser_peek(state).type == TOKEN_RBRACE) {
    parser_advance(state);
    return object;
  }

  while (true) {
    Token key_token = parser_peek(state);
    if (key_token.type == TOKEN_NUMBER) {
      set_error(error, "Expected string as object key", key_token.line, key_token.column);
      free_json_value(object);
      return NULL;
    }

    if (key_token.type != TOKEN_STRING) {
      set_error(error, "Property keys must be doublequoted", key_token.line, key_token.column);
      free_json_value(object);
      return NULL;
    }

    char* key = strdup(key_token.value);
    parser_advance(state);

    if (key_exists(obj, key)) {
      char message[BUFFER_SIZE];
      snprintf(message, sizeof(message), "Duplicate key \"%s\" found", key);
      set_error(error, message, key_token.line, key_token.column);
      free(key);
      free_json_value(object);
      return NULL;
    }

    if (!parser_match(state, TOKEN_COLON)) {
      set_error(error, "Expected ':' after object key", parser_peek(state).line, parser_peek(state).column);
      free(key);
      free_json_value(object);
      return NULL;
    }

    JsonValue* value = parse_json_value(state, error);
    if (!value) {
      free(key);
      free_json_value(object);
      return NULL;
    }

    JsonPair* pair = malloc(sizeof(JsonPair));
    if (pair == NULL) {
      fprintf(stderr, "Error: Can't allocate memory for JsonPair!\n");
      return NULL;
    }

    pair->key = key;
    pair->value = value;

    // Expand object.pairs
    object->object->pairs = realloc(object->object->pairs, sizeof(JsonPair*) * (object->object->count + 1));
    object->object->pairs[object->object->count] = pair;
    object->object->count += 1;

    Token next = parser_peek(state);
    if (next.type == TOKEN_COMMA) {
      parser_advance(state);

      Token after_comma = parser_peek(state);
      if (after_comma.type == TOKEN_RBRACE) {
        set_error(error, "Trailing comma", after_comma.line, after_comma.column);
        free_json_value(object);
        return NULL;
      }

      if (after_comma.type != TOKEN_STRING && after_comma.type == TOKEN_EOF) {
        set_error(error, "Property expected", after_comma.line, after_comma.column);
        free_json_value(object);
        return NULL;
      }

      continue;
    } else if (next.type == TOKEN_RBRACE) {
      parser_advance(state);
      break;
    } else {
      set_error(error, "Expected ',' or '}' in object", next.line, next.column);
      free_json_value(object);
      return NULL;
    }
  }

  return object;
}

bool parser_match(ParserState* state, const TokenType expected) {
  if (parser_peek(state).type == expected) {
    parser_advance(state);
    return true;
  }
  return false;
}

JsonValue* parse_array(ParserState* state, ParseError* error) {
  if (!parser_match(state, TOKEN_LBRACKET)) {
    set_error(error, "Expected '[' at start of array", parser_peek(state).line, parser_peek(state).column);
    return NULL;
  }

  JsonValue* array = malloc(sizeof(JsonValue));
  if (array == NULL) {
    fprintf(stderr, "Error: Can't allocate memory for JsonValue!\n");
    return NULL;
  }

  array->type = JSON_ARRAY;

  JsonArray* arr = malloc(sizeof(JsonArray));
  if (!arr) {
    fprintf(stderr, "Error: Can't allocate memory for JsonArray when parsing object!\n");
    free(array);
    return NULL;
  }

  arr->elements = NULL;
  arr->count = 0;

  array->array = arr;

  if (parser_peek(state).type == TOKEN_RBRACKET) {
    parser_advance(state);
    return array;
  }

  while (true) {
    JsonValue* element = parse_json_value(state, error);
    if (!element) {
      free_json_value(array);
      return NULL;
    }

    // Expand array
    array->array->elements = realloc(array->array->elements, sizeof(JsonValue*) * (array->array->count + 1));
    array->array->elements[array->array->count] = element;
    array->array->count += 1;

    Token next = parser_peek(state);
    if (next.type == TOKEN_COMMA) {
      parser_advance(state);

      Token after_comma = parser_peek(state);
      if (after_comma.type == TOKEN_RBRACKET) {
        set_error(error, "Trailing comma", after_comma.line, after_comma.column);
        free_json_value(array);
        return NULL;
      }

      continue;
    } else if (next.type == TOKEN_RBRACKET) {
      parser_advance(state);
      break;
    } else {
      set_error(error, "Expected ',' or ']' in array", next.line, next.column);
      free_json_value(array);
      return NULL;
    }
  }

  return array;
}

Token parser_peek(ParserState* state) {
  return state->tokens[state->current_index];
}

void parser_advance(ParserState* state) {
  if (state->tokens[state->current_index].type != TOKEN_EOF) {
    state->current_index += 1;
  }
}

bool key_exists(JsonObject* object, const char* key) {
  for (int i = 0; i < object->count; ++i) {
    if (strcmp(object->pairs[i]->key, key) == 0) {
      return true;
    }
  }
  return false;
}