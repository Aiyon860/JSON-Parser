#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdbool.h>
#include "token_type.h"

typedef struct token {
  TokenType type;
  char* value;
  int line;
  int column;
} Token;

typedef struct tokenizerState {
  const char* input;
  int current_index;
  int line;
  int column;
} TokenizerState;

#include "error.h"

Token* tokenize(const char* input, int* token_count);
Token next_token(TokenizerState* state);
char peek(TokenizerState*);
char advance(TokenizerState*);
Token make_token(TokenType type, const char* value, const int line, const int column);
TokenizerState init_tokenizer(const char* input);
void free_tokens(Token* tokens, const int count);
void print_token(Token token, const int index, const bool color_enabled);
bool match_keyword(TokenizerState* state, const char* keyword);

#endif 