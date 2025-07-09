#include <stdio.h>
#include <string.h>
#include "json.h"

// ANSI color codes
#define RESET   "\033[0m"
#define BG_RED  "\033[41m"
#define WHITE   "\033[97m"

void set_error(ParseError* error, const char* message, const int line, const int column) {
  if (!error) {
    return;
  }

  strncpy(error->message, message, sizeof(error->message) - 1);
  error->message[sizeof(error->message) - 1] = '\0';
  error->line = line;
  error->column = column;
}

void print_error(const ParseError* error, const bool color_enabled) {
  if (!error) {
    return;
  }

  if (color_enabled) {
    fprintf(stderr, "%s%sError: %s (line %d, column %d)%s\n", 
      BG_RED, WHITE, error->message, error->line, error->column, RESET);
  } else {
    fprintf(stderr, "Error: %s (line %d, column %d)\n", 
      error->message, error->line, error->column);
  }
}

void clear_error(ParseError* error) {
  if (!error) {
    return;
  }

  error->message[0] = '\0';
  error->line = 0;
  error->column = 0;
}