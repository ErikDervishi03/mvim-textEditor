#pragma once
#include "cursor.hpp"
#include "globals/mem.h"
#include "utils.h"
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <ncurses.h>

namespace action{

  namespace movement {
    /**
     * Create a new line in the buffer. 
     * If the cursor is not at the end of the line, 
     * the text from the cursor to the end of the line will be moved to the new line.
     */
    void new_line();

    /**
     * Move the cursor up one row.
     */
    void move_up();

    /**
     * Move the cursor down one row.
     */
    void move_down();

    /**
     * Move the cursor to the left by one column. 
     * If the cursor is at the beginning of the line,
     * it will move to the end of the previous line.
     */
    void move_left();

    /**
     * Move the cursor to the right by one column.
     */
    void move_right();

    /**
     * Move the cursor down and create a new line in the buffer, 
     * positioning the cursor at the start of the new line.
     * This is typically used for inserting new content below the current line.
     * the mode is switched to insert mode after the cursor is moved. 
     */
    void go_down_creating_newline();

    /**
     * Move the cursor up and create a new line in the buffer, 
     * positioning the cursor at the start of the new line.
     * This is typically used for inserting new content above the current line.
     * the mode is switched to insert mode after the cursor is moved. 
     */
    void go_up_creating_newline();

    /**
     * Move the cursor to the end of the current line.
     * If the cursor is already at the end of the line, it remains in place.
     */
    void move_to_end_of_line();

    /**
     * Move the cursor to the void beginning of the current line.
     * If the cursor is already at the beginning of the line, it remains in place.
     */
    void move_to_beginning_of_line();

    /**
     * Move the cursor to the start of the next word. 
     * A word is defined by spaces, punctuation, or other delimiter characters.
     */
    void move_to_next_word();

    /**
     * Move the cursor to the end of the file, positioning it at the last character
     * or the end of the last line, depending on the file's structure.
     */
    void move_to_end_of_file();

    /**
     * Move the cursor to the beginning of the file, positioning it at the first character.
     */
    void move_to_beginning_of_file();
  };

  namespace modify {
    
    /**
     * Insert a letter at the current cursor position. 
     * If the cursor is at the end of the line, a new row is created, and the cursor moves to the beginning of the new line.
     * Updates the buffer and moves the cursor accordingly.
     */
    void insert_letter(int letter);

    /**
     * Deletes a letter at the current cursor position. 
     * If the cursor is at the start of a line, the line merges with the previous line.
     * Adjusts the cursor position based on the changes made.
     */
    void delete_letter();

    /**
     * Deletes a letter normally at the current cursor position without handling line merging.
     * Moves the cursor left if necessary and updates the buffer.
     */
    void normal_delete_letter();

    /**
     * Inserts a tab (a series of spaces, defined by tab_size) at the current cursor position.
     * Adjusts the cursor position after each space is inserted.
     */
    void tab();

    /**
     * Deletes the entire row at the current cursor position.
     * If the buffer becomes empty, resets the buffer and cursor to the initial state.
     * Otherwise, it moves the cursor to the end of the previous row.
     */
    void delete_row();

    /**
     * Pastes the content of the copy-paste buffer at the current cursor position.
     * If a newline character is encountered, it inserts a new line and moves the cursor accordingly.
     */
    void paste();

    /**
      * when in find mode, replace the current word searched with the word provided by the user.
    */
    void replace();
    
  };

namespace file {
      /**
      * Saves the current buffer content to a file.
      * If the file name has not been provided, it prompts the user to input a file name.
      * The buffer is then written to the specified file if a valid file name is provided.
      * If saving is successful, the status is set to "saved".
      */
      void save();

      /**
      * Reads the contents of a file into the buffer.
      * It checks if the file exists, if it's a regular file, and if the file size is within a certain limit.
      * If the file is valid, its content is read line by line into the buffer. If the file can't be opened, an error is printed.
      */
      void read(const char* file_name);

      /** 
      * Displays a file selection menu to choose a file to open.
      * The user can navigate through the files and directories using arrow keys and select a file to open.
      * If the selected item is a directory, it navigates into the directory. If it's a file, it opens the file.
      * The user can exit the menu by pressing the ESC key.
      */
      void file_selection_menu();
  };


  namespace system {
      /**
      * Displays a confirmation popup asking the user if they want to exit without saving unsaved changes.
      * The user can select "Yes" or "No" using the arrow keys and confirm with Enter.
      * Returns true if the user selects "Yes" (exit without saving), false otherwise.
      */
      bool confirm_exit();

      /**
      * Exits the IDE application.
      * If there are unsaved changes, it prompts the user for confirmation via the confirm_exit() function.
      * If the user confirms, the IDE exits and ncurses mode is terminated.
      */
      void exit_ide();

      /**
      * Displays a help menu with basic instructions for using the IDE.
      * The menu includes key bindings and descriptions for normal mode commands.
      * The user can exit the menu by pressing the ESC key.
      */
      void helpMenu();

      /**
      * Switches the editor mode to "command" mode, positioning the cursor accordingly.
      */
      void change2command();

      /**
      * Displays a form to input a string
      * The user can input a string and confirm with Enter or cancel with ESC.
      * The input is returned.
      */
      char* text_form(const char* label);

      /**
      * Switches the editor mode to "normal" mode.
      */
      void change2normal();

      /**
      * Switches the editor mode to "insert" mode.
      */
      void change2insert();

      /**
      * Switches the editor mode to "visual" mode.
      */
      void change2visual();

      /**
      * Switches the editor mode to "find" mode.
      **/
      void change2find();
  };

  namespace visual {
      /**
      * Highlights the text in a specified range.
      **/
      void highlight(int start_row, int end_row, int start_col, int end_col);

      /**
      * Highlights the text in the visual selection range.
      * The function iterates through the selected rows and columns and changes the background
      * color of the highlighted text to indicate the selection.
      */
      void highlight_selected();

      /**
      * Copies the highlighted text based on the visual selection.
      * The copied text is stored in the `copy_paste_buffer`. It handles the cases where the
      * selection spans one or multiple rows and adjusts accordingly.
      * After copying, it switches the mode back to normal mode.
      */
      void copy_highlighted();

      /**
      * Deletes and copy the highlighted text based on the visual selection.
      **/
      void delete_highlighted();

      /**
      * Highlights all occurrences of the keywords in the buffer.
      **/
      void highlight_keywords();

  };

  namespace find{

    /**
    * Highlights all occurrences of the word within the visible rows.
    * The function iterates through the found occurrences and highlights the word in the visible rows.
    */
    void highlight_visible_occurrences();

    /**
    * Moves the cursor to the next occurrence of the word in the buffer.
    * The function cycles through the found occurrences and updates the cursor position to the next occurrence.
    */
    void go_to_next_occurrence();

    /**
    * Moves the cursor to the previous occurrence of the word in the buffer.
    * The function cycles through the found occurrences and updates the cursor position to the previous occurrence.
    */
    void go_to_previous_occurrence();

    /**
    * Initiates the find action by prompting the user to input a search term.
    * The function displays a form to input the search term and initiates the search process.
    */
    void find();

    /**
    * Find all occurrences of a word in the buffer.
    * The function searches for the word in each row of the buffer and stores the row and column
    * positions of each occurrence in the `found_occurrences` vector.
    */
    void find_all_occurrence(char* word);
    
  };
};

