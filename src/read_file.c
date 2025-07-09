#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "read_file.h"

char* read_file(const char* filename) {
  FILE* fptr = fopen(filename, "r");
  if (fptr == NULL) {
    fprintf(stderr, "Error: File '%s' not found!\n", filename);
    fclose(fptr);
    return NULL;
  }

  fseek(fptr, 0, SEEK_END);
  long long length = ftell(fptr);
  rewind(fptr);

  char* buffer = (char*)malloc(length + 1);
  if (!buffer) {
    fprintf(stderr, "Error: Can't allocate memory for file's content!\n");
    fclose(fptr);
    return NULL;
  }

  fread(buffer, sizeof(char), length, fptr);
  buffer[length] = '\0';
  fclose(fptr);

  return buffer;
}

bool has_json_extension(const char* filename) {
  const char* dot = strrchr(filename, '.');
  return dot && strcmp(dot, ".json") == 0;
}

bool is_regular_file(const char* path) {
  struct stat path_stat;
  if (stat(path, &path_stat) != 0) {
    return 0;
  }
  return S_ISREG(path_stat.st_mode);
}