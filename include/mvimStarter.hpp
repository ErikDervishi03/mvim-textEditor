#pragma once

// Include necessary headers
#include "colorManager.hpp"
#include "service.hpp"
#include "screen.hpp"
#include "command.hpp"


// mvimStarter class definition
class mvimStarter {
private:
    Screen& screen;       // Screen instance
    Command _command;     // Command processor
    service mvimService;
    ColorManager mvimColorManager;
    bool benchmark;       // Flag to indicate if benchmarking is enabled

    void homeScreen();
    void initialize_ncurses();  // Helper function to initialize ncurses and colors
    void setDefaults();


    void updateVar();

    void startBenchmark(std::string filename);

public:
    // Constructors
    mvimStarter();
    mvimStarter(std::string filename, bool benchmark = false);

    // Run the mvimStarter main loop
    void run();
};

