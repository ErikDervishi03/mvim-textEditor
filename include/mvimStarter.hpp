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


// mvimStarter class definition
class mvimStarter {
private:
    struct colorSchema{
        color keyWord;
        color numberRows;
        color comments;
        color highlightedText;
        color brackets;
        color preprocessorColor;
    }; 

    Screen& screen;       // Screen instance
    Command _command;     // Command processor
    bool benchmark;       // Flag to indicate if benchmarking is enabled

    void homeScreen();
    void initialize_ncurses();  // Helper function to initialize ncurses and colors
    void setKeyWordColor(color pColor);
    void setCommentColor(color pColor);
    void setNumberRowsColor(color pColor);
    void setHighlightedTextColor(color pColor);
    void setBracketsColor(color pColor);
    void setPreprocessorColor(color pColor);
    void setDefaults();
    void setColorSchema(colorSchema pColorSchema);

    void startBenchmark(std::string filename);
public:
    // Constructors
    mvimStarter();
    mvimStarter(std::string filename, bool benchmark = false);

    // Run the mvimStarter main loop
    void run();
};

