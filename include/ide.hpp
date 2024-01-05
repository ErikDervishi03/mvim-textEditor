#pragma once
#include <string>
#include "cursor.hpp"
#include "buffer.hpp"
#include <ncurses.h>
#include <iostream>
#include <fstream>

class Ide{
  private:
    Cursor cursor;
    Buffer buffer;
    int pointed_row;
    int starting_row;
    int starting_col;
    const int tab_size = 2;
    int max_row, max_col;
    const int span = 4;// distance of the text from the left side of the screen
                       //
    void update_ide_info();
    void handle_input(int input);
    void restore_cursor();
    void show_info();
    void scroll_down();
    void scroll_up();
    void scroll_left();
    void scroll_right();
    void clear_line(int row);
    void refresh_row(int row);
    void update_screen();
    void print_row(int row);
    void print_buffer();
    void init_index_numbers_column();
  public:
    Ide();
    void open(); 
    void init_screen();
    void write_buffer_into_file(const char* file_name);
    void read_file(const char* file_name);
   };
