#pragma once
#include "utils.h"
#include "screen.hpp"
#include "command.hpp"

class Ide{
  private:
    Screen& screen;
    Command _command;
    void draw_command_box();
  public:
    Ide();
    Ide(const char* filename);
    void run();
};


Ide::Ide() : screen(Screen::getScreen()) {
  screen.start();
  pointed_file = "";
}

Ide::Ide(const char* filename) : screen(Screen::getScreen()) {
  screen.start();
  cursor.restore(span);
  action::file::read(filename);
  screen.update();
  pointed_file = filename;
}

void Ide::run(){ 
  draw_command_box();
  cursor.restore(span);
  while(true){
    int input = getch();
    if(input != ERR){
      _command.execute(input);
      screen.update();
      draw_command_box();
      /*if(mode == command)
        cursor.restore(0);*/
      
      cursor.restore(span);
    }
  }
  
}

void Ide::draw_command_box(){
  screen.draw_rectangle(screen.get_height() - 3, 0, screen.get_height() - 1, screen.get_width()-1);
}

