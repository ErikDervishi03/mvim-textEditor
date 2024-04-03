#include "utils.h"

Cursor cursor;
Buffer buffer;
std::string command_buffer;
size_t max_row;
size_t max_col;
size_t pointed_row;
size_t starting_row;
Mode mode;
const char* pointed_file;