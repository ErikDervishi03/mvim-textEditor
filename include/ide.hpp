#pragma once
#include "action.hpp"
#include "globals/mem.h"
#include "globals/mode.h"
#include "utils.h"
#include "screen.hpp"
#include "command.hpp"
#include <cstdlib>
#include <cstring>
#include <ncurses.h>


const char * ide_name = R"(
                      _             
                     (_)            
  _ __ ___   __   __  _   _ __ ___  
 | '_ ` _ \  \ \ / / | | | '_ ` _ \ 
 | | | | | |  \ V /  | | | | | | | |
 |_| |_| |_|   \_/   |_| |_| |_| |_|
                                    
        press a key to start
)";


class Ide{
  private:
    Screen& screen;
    Command _command;
    void draw_command_box();
    void show_initial_screen();
  public:
    Ide();
    Ide(const char* filename);
    void run();
};


Ide::Ide() : screen(Screen::getScreen()) {
  screen.start();
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_RED);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);

  copy_paste_buffer = (char *)malloc(10000);
  pointed_file = (char *)malloc(10000);
  strcpy(pointed_file, "");  
  status = Status::saved;
  visual_start_row = visual_end_row = pointed_row;
  visual_start_col = visual_end_col = cursor.getX();
}

Ide::Ide(const char* filename) : screen(Screen::getScreen()) {
  screen.start();
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_RED);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);

  copy_paste_buffer = (char *)malloc(10000);
  pointed_file = (char *)malloc(10000);
  strcpy(pointed_file, "");

  cursor.restore(span);
  action::file::read(filename);
  screen.update();
  action::visual::highlight_keywords();

  refresh();
  status = Status::saved;

  visual_start_row = visual_end_row = pointed_row;
  visual_start_col = visual_end_col = cursor.getX();

}


void print_to_terminal(int message) {
    // Esci dalla modalità ncurses
    endwin();

    // Usa std::cout o printf per stampare direttamente nel terminale
    std::cout << message << std::endl;

    // Rientra nella modalità ncurses
    refresh();
}


void Ide::run(){ 
  show_initial_screen();

  //draw_command_box();
  cursor.restore(span);
  while(true){
    int input = getch();
    if(input != ERR){ 
      erase();
      _command.execute(input);
      screen.update();
      action::visual::highlight_keywords();

      if(mode != visual){
        visual_start_row = pointed_row;
        visual_start_col = cursor.getX() + span + 1;
      }else{
        visual_end_row = pointed_row;
        visual_end_col = cursor.getX() + span + 1;
        action::visual::highlight_selected();
      }
      
      if(mode == find){
        action::find::highlight_visible_occurrences();
      }

      cursor.restore(span);
      refresh();
    }
  }
  
}

void Ide::draw_command_box(){
  screen.draw_rectangle(screen.get_height() - 3, 0, screen.get_height() - 1, screen.get_width()-1);
}

void Ide::show_initial_screen() {
    if (strcmp(pointed_file, "") == 0) {
        curs_set(0);

        // Calculate the dimensions of the text
        int text_width = 40;  // Width of the text block (longest line)
        int text_height = 9;  // Height of the text block (number of lines)

        // Get the screen size
        int screen_width = screen.get_width();
        int screen_height = screen.get_height();

        // Calculate the starting position for centering the text
        int start_x = (screen_width - text_width) / 2;
        int start_y = (screen_height - text_height) / 2;

        // Print the multiline string centered
        screen.print_multiline_string(start_y, start_x, ide_name);

        getch();  // Wait for user input
        screen.update();  // Update the screen
        curs_set(1);  // Restore cursor visibility
    }
}


