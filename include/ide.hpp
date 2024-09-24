#pragma once
#include "globals/mem.h"
#include "utils.h"
#include "screen.hpp"
#include "command.hpp"
#include <cstdlib>
#include <cstring>


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
  pointed_file = (char *)malloc(10000);
  strcpy(pointed_file, "");  
  status = Status::unsaved;
}

Ide::Ide(const char* filename) : screen(Screen::getScreen()) {
  screen.start();
  cursor.restore(span);
  action::file::read(filename);
  screen.update();
  pointed_file = (char *)malloc(10000);
  strcpy(pointed_file, filename);
  status = Status::saved;
}

void Ide::run(){ 
  show_initial_screen();

  //draw_command_box();
  cursor.restore(span);
  while(true){
    int input = getch();
    if(input != ERR){
      _command.execute(input);
      screen.update();
      //draw_command_box();
      /*if(mode == command)
        cursor.restore(0);*/
      
      cursor.restore(span);
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


