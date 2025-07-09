#include <stdlib.h>
#include <string.h>
#include "helper.h"

char* strndup(const char* s, size_t n) {
  char *p;
  size_t n1;

  for (n1 = 0; n1 < n && s[n1] != '\0'; ++n1) {
    continue;
  }

  p = malloc(n + 1);
  if (p != NULL) {
    memcpy(p, s, n1);
    p[n1] = '\0';
  }
  
  return p;
}

void clear() {
  #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    system("clear");
  #endif

  #if defined(_WIN32) || defined(_WIN64)
    system("cls");
  #endif
}