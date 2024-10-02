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

    // Private methods
    void draw_command_box();
    void show_initial_screen();
    void initialize_ncurses();  // Helper function to initialize ncurses and colors

public:
    // Constructors
    Ide();
    Ide(const char* filename);

    // Run the IDE main loop
    void run();
};

// Constructor implementations
Ide::Ide() : screen(Screen::getScreen()) {
    initialize_ncurses();  // Initialize ncurses and colors
    strcpy(pointed_file, "");  
    status = Status::saved;
    visual_start_row = visual_end_row = pointed_row;
    visual_start_col = visual_end_col = cursor.getX();
}

Ide::Ide(const char* filename) : screen(Screen::getScreen()) {
    initialize_ncurses();  // Initialize ncurses and colors
    strcpy(pointed_file, "");

    cursor.restore(span);  // Restore cursor position
    action::file::read(filename);  // Load file content
    screen.update();  // Update screen
    action::visual::highlight_keywords();  // Highlight keywords

    refresh();
    status = Status::saved;
    visual_start_row = visual_end_row = pointed_row;
    visual_start_col = visual_end_col = cursor.getX();
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
        erase();  // Pulisce lo schermo

        // Esegui il comando basato sull'input (modifica il buffer se necessario)
        _command.execute(input);

        // Stampa il buffer aggiornato
        screen.update();

        // Evidenzia le parole chiave dopo aver stampato il buffer
        action::visual::highlight_keywords();

        // Se sei in modalità visual, evidenzia la selezione
        if (mode == visual) {
            visual_end_row = pointed_row;
            visual_end_col = cursor.getX() + span + 1;
            action::visual::highlight_selected();  // Evidenzia selezione visual mode
        }else{
            visual_start_row = pointed_row;
            visual_start_col = cursor.getX() + span + 1;
        }

        // Se sei in modalità find, evidenzia le occorrenze trovate
        if (mode == find) {
            action::find::highlight_visible_occurrences();  // Evidenzia occorrenze trovate
        }

        // Ripristina la posizione del cursore dopo tutte le operazioni
        cursor.restore(span);

        // Visualizza i risultati sullo schermo
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
    if (strcmp(pointed_file, "") == 0) {
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
