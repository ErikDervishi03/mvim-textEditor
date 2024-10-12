#pragma once

// Include necessary headers
#include "action.hpp"
#include "globals/mem.h"
#include "globals/mode.h"
#include "utils.h"
#include "screen.hpp"
#include "command.hpp"
#include <cstdlib>
#include <cstring>
#include <ncurses.h>
#include <chrono>  // For high-resolution timing
#include <iostream> // For printing output


// Ide class definition
class Ide {
private:
    Screen& screen;       // Screen instance
    Command _command;     // Command processor
    bool benchmark;       // Flag to indicate if benchmarking is enabled

    void draw_command_box();
    void show_initial_screen();
    void initialize_ncurses();  // Helper function to initialize ncurses and colors
    void setKeyWordColor(color pColor);
    void setCommentColor(color pColor);
    void setNumberRowsColor(color pColor);
    void setHighlightedTextColor(color pColor);
    void setBracketsColor(color pColor);
    void setDefaults();

    void startBenchmark(std::string filename);
public:
    // Constructors
    Ide();
    Ide(std::string filename, bool benchmark = false);

    // Run the IDE main loop
    void run();
};

