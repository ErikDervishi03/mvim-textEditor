#include "../include/editor/system.hpp"
#include "../include/editor/state.hpp"      // Needed to clear action_history on restore
#include "../include/bufferManager.hpp"
#include "../include/globals/mvimResources.h"
#include "../include/screen.hpp"

#include <ncurses.h>
#include <string>
#include <cstdlib>   // Needed for std::_Exit()

// Function to prompt user for confirmation before exiting unsaved changes
bool editor::system::confirm_exit()
{
  return Screen::getScreen().prompt_confirm("You have unsaved changes. Exit without saving? (y/n): ");
}

std::string editor::system::text_form(const std::string& label)
{
  return Screen::getScreen().prompt_text(label);
}

// Function to exit the IDE
void editor::system::exit_ide() {
    auto& bufferManager = BufferManager::instance();

    // If the status is unsaved, prompt for confirmation
    if (status == Status::unsaved) {
        bool confirmed = editor::system::confirm_exit();
        if (!confirmed) {
            return; // If the user selects "No", return to the editor
        }
    }

    int bufferCount = bufferManager.getBufferCount();

    // Delete the currently active buffer
    bufferManager.delete_buffer();

    // If buffers remain, switch to the first one
    if (bufferCount > 1) {
        bufferManager.set_active_buffer(0); // Set the first buffer as active
        bufferManager.syncSystemVarsFromBuffer();
        wclear(pointed_window);
        wrefresh(pointed_window);
    } else {
        // If no buffers remain, exit the program
        endwin(); // End ncurses mode
        exit(0);  // Terminate the program
    }
}



void editor::system::helpMenu()
{
  initscr();                  // Start ncurses mode
  cbreak();                   // Disable line buffering
  noecho();                   // Disable echo
  keypad(stdscr, TRUE);       // Enable function keys

  int height, width;
  getmaxyx(stdscr, height, width);   // Get screen size

  int menuHeight = 10;        // Height of the help menu window
  int menuWidth = 50;         // Width of the help menu window
  int starty = (height - menuHeight) / 2;    // Centered vertically
  int startx = (width - menuWidth) / 2;      // Centered horizontally

  WINDOW* menu_win = newwin(menuHeight, menuWidth, starty, startx);    // Create the window
  box(menu_win, 0, 0);        // Draw a box around the window

  // Display help instructions
  mvwprintw(menu_win, 1, 1, "Help Menu:");
  mvwprintw(menu_win, 2, 1, "Normal Mode:");
  mvwprintw(menu_win, 3, 3, "h/j/k/l - Move left/down/up/right");
  mvwprintw(menu_win, 4, 3, "x - Delete character");
  mvwprintw(menu_win, 5, 3, "s - Save file");
  mvwprintw(menu_win, 6, 3, "i - Switch to Insert Mode");
  mvwprintw(menu_win, 7, 3, "q - Exit IDE");

  mvwprintw(menu_win, 8, 1, "Press ESC to exit this menu");

  wrefresh(menu_win);         // Refresh the window to show the menu

  // Wait for the user to press ESC to exit the menu
  int ch;

  while ((ch = wgetch(menu_win)) != 27)
  {
    // Do nothing, just wait for ESC
  }

  // Cleanup
  delwin(menu_win);           // Delete the window
  endwin();                   // End ncurses mode
}

/**
 * Appends a character to a std::string.
 * The character is added at the end of the string.
 */
static void strcat_c(std::string& str, char c)
{
  str += c;
}

/**
 * Centers text horizontally within a given window.
 * The text is placed at the specified row, and the horizontal position
 * is calculated based on the width of the window and the length of the string.
 */
static void centerText(WINDOW* win, int starty, int width, const std::string& str)
{
  int length = str.size();
  int x = (width - length) / 2;
  mvwprintw(win, starty, x, "%s", str.c_str());
}

void editor::system::change2command()
{
  mode = Mode::command;
  cursor.set(1, getmaxy(stdscr) - 2);
  cursor.restore(0);
}
void editor::system::change2normal()
{
  mode = Mode::normal;
}
void editor::system::change2insert()
{
  mode = Mode::insert;
}

void editor::system::change2visual()
{
  // Capture the anchor
  visual_start_row = pointed_row;
  visual_start_col = pointed_col;

  mode = Mode::visual;
}

void editor::system::change2find()
{
  mode = Mode::find;
}

