#include "screen.hpp"

Screen::~Screen()
{
}
Screen::Screen()
{
}

void Screen::start(){
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  getmaxyx(stdscr, max_row, max_col);
  starting_row = 0;
  pointed_row = 0;
  cursor.set(0,0);

  update();
}

void Screen::update(){
  /*
  if(mode == command){
    erase();
    print_command();
    print_buffer();
  }*/

  erase();
  print_buffer();
  
}

void  Screen::print_command(){
  mvprintw(get_height() - 2 , 2 , command_buffer.c_str());
}

int Screen::get_width(){
  return getmaxx(stdscr);
}

int Screen::get_height(){
  return getmaxy(stdscr);
}

void Screen::draw_rectangle(int y1, int x1, int y2, int x2){
    mvhline(y1, x1, 0, x2-x1);
    mvhline(y2, x1, 0, x2-x1);
    mvvline(y1, x1, 0, y2-y1);
    mvvline(y1, x2, 0, y2-y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}


void Screen::print_buffer() {
  std::vector<std::string> buffer_ = buffer.get_buffer();
  for (int i = 0 ; (i + starting_row) < buffer_.size() && i < max_row; i++) { 
    mvprintw(i, 0, "%d",i + starting_row + 1);
    mvprintw(i, span + 1, buffer_[i + starting_row].c_str());
  }
}
