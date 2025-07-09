#ifndef READ_FILE_H
#define READ_FILE_H

char* read_file(const char* filename);
bool has_json_extension(const char* filename);
bool is_regular_file(const char* path);

#endif