#include "token_type.h"

const char* token_type_to_string(TokenType type) {
  switch (type) {
    case TOKEN_LBRACE: return "LBRACE";
    case TOKEN_RBRACE: return "RBRACE";
    case TOKEN_LBRACKET: return "LBRACKET";
    case TOKEN_RBRACKET: return "RBRACKET";
    case TOKEN_COLON: return "COLON";
    case TOKEN_COMMA: return "COMMA";
    case TOKEN_STRING: return "STRING";
    case TOKEN_NUMBER: return "NUMBER";
    case TOKEN_TRUE: return "TRUE";
    case TOKEN_FALSE: return "FALSE";
    case TOKEN_NULL: return "NULL";
    case TOKEN_EOF: return "EOF";
    case TOKEN_INVALID: return "INVALID";
    case TOKEN_PERIOD: return "PERIOD";
    case TOKEN_INVALID_LEADING_ZEROES: return "INVALID_LEADING_ZEROES";
    case TOKEN_INVALID_ESCAPE: return "INVALID_ESCAPE_CHARACTERS";
    case TOKEN_INVALID_HEX: return "INVALID_HEX";
    case TOKEN_INVALID_CONTROL_CHARACTERS: return "INVALID_CONTROL_CHARACTERS";
    case TOKEN_INVALID_UNEXPECTED_END_OF_NUMBER: return "TOKEN_INVALID_UNEXPECTED_END_OF_NUMBER";
    default: return "UNKNOWN";
  }
}