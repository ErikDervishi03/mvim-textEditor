#include "../include/screen.hpp"
#include <ncurses.h>

Screen::~Screen()
{
}

Screen::Screen()
{
}

void Screen::start()
{
  initscr();
  keypad(stdscr, TRUE);
  noecho();

  update();
}

void Screen::update()
{
  print_buffer();
}

int Screen::get_width()
{
  return getmaxx(pointed_window);
}

int Screen::get_height()
{
  return getmaxy(pointed_window);
}

void Screen::draw_rectangle(int y1, int x1, int y2, int x2)
{
  mvhline(y1, x1, 0, x2 - x1);
  mvhline(y2, x1, 0, x2 - x1);
  mvvline(y1, x1, 0, y2 - y1);
  mvvline(y1, x2, 0, y2 - y1);
  mvaddch(y1, x1, ACS_ULCORNER);
  mvaddch(y2, x1, ACS_LLCORNER);
  mvaddch(y1, x2, ACS_URCORNER);
  mvaddch(y2, x2, ACS_LRCORNER);
}


void Screen::print_buffer()
{
  for (int i = 0; (i + starting_row) < buffer.getSize() && i < max_row; i++)
  {
    wattron(pointed_window, COLOR_PAIR(numberRowsColor));
    mvwprintw(pointed_window, i, 0, "%zu", i + starting_row + 1);
    wattroff(pointed_window, COLOR_PAIR(numberRowsColor));
    
    std::string curr_row = buffer[i + starting_row];

    // if curr_row.length() <= starting_col the string is not visible
    if(curr_row.length() > starting_col){ 
      std::string row2print = curr_row.substr(starting_col, max_col);
      wattron(pointed_window, COLOR_PAIR(textColor));
      mvwprintw(pointed_window, i, span + 1, "%s", row2print.c_str());
      wattroff(pointed_window, COLOR_PAIR(textColor));
    } 
  }
}

void Screen::print_buffer(WINDOW* window)
{
  for (int i = 0; (i + starting_row) < buffer.getSize() && i < max_row; i++)
  {
    wattron(window, COLOR_PAIR(numberRowsColor));
    mvwprintw(window, i, 0, "%zu", i + starting_row + 1);
    wattroff(window, COLOR_PAIR(numberRowsColor));
    
    std::string curr_row = buffer[i + starting_row];

    // if curr_row.length() <= starting_col the string is not visible
    if(curr_row.length() > starting_col){ 
      std::string row2print = curr_row.substr(starting_col, max_col);
      wattron(window, COLOR_PAIR(textColor));
      mvwprintw(window, i, span + 1, "%s", row2print.c_str());
      wattroff(window, COLOR_PAIR(textColor));
    } 
  }
}

void Screen::print_multiline_string(int start_y, int start_x, const char* str)
{
  int current_y = start_y;    // Coordinata Y attuale per la stampa
  int current_x = start_x;    // Coordinata X attuale per la stampa
  wattron(pointed_window, COLOR_PAIR(textColor));
  // Itera la stringa carattere per carattere
  for (int i = 0; str[i] != '\0'; i++)
  {
    if (str[i] == '\n')
    {
      current_y++;             // Se è un newline, spostati alla riga successiva
      current_x = start_x;       // Torna alla colonna iniziale
    }
    else
    {
      mvwaddch(pointed_window, current_y, current_x++, str[i]);       // Stampa il carattere e incrementa X
    }
  }
  wattroff(pointed_window, COLOR_PAIR(textColor));
}

void Screen::print_buffer(
  std::vector<std::string> buffer,
  WINDOW* window,
  size_t starting_row,
  size_t starting_col,
  size_t max_col
  )
{

    for (int i = 0; (i + starting_row) < buffer.size() && i < max_row; i++)
    {
        wattron(window, COLOR_PAIR(numberRowsColor));
        mvwprintw(window, i, 0, "%zu", i + starting_row + 1);
        wattroff(window, COLOR_PAIR(numberRowsColor));

        std::string curr_row = buffer[i + starting_row];

        // Se la riga corrente è troppo corta, non è visibile
        if(curr_row.length() > starting_col) { 
            std::string row2print = curr_row.substr(starting_col, max_col);
            wattron(window, COLOR_PAIR(textColor));
            mvwprintw(window, i, span + 1, "%s", row2print.c_str());
            wattroff(window, COLOR_PAIR(textColor));
        }
    }
}



