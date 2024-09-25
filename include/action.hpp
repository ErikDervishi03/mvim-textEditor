#pragma once
#include "globals/mem.h"
#include "utils.h"
#include <cstring>
#include <ncurses.h>

namespace action{

  namespace movement{
    void new_line(){
        buffer.new_row("", pointed_row + 1);
        
        //here we are breaking down the line.
        //All the text from the current cursor
        //position to the end of the line go in 
        //the next line
        
        const int curr_row_length = buffer.get_string_row(pointed_row).length();
        if (cursor.getX() != curr_row_length) {
          std::string line_break = buffer.slice_row(pointed_row, cursor.getX(), curr_row_length);
          buffer.row_append(pointed_row + 1, line_break);
        }
        
        if (cursor.getY() >= max_row - SCROLL_START_THRESHOLD - 1 && 
            !buffer.is_void_row(max_row) && 
            pointed_row < buffer.get_number_rows()) {

            starting_row++;
        }else if(cursor.getY() < max_row - 1){
          cursor.move_down();
        }

        cursor.setX(0);
        pointed_row++;
    }

    void move_up(){
      if (!(starting_row == 0 && pointed_row == 0)) {
        std::string prev_row = buffer.get_string_row(pointed_row - 1);
        if(cursor.getY() > SCROLL_START_THRESHOLD || starting_row == 0){
          cursor.move_up();
        }
        if (prev_row.length() < cursor.getX()) {
          cursor.setX(prev_row.length());
        } 
        if(starting_row > 0 && pointed_row <= starting_row + SCROLL_START_THRESHOLD){
          starting_row--;
        }
        pointed_row--;
      }
    }

    void move_down(){
      if (pointed_row < buffer.get_number_rows() - 1) {
        std::string next_row = buffer.get_string_row(pointed_row + 1);
        if(cursor.getY() < max_row - SCROLL_START_THRESHOLD - 1 || 
          pointed_row >= buffer.get_number_rows() - SCROLL_START_THRESHOLD - 1){
          
          cursor.move_down();
        }
        if(cursor.getX() >= next_row.length()){
          cursor.setX(next_row.length());
        }
        if(pointed_row - starting_row == max_row - SCROLL_START_THRESHOLD - 1 &&
          pointed_row < buffer.get_number_rows() - SCROLL_START_THRESHOLD - 1){
          
          starting_row++;
        }
        pointed_row++;
      } 
    }

    void move_left(){
      if(cursor.getX() == 0 && cursor.getY() > 0){
        cursor.set(buffer.get_string_row(pointed_row - 1).length(), cursor.getY() - 1);
        pointed_row--;
      }else if(cursor.getX() > 0){
        cursor.move_left();
      }
      if(starting_row > 0 && pointed_row == starting_row){
        starting_row--;
      }
    }

    void move_right(){
      if((cursor.getX() <= buffer.get_string_row(pointed_row).length() - 1) && !buffer.get_string_row(pointed_row).empty()){  
        cursor.move_right();
      }
    }

    void go_down_creating_newline() {
      // Create a new line below the current pointed_row
      buffer.new_row("", pointed_row + 1);

      // Move the cursor to the new line
      cursor.setX(0); // Set cursor to the beginning of the new line
      pointed_row++; // Move the pointed_row down

      // Adjust starting_row if needed
      if (cursor.getY() >= max_row - SCROLL_START_THRESHOLD - 1 &&
          !buffer.is_void_row(max_row) &&
          pointed_row < buffer.get_number_rows()) {
          starting_row++;
      } else if (cursor.getY() < max_row - 1) {
          cursor.move_down();
      }
    }

    void move_to_end_of_line() {
      // Get the length of the current row
      const int curr_row_length = buffer.get_string_row(pointed_row).length();
      
      // Set the cursor to the end of the current line
      cursor.setX(curr_row_length);
    }

    void move_to_end_of_file() {
    }


    void move_to_beginning_of_file() {
    }

  };

