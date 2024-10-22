#pragma once


#include "globals/consts.h"
#include "globals/keywords.h"
#include "cursor.hpp"
#include "textBuffer.hpp"
#include "globals/mode.h"
#include "globals/status.h"

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
