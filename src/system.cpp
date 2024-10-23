#include <string>
#include "../include/editor.hpp"

// Function to prompt user for confirmation before exiting unsaved changes
bool editor::system::confirm_exit()
{
  curs_set(0);
  int height, width;
  getmaxyx(stdscr, height, width);   // Get screen size

  int popupHeight = 7;    // Height of the popup window
  int popupWidth = 40;    // Width of the popup window
  int starty = (height - popupHeight) / 2;    // Centered vertically
  int startx = (width - popupWidth) / 2;      // Centered horizontally

  // Create the popup window
  WINDOW* popup_win = newwin(popupHeight, popupWidth, starty, startx);
  box(popup_win, 0, 0);   // Draw a box around the popup
  keypad(popup_win, TRUE);   // Enable function keys and arrow keys

  // Display the confirmation message
  mvwprintw(popup_win, 2, 5, "You have unsaved changes.");
  mvwprintw(popup_win, 3, 5, "You want to exit without saving?");

  // Variables for handling option selection
  int ch;
  bool confirm = false;    // True if "Yes" is selected, False if "No"
  int choice = 1;          // 1 for "No", 0 for "Yes"
  bool selection_made = false;

  // Function to update the highlighted options
  auto draw_options = [&](int choice)
    {
      if (choice == 0)
      {
        // Highlight "Yes"
        wattron(popup_win, A_REVERSE);
        mvwprintw(popup_win, 5, 12, "Yes");
        wattroff(popup_win, A_REVERSE);
        mvwprintw(popup_win, 5, 22, "No ");      // Normal "No"
      }
      else
      {
        // Highlight "No"
        mvwprintw(popup_win, 5, 12, "Yes");
        wattron(popup_win, A_REVERSE);
        mvwprintw(popup_win, 5, 22, "No ");
        wattroff(popup_win, A_REVERSE);
      }
      wrefresh(popup_win);    // Refresh to apply changes
    };

  draw_options(choice);    // Initial display with "No" highlighted

  // Capture user input (use arrow keys or enter key to choose)
  while (!selection_made)
  {
    ch = wgetch(popup_win);

    // Move between options using arrow keys
    if (ch == KEY_LEFT || ch == KEY_RIGHT)
    {
      choice = (choice == 1) ? 0 : 1;       // Toggle between 0 ("Yes") and 1 ("No")
      draw_options(choice);        // Update the highlighted option
    }
    // Confirm the selection with Enter
    else if (ch == '\n' || ch == KEY_ENTER)
    {
      confirm = (choice == 0);        // "Yes" selected if choice == 0
      selection_made = true;
    }
  }

  // Cleanup the popup window
  delwin(popup_win);
  curs_set(1);
  return confirm;
}

// Function to exit the IDE
void editor::system::exit_ide()
{
  // If the status is unsaved, prompt for confirmation
  if (status == Status::unsaved)
  {
    bool confirmed = editor::system::confirm_exit();
    if (!confirmed)
    {
      // If the user selects "No", return to the editor
      return;
    }
  }

  // Exit the IDE (end ncurses session and terminate the program)
  endwin();     // End ncurses mode
  exit(0);      // Terminate the program
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

std::string editor::system::text_form(const std::string& label)
{
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  int height, width;
  getmaxyx(stdscr, height, width);

  int formHeight = 5;
  int formWidth = 40;
  int starty = (height - formHeight) / 2;
  int startx = (width - formWidth) / 2;

  WINDOW* form_win = newwin(formHeight, formWidth, starty, startx);
  box(form_win, 0, 0);

  centerText(form_win, 1, formWidth, label);

  mvwprintw(form_win, 3, 2, "> ");
  wrefresh(form_win);

  std::string input;
  int ch;
  bool exit_form = false;

  while (1)
  {
    ch = wgetch(form_win);

    if (ch == 27)        // ESC key
    {
      exit_form = true;
      break;
    }
    else if (ch == '\n')
    {
      break;
    }
    else if (ch == KEY_BACKSPACE || ch == 127)
    {
      if (!input.empty())
      {
        input.pop_back();          // Remove last character
        mvwaddch(form_win, 3, 4 + input.size(), ' ');          // Remove character from window
        wmove(form_win, 3, 4 + input.size());
        wrefresh(form_win);
      }
    }
    else if (input.size() < 30)          // Limit input length to 30 characters
    {
      strcat_c(input, ch);        // Append character
      mvwaddch(form_win, 3, 4 + input.size() - 1, ch);        // Display character
      wrefresh(form_win);
    }
  }

  delwin(form_win);
  endwin();

  return input;
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
  visual_start_row = visual_end_row = pointed_row;
  visual_start_col = visual_end_col = cursor.getX();
  current_occurrence_index = -1;
  found_occurrences.clear();
}

// Utility function to print a message directly to the terminal (outsmvimStarter of ncurses mode)
void editor::system::print_to_terminal(int message)
{
  endwin();    // End ncurses mode
  std::cout << "dio cane";
  refresh();    // Re-enter ncurses mode
}