void editor::system::restore()
{
  starting_row = 0;
  pointed_row = 0;
  cursor.set(0, 0);
  copy_paste_buffer = "";
  status = Status::saved;
  visual_start_row  = pointed_row;
  visual_start_col  = cursor.getX();
  current_occurrence_index = -1;
  found_occurrences.clear();
}

void editor::system::switch_to_next_buffer() {
    BufferManager::instance().syncBufferFromSystemVars();
    // Passa al buffer successivo nel BufferManager
    BufferManager::instance().next();

    // Sincronizza le variabili di sistema con il nuovo buffer attivo
    BufferManager::instance().syncSystemVarsFromBuffer();

    // Aggiorna lo schermo con il nuovo buffer
    wclear(pointed_window);
    wrefresh(pointed_window);
}

void editor::system::switch_to_previous_buffer() {
    BufferManager::instance().syncBufferFromSystemVars();
    // Passa al buffer precedente nel BufferManager
    BufferManager::instance().previous();

    // Sincronizza le variabili di sistema con il nuovo buffer attivo
    BufferManager::instance().syncSystemVarsFromBuffer();

    // Aggiorna lo schermo con il nuovo buffer
    wclear(pointed_window);
    wrefresh(pointed_window);
}

void editor::system::new_buffer() {
    auto& bufferManager = BufferManager::instance();

    try {
        // Sincronizza le variabili di sistema nel buffer attivo prima di creare un nuovo buffer
        bufferManager.syncBufferFromSystemVars();

        // Genera un nome basato sull'indice del prossimo buffer
        int bufferIndex = bufferManager.getBufferCount();
        std::string bufferName = "Buffer_" + std::to_string(bufferIndex);

        // Crea il nuovo buffer
        bufferManager.create_buffer(bufferName);

        // Imposta il nuovo buffer come attivo
        bufferManager.set_active_buffer(bufferIndex);

        // Sincronizza le variabili di sistema con il nuovo buffer
        bufferManager.syncSystemVarsFromBuffer();

        // Pulisce e aggiorna la finestra per il nuovo buffer
        wclear(pointed_window);
        wrefresh(pointed_window);

    } catch (const std::exception& e) {
        // Gestisce eventuali errori nella creazione del buffer
        return;
    }
}


void editor::system::resize(){
  // 1. Resize internal buffers
  BufferManager::instance().update_all_buffers_dimensions();
  BufferManager::instance().getWindowManager().resize_windows();
  
  // 2. Update GLOBAL dimensions
  getmaxyx(pointed_window, max_row, max_col);
  
  max_row = max_row - 1;           // Reserve space for Status Bar
  max_col = max_col - span - 1;    // Reserve space for Line Numbers

  // --- HORIZONTAL LOGIC (Keep existing) ---
  if (pointed_col < max_col) {
      starting_col = 0;
  }
  else {
      int min_required_start = std::max(0, (int)(pointed_col - max_col + 2)); 
      if (starting_col > min_required_start) {
          starting_col = min_required_start;
      }
  }
  if (pointed_col >= starting_col + max_col) {
      starting_col = pointed_col - max_col + 1;
  }
  
  // --- VERTICAL LOGIC ---

  // A. If the cursor is in the first "page", just reset to top
  if (pointed_row < max_row) {
      starting_row = 0;
  }
  else {
      // B. "Backfill" Check:
      // If the current view shows void lines at the bottom (starting_row + max_row > size)
      // AND we have hidden lines at the top (starting_row > 0)...
      // We should scroll UP to fill the screen with text.
      int total_lines = buffer.getSize();
      if (starting_row + max_row > total_lines) {
           // Try to fit the last line of the file at the bottom of the screen
           int new_start = total_lines - max_row;
           if (new_start < 0) new_start = 0;
           starting_row = new_start;
      }
  }

  // C. Standard Safety Clamp
  // Ensure cursor is strictly visible at the bottom
  if (pointed_row >= starting_row + max_row) {
      starting_row = pointed_row - max_row + 1;
  }
  // Ensure cursor is strictly visible at the top
  if (pointed_row < starting_row) {
      starting_row = pointed_row;
  }

  // 5. UPDATE CURSOR
  cursor.setX(pointed_col - starting_col);
  cursor.setY(pointed_row - starting_row);

  // 6. Refresh
  wclear(stdscr);
  wrefresh(stdscr);
}