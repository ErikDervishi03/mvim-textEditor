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

// Define constants and global variables
const char * ide_name = R"(
                      _             
                     (_)            
  _ __ ___   __   __  _   _ __ ___  
 | '_ ` _ \  \ \ / / | | | '_ ` _ \ 
 | | | | | |  \ V /  | | | | | | | |
 |_| |_| |_|   \_/   |_| |_| |_| |_|
                                    
        press a key to start
)";

// Ide class definition
class Ide {
private:
    Screen& screen;       // Screen instance
    Command _command;     // Command processor
    bool benchmark;       // Flag to indicate if benchmarking is enabled

    // Private methods
    void draw_command_box();
    void show_initial_screen();
    void initialize_ncurses();  // Helper function to initialize ncurses and colors

public:
    // Constructors
    Ide();
    Ide(std::string filename, bool benchmark = false);

    // Run the IDE main loop
    void run();
};

// Constructor implementations
Ide::Ide() : screen(Screen::getScreen()), benchmark(false) {
    initialize_ncurses();  // Initialize ncurses and colors
    pointed_file = "";  
    status = Status::saved;
    visual_start_row = visual_end_row = pointed_row;
    visual_start_col = visual_end_col = cursor.getX();
}

Ide::Ide(std::string filename, bool benchmark) 
    : screen(Screen::getScreen()), benchmark(benchmark) {
    if (benchmark) {
        auto start_time = std::chrono::high_resolution_clock::now();  // Start timing

        cursor.restore(span);  // Restore cursor position
        action::file::read(filename);  // Load file content
        screen.update();  // Update screen
        action::visual::highlight_keywords();  // Highlight keywords
        auto end_time = std::chrono::high_resolution_clock::now();  // End timing
        std::chrono::duration<double, std::milli> load_time = end_time - start_time;  // Get load time in milliseconds

        std::cout << "Time taken to load the file: " << load_time.count() << " ms" << std::endl;
        std::cout << "Benchmarking mode: Exiting IDE after loading." << std::endl;
        exit(0);  // Exit the program after showing benchmark results
    } else {
        initialize_ncurses();  // Initialize ncurses and colors

        pointed_file = filename;
        cursor.restore(span);  // Restore cursor position
        action::file::read(filename);  // Load file content
        screen.update();  // Update screen
        action::visual::highlight_keywords();  // Highlight keywords

        refresh();
        status = Status::saved;
        visual_start_row = visual_end_row = pointed_row;
        visual_start_col = visual_end_col = cursor.getX();
    }
}

// Helper function to initialize ncurses and color pairs
void Ide::initialize_ncurses() {
    screen.start();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);

    // Allocate memory for buffers
    copy_paste_buffer = (char *)malloc(10000);
    pointed_file = (char *)malloc(10000);
}

// Run the IDE main loop
void Ide::run() {
    show_initial_screen();  // Show initial screen if no file is loaded

    cursor.restore(span);
    while (true) {
        int input = getch();
        if (input != ERR) {
            erase();  // Clear the screen

            // Execute command based on input (modify the buffer if necessary)
            _command.execute(input);

            // Print the updated buffer
            screen.update();

            // Highlight keywords after printing the buffer
            action::visual::highlight_keywords();

            // If in visual mode, highlight the selection
            if (mode == visual) {
                visual_end_row = pointed_row;
                visual_end_col = cursor.getX() + span + 1;
                action::visual::highlight_selected();  // Highlight selection in visual mode
            } else {
                visual_start_row = pointed_row;
                visual_start_col = cursor.getX() + span + 1;
            }

            // If in find mode, highlight found occurrences
            if (mode == find) {
                action::find::highlight_visible_occurrences();  // Highlight found occurrences
            }

            // Restore cursor position after all operations
            cursor.restore(span);

            // Refresh screen to display results
            refresh();
        }
    }
}

// Draw command box at the bottom of the screen
void Ide::draw_command_box() {
    screen.draw_rectangle(screen.get_height() - 3, 0, screen.get_height() - 1, screen.get_width() - 1);
}

// Show the initial welcome screen
void Ide::show_initial_screen() {
    if (pointed_file.empty()) {
        curs_set(0);

        // Calculate dimensions for centering the text
        int text_width = 40;  // Width of the text block
        int text_height = 9;  // Height of the text block
        int screen_width = screen.get_width();
        int screen_height = screen.get_height();
        int start_x = (screen_width - text_width) / 2;
        int start_y = (screen_height - text_height) / 2;

        // Print the welcome message
        screen.print_multiline_string(start_y, start_x, ide_name);

        getch();  // Wait for user input
        screen.update();  // Update screen
        curs_set(1);  // Restore cursor visibility
    }
}

// Utility function to print a message directly to the terminal (outside of ncurses mode)
void print_to_terminal(int message) {
    endwin();  // End ncurses mode
    std::cout << message << std::endl;  // Print message to terminal
    refresh();  // Re-enter ncurses mode
}
