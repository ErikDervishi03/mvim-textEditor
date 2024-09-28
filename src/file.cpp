#include "../include/action.hpp"
#include <ncurses.h>

  namespace fs = std::filesystem;

  /**
  * Appends a character to a null-terminated C string.
  * The character is added at the end of the string, and the null terminator is updated.
  */
  static void strcat_c (char *str, char c){
    for (;*str;str++); // note the terminating semicolon here. 
    *str++ = c; 
    *str++ = 0;
  
  }

  /**
  * Centers text horizontally within a given window.
  * The text is placed at the specified row, and the horizontal position 
  * is calculated based on the width of the window and the length of the string.
  */
  static void centerText(WINDOW* win, int starty, int width, const char* str) {
      int length = strlen(str);
      int x = (width - length) / 2;
      mvwprintw(win, starty, x, "%s", str);
  }

  /**
  * Displays a form for inputting a filename.
  * The form consists of a simple window where the user can type a filename, with support for backspace and exiting via the ESC key.
  * Once the filename is entered and confirmed, it is stored in the global variable `pointed_file`.
  */
  static void form_for_filename() {
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

      const char* label = "Insert file name:";
      centerText(form_win, 1, formWidth, label);

      mvwprintw(form_win, 3, 2, "> ");
      wrefresh(form_win);

      char filename[30] = {0};  // Adjusted buffer for filename input
      int ch, i = 0;
      bool exit_form = false;

      while (1) {
          ch = wgetch(form_win);

          if (ch == 27) {  // ESC key
              exit_form = true;
              break;
          } else if (ch == '\n') {
              break;
          } else if (ch == KEY_BACKSPACE || ch == 127) {
              if (i > 0) {
                  filename[--i] = '\0';  // Remove last character from filename
                  mvwaddch(form_win, 3, 4 + i, ' ');  // Remove character from window
                  wmove(form_win, 3, 4 + i);
                  wrefresh(form_win);
              }
          } else if (i < sizeof(filename) - 1) {  // Avoid buffer overflow
              strcat_c(filename, ch);  // Append character
              mvwaddch(form_win, 3, 4 + i, ch);  // Display character
              i++;
              wrefresh(form_win);
          }
      }

      delwin(form_win);
      endwin();

      if (!exit_form) {
          strncpy(pointed_file, filename, sizeof(filename) - 1);
      }
  }


  // Save function
  void action::file::save() {

      if (strcmp(pointed_file, "") == 0) {
          form_for_filename();  // Prompt for filename if not set
      }

      // Only save if a filename was entered
      if (strcmp(pointed_file, "") != 0) {
          status = Status::saved;
          std::ofstream myfile(pointed_file);
          if (!myfile.is_open()) {
              std::cerr << "Failed to open file for writing: " << pointed_file << "\n";
              return;
          }
          for (int i = 0; i < buffer.get_number_rows(); i++) {
              myfile << buffer.get_buffer()[i] << "\n";
          }
          myfile.close();
      } else {
          std::cerr << "No file name provided. Save aborted.\n";
      }
  }

  // Read function
  void action::file::read(const char* file_name) {
      if (std::filesystem::exists(file_name) && 
          std::filesystem::is_regular_file(file_name) &&
          std::filesystem::file_size(file_name) < 1000000 &&
          std::filesystem::file_size(file_name) > 0) {

          std::ifstream myfile(file_name);
          if (!myfile.is_open()) {
              std::cerr << "Can't open: " << file_name << "\n";
              return;
          }

          status = Status::saved;

          buffer.clear();
          std::string line;
          while (std::getline(myfile, line)) {
              buffer.push_back(line);
          }

          myfile.close();
      } else {
          buffer.restore();
      }
  }

void action::file::file_selection_menu() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);  // Hide the cursor

    fs::path current_path = fs::current_path();  // Start from the current directory
    std::vector<std::string> files;
    int selected = 0;
    bool exit_menu = false;

    // Helper function to load directory contents
    auto load_directory = [&]() {
        files.clear();
        files.push_back(".."); // Option to go to the parent directory

        for (const auto& entry : fs::directory_iterator(current_path)) {
            files.push_back(entry.path().filename().string());
        }
        std::sort(files.begin(), files.end()); // Sort files alphabetically
    };

    // Load initial directory
    load_directory();

    int height, width;
    getmaxyx(stdscr, height, width);

    while (!exit_menu) {
        clear();  // Clear the screen

        // Display the full path of the current directory
        std::string full_path = current_path.string();  // Full path
        mvprintw(0, 0, "Current Path: %s", full_path.c_str());

        // Display the list of files
        for (size_t i = 0; i < files.size(); ++i) {
            if (i == selected) {
                attron(A_REVERSE);  // Highlight selected item
            }
            mvprintw(2 + i, 2, "%s", files[i].c_str());
            if (i == selected) {
                attroff(A_REVERSE); // Remove highlight
            }
        }

        // Refresh the screen
        refresh();

        // Handle user input for navigation
        int ch = getch();
        switch (ch) {
            case KEY_UP:
                selected = (selected > 0) ? selected - 1 : files.size() - 1;
                break;
            case KEY_DOWN:
                selected = (selected + 1) % files.size();
                break;
            case '\n':  // Enter key to select
                if (files[selected] == "..") {
                    // Go back to the parent directory
                    current_path = current_path.parent_path();
                    load_directory();
                    selected = 0;  // Reset selection to the top
                } else {
                    // Check if it's a directory or file
                    fs::path chosen_path = current_path / files[selected];
                    if (fs::is_directory(chosen_path)) {
                        current_path = fs::absolute(chosen_path);
                        load_directory();
                        selected = 0;  // Reset selection to the top
                    } else {
                        // It's a file, call the `read` function of `action::file`
                        action::file::read(chosen_path.c_str());
                        exit_menu = true;  // Exit menu after opening the file
                    }
                }
                break;
            case 27:  // ESC key to exit the menu
                exit_menu = true;
                break;
        }
    }
    curs_set(1);
    endwin();
}