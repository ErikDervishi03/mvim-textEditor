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
  erase();
  print_buffer();
}

void Screen::refresh_row(int row){
  clear_line(row);
  print_row(row);
}

void Screen::clear_line(int row){
  move(row - starting_row, span + 1);
  clrtoeol();
}

void Screen::show_info(){
  mvprintw(0, max_col - 100, "                                                                      ");
  mvprintw(0, max_col - 100,"pointed row : %d, size: %d, starting_row : %d, xcur : %d, ycur: %d", 
          pointed_row, buffer.get_number_rows(), starting_row, cursor.getX(), cursor.getY());
}

void Screen::print_buffer() {
  std::vector<std::string> buffer_ = buffer.get_buffer();
  for (int i = 0 ; (i + starting_row) < buffer_.size() && i < max_row; i++) { 
    mvprintw(i, 0, "%d",i + starting_row + 1);
    mvprintw(i, span + 1, buffer_[i + starting_row].c_str());
  }
}

void Screen::print_row(int row){
  mvprintw(row - starting_row, span + 1, buffer.get_string_row(row).c_str());
}