  namespace modify{
    void insert_letter(int letter){

      status = Status::unsaved;

      if(cursor.getX()==max_col-7){
        buffer.new_row("", pointed_row + 1);
        if (cursor.getY() >= max_row - SCROLL_START_THRESHOLD - 1 && 
            !buffer.is_void_row(max_row) && 
            pointed_row < buffer.get_number_rows()) {

            starting_row++;
        }else if(cursor.getY() < max_row - 1){
          cursor.move_down();
        }

        cursor.setX(0);
        //update_screen(); --> remember to do this in IDE
        pointed_row++;
      }
      buffer.insert_letter(pointed_row, cursor.getX(), letter);
      cursor.move_right();
      // refresh_row(pointed_row); --> remember to do this in IDE
    }

    void command_insert_letter(int letter){

      status = Status::unsaved;

      command_buffer.push_back(letter);
      cursor.move_right();
    }

    void command_delete_letter(){

      status = Status::unsaved;

      if(command_buffer.length() > 0){
        command_buffer.pop_back();
        cursor.move_left();
      }
    }

    void delete_letter(){

      status = Status::unsaved;

      if (cursor.getX() == 0 && (cursor.getY() > 0 || starting_row > 0)) {
        if (starting_row > 0 && cursor.getY() == SCROLL_START_THRESHOLD) {
            starting_row--;
        }else if(cursor.getY() > 0){
          cursor.move_up();
        }
        cursor.setX(buffer.get_string_row(pointed_row - 1).length());
        buffer.merge_rows(pointed_row - 1, pointed_row);
        pointed_row--;
      } else if (cursor.getX() > 0) {
        cursor.move_left();
        buffer.delete_letter(pointed_row, cursor.getX());
      }
    }

    void normal_delete_letter(){

      status = Status::unsaved;

      if (cursor.getX() >= 0) {
        if(buffer.get_string_row(pointed_row).length() == cursor.getX() && cursor.getX() > 0)
          cursor.move_left();
        buffer.delete_letter(pointed_row, cursor.getX());
      }
    }

    void tab(){

      status = Status::unsaved;

      for(int i = 0; i < tab_size; i++){
        buffer.insert_letter(pointed_row, cursor.getX(), ' ');
        cursor.move_right();  
      }
      // refresh_row(pointed_row);
    }

 void delete_row() {
    status = Status::unsaved;

    // Delete the current row
    if(!buffer.is_void()){
      buffer.del_row(pointed_row);
    }
  
    if (cursor.getY() > 0 || starting_row > 0) {
        if (starting_row > 0 && cursor.getY() == SCROLL_START_THRESHOLD) {
            starting_row--;
        }else if(cursor.getY() > 0){
          cursor.move_up();
        }
        cursor.setX(buffer.get_string_row(pointed_row - 1).length());
        pointed_row--;

        if(buffer.get_number_rows() == 1){
          cursor.set(0,0);
        }
    } 
  }

  };

namespace file {

    void strcat_c (char *str, char c){
      for (;*str;str++); // note the terminating semicolon here. 
      *str++ = c; 
      *str++ = 0;
    
    }
    // Function to center text on the ncurses form
    void centerText(WINDOW* win, int starty, int width, const char* str) {
        int length = strlen(str);
        int x = (width - length) / 2;
        mvwprintw(win, starty, x, "%s", str);
    }

