#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "tokenizer.h"
#include "helper.h"

// ANSI color codes
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

#define BUFFER_SIZE 128
#define INVALID_ESCAPE_SIZE 3
#define CHAR_SIZE 2
#define INIT_TOKEN_CAPACITY 64

Token* tokenize(const char* input, int* token_count) {
  TokenizerState state = init_tokenizer(input);

  int capacity = INIT_TOKEN_CAPACITY;
  int count = 0;
  Token* tokens = (Token*)malloc(capacity * sizeof(Token));

  if (!tokens) {
    printf("Error: Can't reallocate memory when creating tokens!\n");
    return NULL;
  }

  while (true) {
    Token token = next_token(&state);

    if (count >= capacity) {
      capacity *= 2;
      tokens = (Token*)realloc(tokens, capacity * sizeof(Token));

      if (!tokens) {
        printf("Error: Can't reallocate memory while increasing tokens' capacity!\n");
        return NULL;
      }
    }

    tokens[count] = token;
    count += 1;
    
    if (token.type == TOKEN_EOF) {
      break;
    }
  }

  *(token_count) = count;
  return tokens;
}

Token next_token(TokenizerState* state) {
  char c = peek(state);

  if (c == '\0') {
    return make_token(TOKEN_EOF, "", state->line, state->column);
  }

  if (isspace(c)) {
    advance(state);
    return next_token(state);
  }

  if (
    c == '{' || c == '}' || 
    c == '[' || c == ']' || 
    c == ':' || c == ',' || 
    c == '.' 
  ) {
    advance(state);
    char str[CHAR_SIZE] = { c, '\0' };
    const char* text = strdup(str);
    switch (c) {
      case '{': return make_token(TOKEN_LBRACE, text, state->line, state->column);
      case '}': return make_token(TOKEN_RBRACE, text, state->line, state->column);
      case '[': return make_token(TOKEN_LBRACKET, text, state->line, state->column);
      case ']': return make_token(TOKEN_RBRACKET, text, state->line, state->column);
      case ':': return make_token(TOKEN_COLON, text, state->line, state->column);
      case ',': return make_token(TOKEN_COMMA, text, state->line, state->column);
      case '.': return make_token(TOKEN_PERIOD, text, state->line, state->column);
    }
  } else if (c == '"') {
    advance(state);
    int start_col = state->column + 1;

    char buffer[BUFFER_SIZE];
    int buffer_index = 0;

    while (peek(state) != '"' && peek(state) != '\0') {
      char c = peek(state);

      if (c == '\\') {
        advance(state);
        char esc = peek(state);

        if (esc == '"' || esc == '\\' || esc == '/' ||
          esc == 'b' || esc == 'f' || esc == 'n' ||
          esc == 'r' || esc == 't') {
          buffer[buffer_index] = '\\';
          buffer_index += 1;
          
          buffer[buffer_index] = esc;
          buffer_index += 1;

          advance(state);
        } else if (esc == 'u') {
          buffer[buffer_index] = '\\';
          buffer_index += 1;
          
          buffer[buffer_index] = 'u';
          buffer_index += 1;

          advance(state);

          for (int i = 0; i < 4; ++i) {
            char hex = peek(state);
            if (!isxdigit(hex)) {
              return make_token(TOKEN_INVALID_ESCAPE, "\\uXXXX", state->line, state->column);
            }

            buffer[buffer_index] = hex;
            buffer_index += 1;

            advance(state);
          }
        } else {
          // Invalid escape (e.g. \x)
          char invalid[INVALID_ESCAPE_SIZE] = {'\\', esc, '\0'};
          advance(state);
          return make_token(TOKEN_INVALID_ESCAPE, strdup(invalid), state->line, state->column);
        }
      } else if (c == '\t' || (c >= 0 && c <= 0x1F)) {  // 0x1F == 31
        // Unescaped control character (tab, newline)
        char message[MESSAGE_SIZE];
        snprintf(message, 64, "INVALID_CONTROL:0x%02X", c);
        return make_token(TOKEN_INVALID_CONTROL_CHARACTERS, message, state->line, state->column);
      } else {
        buffer[buffer_index] = c;
        buffer_index += 1;
        advance(state);
      }
    }

    if (peek(state) != '"') {
      return make_token(TOKEN_INVALID, "Unterminated string", state->line, state->column);
    }

    buffer[buffer_index] = '\0';
    advance(state);
    return make_token(TOKEN_STRING, strdup(buffer), state->line, start_col);
  } else if (isdigit(c) || c == '-') {
    int start = state->current_index;
    int start_col = state->column + 1;

    if (peek(state) == '-') {
      advance(state);
      if (!isdigit(peek(state))) {
        return make_token(TOKEN_INVALID, strdup("-"), state->line, start_col);
      }
    }

    // integer part
    if (peek(state) == '0') {
      advance(state);

      if (isdigit(peek(state))) {
        return make_token(TOKEN_INVALID_LEADING_ZEROES, strdup("0X"), state->line, state->column);
      } else if (peek(state) == 'x') {
        return make_token(TOKEN_INVALID_HEX, strdup("0x"), state->line, state->column);
      }
    } else if (isdigit(peek(state))) {
      while (isdigit(peek(state))) {
        advance(state);
      }
    } else {
      // no digit after optional minus
      return make_token(TOKEN_INVALID, strdup("-X"), state->line, start_col);
    }

    // fractional part (e.g. .123)
    if (peek(state) == '.') {
      advance(state);
      if (!isdigit(peek(state))) {
        return make_token(TOKEN_INVALID_UNEXPECTED_END_OF_NUMBER, strdup(".X"), state->line, state->column);
      }
      while (isdigit(peek(state))) {
        advance(state);
      }
    } 

    // exponent part (e.g. e+5, E-2)
    if (peek(state) == 'e' || peek(state) == 'E') {
      advance(state);

      if (peek(state) == '+' || peek(state) == '-') {
        advance(state);
      }

      if (!isdigit(peek(state))) {
        return make_token(TOKEN_INVALID_UNEXPECTED_END_OF_NUMBER, strdup("eX"), state->line, state->column);
      }

      while (isdigit(peek(state))) {
        advance(state);
      }
    }

    int end = state->current_index;
    int length = end - start;
    char* number = strndup(&state->input[start], length);

    return make_token(TOKEN_NUMBER, number, state->line, start_col);
  } else if (match_keyword(state, "true")) {
    state->current_index += 4;
    state->column += 4;
    return make_token(TOKEN_TRUE, "true", state->line, state->column - 4);
  } else if (match_keyword(state, "false")) {
    state->current_index += 5;
    state->column += 5;
    return make_token(TOKEN_FALSE, "false", state->line, state->column - 5);
  } else if (match_keyword(state, "null")) {
    state->current_index += 4;
    state->column += 4;
    return make_token(TOKEN_NULL, "null", state->line, state->column - 4);
  } else {
    char str[2] = { c, '\0' };
    const char* invalid = strdup(str);
    advance(state);
    return make_token(TOKEN_INVALID, invalid, state->line, state->column);
  }
}

