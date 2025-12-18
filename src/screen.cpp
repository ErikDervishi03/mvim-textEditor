#include "../include/screen.hpp"
#include "../include/globals/mvimResources.h"
#include "../include/bufferManager.hpp"
#include <ncurses.h>
#include <string>

Screen::~Screen()
{
}

Screen::Screen() : message_color_pair(1)
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
  draw_status_bar(); // Update status bar whenever screen updates
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

void Screen::draw_status_bar()
{
  int height, width;
  getmaxyx(stdscr, height, width);
  int row = height - 1;

  // Check if we have a valid, recent message (< 3 seconds old)
  auto now = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - message_timestamp).count();
  
  // -- ERROR MESSAGE MODE --
  if (!status_message.empty() && elapsed < 1) 
  {
    attron(COLOR_PAIR(message_color_pair)); // Use the specific color for the message
    
    // Clear the line first
    move(row, 0);
    clrtoeol(); 
    
    // Print message
    mvprintw(row, 0, "%s", status_message.c_str());
    
    // Fill the rest of the bar with space to look consistent
    if (status_message.length() < width) {
        std::string padding(width - status_message.length(), ' ');
        printw("%s", padding.c_str());
    }

    attroff(COLOR_PAIR(message_color_pair));
  } 
  // -- STANDARD STATUS BAR MODE --
  else 
  {
    // Clear old message so it doesn't reappear randomly
    if(!status_message.empty()) status_message.clear();

    // Determine Mode String
    std::string mode_str;
    switch (mode) {
      case Mode::normal: mode_str = " NORMAL "; break;
      case Mode::insert: mode_str = " INSERT "; break;
      case Mode::visual: mode_str = " VISUAL "; break;
      case Mode::command: mode_str = " COMMAND "; break;
      case Mode::find:   mode_str = " FIND "; break;
      default:           mode_str = " UNKNOWN "; break;
    }

    // Construct status string
    std::string filename = pointed_file.empty() ? "[No Name]" : pointed_file;
    std::string cursor_pos = std::to_string(pointed_row + 1) + ":" + std::to_string(pointed_col + 1);
    
    std::string status_text = mode_str + " | " + filename + " | " + cursor_pos + " / " + std::to_string(buffer.getSize()) + std::to_string(starting_row);

    // Pad the rest of the line with spaces
    if (status_text.length() < width) {
      status_text.append(width - status_text.length(), ' ');
    }

    // Draw on stdscr (background window)
    attron(A_REVERSE); // Invert colors for status bar
    mvprintw(row, 0, "%s", status_text.c_str());
    attroff(A_REVERSE);
  }

  refresh(); // Refresh stdscr to render the bar
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
  const std::vector<std::string>& buffer,
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


void Screen::refresh_all_buffers() {
    // Ottieni tutte le finestre dei buffer dal BufferManager
    const auto& windows = BufferManager::instance().get_bufferWindows();
    for (const auto& pair : windows) {
        WINDOW* window = pair.second;
        
        // Evita di stampare nella finestra 'pointed_window'
        if (window == pointed_window) continue;

        // Pulisci ciascuna finestra
        werase(window);

        // Ottieni il buffer associato alla finestra corrente
        BufferManager::BufferStructure* buffer = BufferManager::instance().get_buffer_by_name(pair.first);
        if (buffer != nullptr) {
            // Usa la funzione print_buffer per stampare il contenuto del buffer
            print_buffer(
                buffer->tBuffer.get_buffer(),  // Contenuto del buffer
                window,                        // La finestra da stampare
                buffer->starting_row,          // Riga iniziale
                buffer->starting_col,          // Colonna iniziale
                buffer->max_col                // Numero massimo di colonne visibili
            );
        }

        // Rinfresca la finestra per aggiornare il contenuto
        wrefresh(window);
    }
}

void Screen::set_status_message(const std::string& msg, int color_pair)
{
    status_message = msg;
    message_color_pair = color_pair;
    message_timestamp = std::chrono::steady_clock::now();
    
    // Force an immediate update of the status bar so the user sees the error instantly
    draw_status_bar();
}