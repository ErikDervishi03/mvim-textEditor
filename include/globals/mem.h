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

// Find mode
extern std::vector<std::pair<int, int>> found_occurrences;  // Store (row, col) of each found occurrence
extern int current_occurrence_index;  // Track which occurrence is currently highlighted
extern int current_searched_word_length;  // Length of the word being searched