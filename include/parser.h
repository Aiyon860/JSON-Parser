#ifndef PARSER_H
#define PARSER_H

#include "error.h"

typedef struct JsonValue JsonValue;
typedef struct JsonObject JsonObject;

typedef struct parserState {
  Token* tokens;
  int current_index;
} ParserState;

JsonValue* parse_json_value(ParserState* state, ParseError* error);
JsonValue* parse_null(ParserState* state, ParseError* error);
JsonValue* parse_bool(ParserState* state, Token* token, ParseError* error);
JsonValue* parse_number(ParserState* state, Token* token, ParseError* error);
JsonValue* parse_string(ParserState* state, Token* token, ParseError* error);
JsonValue* parse_object(ParserState* state, ParseError* error);
JsonValue* parse_array(ParserState* state, ParseError* error);
bool parser_match(ParserState*, const TokenType expected);
Token parser_peek(ParserState*);
void parser_advance(ParserState*);
bool key_exists(JsonObject* object, const char* key);

#endif