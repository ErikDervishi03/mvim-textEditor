#pragma once

#include <menu.h>
#include "globals/mem.h"
#include "globals/consts.h"
#include "globals/keywords.h"
#include <ncurses.h>
#include <memory>
#include <string.h>
#include <iostream>
#include <map>
#include <functional>
#include <fstream>
#include <filesystem>
#include <form.h>
#include <algorithm>
#include <fcntl.h> 
#include <stack>
#include <set>
#include <chrono>

#define nColor 8
#define get_pair(bg, fr) ((nColor * bg) + fr)
#define get_pair_default(fr) (get_pair(bgColor,fr))