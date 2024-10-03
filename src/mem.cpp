#include "../include/utils.h"

Cursor cursor;
Buffer buffer;
std::string command_buffer;
size_t max_row;
size_t max_col;
size_t pointed_row;
size_t starting_row;
Mode mode;
char* pointed_file;
Status status;
int visual_start_row;
int visual_start_col;
int visual_end_row;
int visual_end_col;
std::string copy_paste_buffer;
std::vector<std::pair<int, int>> found_occurrences;  // Store (row, col) of each found occurrence
int current_occurrence_index;  // Track which occurrence is currently highlighted
int current_searched_word_length; 