#pragma once
#include "utils.h"
#include "screen.hpp"
#include "command.hpp"

class Ide{
  private:
    Screen& screen;
    Command command;
  public:
    Ide();
    void run();
};


Ide::Ide() : screen(Screen::getScreen()) {
  screen.start();
}

void Ide::run(){
  while(true){
    int input = getch();
    if(input != ERR){
      command.execute(input);
      screen.update();
      cursor.restore(span);
    }
  }
  
}

