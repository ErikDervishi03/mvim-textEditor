#pragma once

#include "../cursor.hpp"
#include "../buffer.hpp"
#include "mode.h"

/*

  mem.h

  All file can access the resources of the text editor through this file.

*/

extern Cursor cursor;
extern Buffer buffer;
extern std::string command_buffer;
extern size_t max_row;
extern size_t max_col;
extern size_t pointed_row;
extern size_t starting_row;
extern Mode mode;
extern const char* pointed_file;

