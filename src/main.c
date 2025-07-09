#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "helper.h"
#include "read_file.h"
#include "parser.h"
#include "json.h"

// ANSI color codes
#define RESET     "\033[0m"
#define BG_BLUE   "\e[44m"
#define BG_WHITE  "\e[47m"
#define BLACK     "\e[1;30m"
#define WHITE     "\033[97m"

#define PATH_SIZE 512

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Usage: %s <path-to-json-file>\n", argv[0]);
    return 1;
  }

  bool color_enabled = false;

  if (argc >= 3 && strcmp(argv[2], "--color") == 0) {
    color_enabled = true;
  }

  const char* folder_path = argv[1];
  DIR* dir = opendir(folder_path);
  if (!dir) {
    printf("Error: folder '%s' not found!\n", folder_path);
    return 1;
  }

  clear();  

  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    // skip ".", "..", hidden files
    if (entry->d_name[0] == '.') {  
      continue;
    }

    char full_path[PATH_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s%s",
          folder_path,
          folder_path[strlen(folder_path) - 1] == '/' ? "" : "/",
          entry->d_name);


    if (is_regular_file(full_path) && has_json_extension(entry->d_name)) {
      if (color_enabled) {
        printf("%s%s===> Testing file: %s%s\n\n", BG_BLUE, WHITE, full_path, RESET);
      } else {
        printf("===> Testing file: %s\n\n", full_path);
      }

      char* json_text = read_file(full_path);
      if (!json_text) {
        continue;
      }

      int token_count = 0;
      Token* tokens = tokenize(json_text, &token_count);
      
      if (tokens) {
        printf("Total Tokens: %d\n", token_count);
        
        for (int i = 0; i < token_count; ++i) {
          print_token(tokens[i], i + 1, color_enabled);
        }
        
        ParserState parser_state = { .tokens = tokens, .current_index = 0 };
        ParseError error;
        JsonValue* root = parse_json_value(&parser_state, &error);

        if (root && root->type != JSON_OBJECT && root->type != JSON_ARRAY) {
          set_error(&error, "Top-level JSON must be an object or array", 1, 1);
          free_json_value(root);
          root = NULL;
        }

        if (root) {
          Token remaining = parser_peek(&parser_state);
          if (remaining.type != TOKEN_EOF) {
            set_error(&error, "End of file expected", remaining.line, remaining.column);
            free_json_value(root);
            root = NULL;
          }
        }

        if (root) {
          if (color_enabled) {
            printf("\n%s%s=> Parsed JSON AST:%s\n\n", BG_BLUE, WHITE, RESET);
          } else {
            printf("\n=> Parsed JSON AST:\n\n");
          }
          
          print_json_value(root, 0, color_enabled);
          free_json_value(root);
        } else {
          printf("\nParsing failed!\n");
          print_error(&error, color_enabled);
        }

        free_tokens(tokens, token_count);
      } else {
        printf("Tokenization Failed for path: %s\n", full_path);
      }

      free(json_text);
      printf("\n-----\n\n");
    }
  }

  return 0;
}