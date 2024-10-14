#pragma once

#include "../cursor.hpp"
#include "../buffer.hpp"
#include "mode.h"
#include "status.h"
#include <memory>

/*

  mem.h

  All file can access the resources of the text editor through this file.

*/

/*system variables*/
extern Cursor cursor;
extern Buffer buffer;
extern std::string command_buffer;
extern size_t max_row;
extern size_t max_col;
extern size_t pointed_row;
extern size_t starting_row;
extern Mode mode;
extern std::string pointed_file;
extern Status status;
extern int visual_start_row;
extern int visual_start_col;
extern int visual_end_row;
extern int visual_end_col;
extern std::string copy_paste_buffer;

/*mvim colors*/
typedef short color;
extern color keyWordColor;
extern color numberRowsColor;
extern color commentsColor;
extern color highlightedTextColor;
extern color bracketsColor;
extern color preprocessorColor;
extern color bgColor;
extern color highlightedBgColor;
extern color textColor;
extern color cursorColor;