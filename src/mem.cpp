#include "../include/utils.h"

Cursor cursor;
Buffer buffer;
std::string command_buffer;
size_t max_row;
size_t max_col;
size_t pointed_row;
size_t starting_row;
Mode mode;
std::string pointed_file;
Status status;
int visual_start_row;
int visual_start_col;
int visual_end_row;
int visual_end_col;
std::string copy_paste_buffer;

color keyWordColor;
color numberRowsColor;
color commentsColor;
color highlightedTextColor;
color bracketsColor;
color preprocessorColor;
color bgColor;
color highlightedBgColor;
color textColor;
color cursorColor;