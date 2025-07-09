#ifndef ERROR_H
#define ERROR_H

#include "tokenizer.h"
#include "helper.h"

typedef struct parseError {
  char message[MESSAGE_SIZE];
  int line;
  int column;
  Token token;
} ParseError;

void set_error(ParseError* error, const char* message, const int line, const int column);
void print_error(const ParseError* error, const bool color_enabled);
void clear_error(ParseError* error);

#endif