char peek(TokenizerState* state) {
  return state->input[state->current_index];
}

char advance(TokenizerState* state) {
  char current_char = peek(state);
  
  state->current_index += 1;
  if (current_char == '\n') {
    state->line += 1;
    state->column = 0;
  } else {
    state->column += 1;
  }

  return current_char;
}

Token make_token(TokenType type, const char* value, const int line, const int column) {
  Token token = { 
    .type = type, 
    .value = strdup(value), 
    .line = line, 
    .column = column
  };
  return token;
}

TokenizerState init_tokenizer(const char* input) {
  TokenizerState state = {
    .input = input,
    .current_index = 0,
    .line = 1,
    .column = 0,
  };
  return state;
}

bool match_keyword(TokenizerState* state, const char* keyword) {
  int len = strlen(keyword);
  return strncmp(&state->input[state->current_index], keyword, len) == 0;
}

void free_tokens(Token* tokens, const int count) {
  for (int i = 0; i < count; ++i) {
    free((void*)tokens[i].value);
  }
  free(tokens);
}

void print_token(Token token, const int index, const bool color_enabled) {
  if (color_enabled) {
    printf("%sToken #%d:%s [%sL%d%s:%sC%d%s] %s%-10s%s | Value: '%s'\n",
      CYAN, index, RESET, 
      YELLOW, token.line, RESET,
      YELLOW, token.column, RESET,
      GREEN, token_type_to_string(token.type), RESET,
      token.value
    );
  } else {
    printf("Token #%d: [L%d:C%d] %-10s | Value: '%s'\n",
      index, 
      token.line, 
      token.column, 
      token_type_to_string(token.type), 
      token.value
    );
  }

}