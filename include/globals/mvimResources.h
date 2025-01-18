#pragma once

#include "../utils.h"
#include <cstddef>
#include <ncurses.h>

/*

  mem.h

  All file can access the resources of the text editor through this file.

*/
/*system variables*/
inline Cursor cursor;
inline textBuffer buffer;
inline Mode mode;
inline Status status;

inline size_t max_row;
inline size_t max_col;
inline size_t pointed_row;
inline size_t starting_row;
inline size_t pointed_col;
inline size_t starting_col;

inline std::string pointed_file;
inline std::string command_buffer;
inline std::string copy_paste_buffer;

inline int visual_start_row;
inline int visual_start_col;
inline int visual_end_row;
inline int visual_end_col;

inline WINDOW* pointed_window;

/*mvim colors*/
typedef short color;
inline color keyWordColor;
inline color numberRowsColor;
inline color commentsColor;
inline color highlightedTextColor;
inline color bracketsColor;
inline color preprocessorColor;
inline color bgColor;
inline color highlightedBgColor;
inline color textColor;
inline color cursorColor;

/*
   need to map keys for the specific mode
 */
typedef std::function<void ()> _action;
typedef std::map<int, _action> keymap;