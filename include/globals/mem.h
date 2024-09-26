#pragma once

#include "../cursor.hpp"
#include "../buffer.hpp"
#include "mode.h"
#include "status.h"

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
extern char* pointed_file;
extern Status status;
extern int visual_start_row;
extern int visual_start_col;
extern int visual_end_row;
extern int visual_end_col;
extern std::string copy_paste_buffer;
