#pragma once

#include <cstddef>


#include <ncurses.h>
#include <iostream> // std::cerr
#include <map> // for Keymap
#include <functional> // for storing functions or lambdas
#include <fstream> // equired if you're working with file streams
#include <filesystem> // used in file.cpp
#include <stack> // used for command history
#include <set> // used for storing keywords
#include <chrono> // benchmark
#include <unistd.h> // used for access function
#include <cstring> // strchr in visual.cpp

#include "consts.h"
#include "mode.h"
#include "status.h"
#include "../cursor.hpp"
#include "../textBuffer.hpp"
#include "../errorHandler.hpp"


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

inline WINDOW* pointed_window;

inline bool is_undoing;

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