    void form_for_filename() {
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
    void save() {

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
    void read(const char* file_name) {
        if (std::filesystem::exists(file_name) && 
            std::filesystem::is_regular_file(file_name) &&
            std::filesystem::file_size(file_name) < 1000000 &&
            std::filesystem::file_size(file_name) > 0) {

            std::ifstream myfile(file_name);
            if (!myfile.is_open()) {
                std::cerr << "Can't open: " << file_name << "\n";
                return;
            }
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

};


  namespace system{
        // Function to prompt user for confirmation before exiting unsaved changes
    bool confirm_exit() {
        curs_set(0);
        int height, width;
        getmaxyx(stdscr, height, width); // Get screen size

        int popupHeight = 7;  // Height of the popup window
        int popupWidth = 40;  // Width of the popup window
        int starty = (height - popupHeight) / 2;  // Centered vertically
        int startx = (width - popupWidth) / 2;    // Centered horizontally

        // Create the popup window
        WINDOW* popup_win = newwin(popupHeight, popupWidth, starty, startx);
        box(popup_win, 0, 0); // Draw a box around the popup
        keypad(popup_win, TRUE); // Enable function keys and arrow keys

        // Display the confirmation message
        mvwprintw(popup_win, 2, 5, "You have unsaved changes.");
        mvwprintw(popup_win, 3, 5, "You want to exit without saving?");

        // Variables for handling option selection
        int ch;
        bool confirm = false;  // True if "Yes" is selected, False if "No"
        int choice = 1;        // 1 for "No", 0 for "Yes"
        bool selection_made = false;

        // Function to update the highlighted options
        auto draw_options = [&](int choice) {
            if (choice == 0) {
                // Highlight "Yes"
                wattron(popup_win, A_REVERSE);
                mvwprintw(popup_win, 5, 12, "Yes");
                wattroff(popup_win, A_REVERSE); 
                mvwprintw(popup_win, 5, 22, "No ");  // Normal "No"
            } else {
                // Highlight "No"
                mvwprintw(popup_win, 5, 12, "Yes");
                wattron(popup_win, A_REVERSE);  
                mvwprintw(popup_win, 5, 22, "No ");
                wattroff(popup_win, A_REVERSE);
            }
            wrefresh(popup_win);  // Refresh to apply changes
        };

        draw_options(choice);  // Initial display with "No" highlighted

        // Capture user input (use arrow keys or enter key to choose)
        while (!selection_made) {
            ch = wgetch(popup_win);

            // Move between options using arrow keys
            if (ch == KEY_LEFT || ch == KEY_RIGHT) {
                choice = (choice == 1) ? 0 : 1; // Toggle between 0 ("Yes") and 1 ("No")
                draw_options(choice);  // Update the highlighted option
            }
            // Confirm the selection with Enter
            else if (ch == '\n' || ch == KEY_ENTER) {
                confirm = (choice == 0);  // "Yes" selected if choice == 0
                selection_made = true;
            }
        }

        // Cleanup the popup window
        delwin(popup_win);
        curs_set(1);
        return confirm;
    }

    // Function to exit the IDE
    void exit_ide() {
        // If the status is unsaved, prompt for confirmation
        if (status == Status::unsaved) {
            bool confirmed = confirm_exit();
            if (!confirmed) {
                // If the user selects "No", return to the editor
                return;
            }
        }

        // Exit the IDE (end ncurses session and terminate the program)
        endwin(); // End ncurses mode
        exit(0);  // Terminate the program
    }

    void helpMenu() {
      initscr();                // Start ncurses mode
      cbreak();                 // Disable line buffering
      noecho();                 // Disable echo
      keypad(stdscr, TRUE);     // Enable function keys

      int height, width;
      getmaxyx(stdscr, height, width); // Get screen size

      int menuHeight = 10;      // Height of the help menu window
      int menuWidth = 50;       // Width of the help menu window
      int starty = (height - menuHeight) / 2;  // Centered vertically
      int startx = (width - menuWidth) / 2;    // Centered horizontally

      WINDOW* menu_win = newwin(menuHeight, menuWidth, starty, startx);  // Create the window
      box(menu_win, 0, 0);      // Draw a box around the window

      // Display help instructions
      mvwprintw(menu_win, 1, 1, "Help Menu:");
      mvwprintw(menu_win, 2, 1, "Normal Mode:");
      mvwprintw(menu_win, 3, 3, "h/j/k/l - Move left/down/up/right");
      mvwprintw(menu_win, 4, 3, "x - Delete character");
      mvwprintw(menu_win, 5, 3, "s - Save file");
      mvwprintw(menu_win, 6, 3, "i - Switch to Insert Mode");
      mvwprintw(menu_win, 7, 3, "q - Exit IDE");

      mvwprintw(menu_win, 8, 1, "Press ESC to exit this menu");

      wrefresh(menu_win);       // Refresh the window to show the menu

      // Wait for the user to press ESC to exit the menu
      int ch;
      while ((ch = wgetch(menu_win)) != 27) {
        // Do nothing, just wait for ESC
      }

      // Cleanup
      delwin(menu_win);         // Delete the window
      endwin();                 // End ncurses mode
  }

    void change2command(){
      mode = command;
      cursor.set(1, getmaxy(stdscr) - 2);
      cursor.restore(0);
    }
    void change2normal(){
      mode = normal;
    }
    void change2insert(){
      mode = insert;
    }
  };

  

};