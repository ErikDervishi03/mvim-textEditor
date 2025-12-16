#pragma once
#include "globals/mvimResources.h"
// Standard libraries required for std::stack, std::vector, std::string
#include <string>
#include <vector>
#include <stack>
#include <utility>

namespace editor
{
  enum ActionType { INSERT_CHAR, DELETE_CHAR, INSERT_NEWLINE, DELETE_NEWLINE, DELETE_ROW, PASTE, DELETE_SELECTION };

  struct Action
  {
    ActionType type;
    int row;
    int col;
    char letter;              
    std::string text;         
    bool is_chained = false; // if true, undo will continue to the next item
  };

  inline std::stack<Action> action_history;

  // Structure to hold variable-length match data for Regex
  struct SearchMatch {
      int row;
      int col;
      int length;
  };

  inline std::vector<SearchMatch> found_occurrences;  
  inline int current_occurrence_index;

  namespace movement
  {
    void move2X(int x);
    
    /**
     * @brief Move the cursor up one row.
     */
    void move_up();

    /**
     * @brief Move the cursor down one row.
     */
    void move_down();

    /**
     * @brief Move the cursor to the left by one column.
     * If the cursor is at the beginning of the line, it will move to the end of the previous line.
     */
    void move_left();

    /**
     * @brief Move the cursor to the right by one column.
     */
    void move_right();

    /**
     * @brief Move the cursor down and create a new line in the buffer,
     * positioning the cursor at the start of the new line.
     * This is typically used for inserting new content below the current line.
     * The mode is switched to insert mode after the cursor is moved.
     */
    void go_down_creating_newline();

    /**
     * @brief Move the cursor up and create a new line in the buffer,
     * positioning the cursor at the start of the new line.
     * This is typically used for inserting new content above the current line.
     * The mode is switched to insert mode after the cursor is moved.
     */
    void go_up_creating_newline();

    /**
     * @brief Move the cursor to the end of the current line.
     * If the cursor is already at the end of the line, it remains in place.
     */
    void move_to_end_of_line();

    /**
     * @brief Move the cursor to the beginning of the current line.
     * If the cursor is already at the beginning of the line, it remains in place.
     */
    void move_to_beginning_of_line();

    /**
     * @brief Move the cursor to the start of the next word.
     * A word is defined by spaces, punctuation, or other delimiter characters.
     */
    void move_to_next_word();

    /**
     * @brief Move the cursor to the end of the file,
     * positioning it at the last character or the end of the last line,
     * depending on the file's structure.
     */
    void move_to_end_of_file();

    /**
     * @brief Move the cursor to the beginning of the file,
     * positioning it at the first character.
     */
    void move_to_beginning_of_file();
  };

  namespace modify
  {
    /**
     * @brief Insert a letter at the current cursor position.
     * If the cursor is at the end of the line, a new row is created,
     * and the cursor moves to the beginning of the new line.
     * @param letter The ASCII value of the letter to be inserted.
     */
    void insert_letter(int letter);

    /**
     * @brief Create a new line in the buffer.
     * If the cursor is not at the end of the line, the text from the cursor to the end of the line will be moved to the new line.
     */
    void new_line();

    /**
     * @brief Deletes a letter at the current cursor position.
     * If the cursor is at the start of a line, the line merges with the previous line.
     * Adjusts the cursor position based on the changes made.
     */
    void delete_letter();

    /**
     * @brief Deletes a letter normally at the current cursor position
     * without handling line merging.
     * Moves the cursor left if necessary and updates the buffer.
     */
    void normal_delete_letter();

    /**
     * @brief Inserts a tab (a series of spaces, defined by tab_size)
     * at the current cursor position.
     * Adjusts the cursor position after each space is inserted.
     */
    void tab();

    /**
     * @brief Deletes the entire row at the current cursor position.
     * If the buffer becomes empty, resets the buffer and cursor to the initial state.
     * Otherwise, it moves the cursor to the end of the previous row.
     */
    void delete_row();

    /**
     * @brief Pastes the content of the copy-paste buffer at the current cursor position.
     * If a newline character is encountered, it inserts a new line
     * and moves the cursor accordingly.
     */
    void paste();

    void paste_in_visual();

    /**
     * @brief Replaces the current word searched with the word provided by the user.
     * This function is used when in find mode.
     */
    void replace();

    void delete_selection(int start_row, int end_row, int start_col, int end_col);

    void delete_word();

    void delete_word_backyard();

    void undo();
  };

  namespace file
  {
    /**
     * @brief Saves the current buffer content to a file.
     * If the file name has not been provided, it prompts the user to input a file name.
     * The buffer is then written to the specified file if a valid file name is provided.
     * If saving is successful, the status is set to "saved".
     */
    void save();

    /**
     * @brief Reads the contents of a file into the buffer.
     * Checks if the file exists, if it's a regular file, and if the file size is within a certain limit.
     * If the file is valid, its content is read line by line into the buffer.
     * If the file can't be opened, an error is printed.
     * @param file_name The name of the file to read.
     */
    void read(std::string file_name);

    /**
     * @brief Displays a file selection menu to choose a file to open.
     * The user can navigate through the files and directories using arrow keys and select a file to open.
     * If the selected item is a directory, it navigates into the directory. If it's a file, it opens the file.
     * The user can exit the menu by pressing the ESC key.
     */
    void file_selection_menu();
  };


  namespace system
  {
    /**
     * @brief Displays a confirmation popup asking the user if they want to exit without saving unsaved changes.
     * The user can select "Yes" or "No" using the arrow keys and confirm with Enter.
     * @return true if the user selects "Yes" (exit without saving), false otherwise.
     */
    bool confirm_exit();

    /**
     * @brief Exits the IDE application.
     * If there are unsaved changes, it prompts the user for confirmation via the confirm_exit() function.
     * If the user confirms, the IDE exits and ncurses mode is terminated.
     */
    void exit_ide();

    /**
     * @brief Displays a help menu with basic instructions for using the IDE.
     * The menu includes key bindings and descriptions for normal mode commands.
     * The user can exit the menu by pressing the ESC key.
     */
    void helpMenu();

    /**
     * @brief Switches the editor mode to "command" mode, positioning the cursor accordingly.
     */
    void change2command();

    /**
     * @brief Displays a form to input a string.
     * The user can input a string and confirm with Enter or cancel with ESC.
     * @param label The label for the input form.
     * @return The input string.
     */
    std::string text_form(const std::string& label);

    /**
     * @brief Switches the editor mode to "normal" mode.
     */
    void change2normal();

    /**
     * @brief Switches the editor mode to "insert" mode.
     */
    void change2insert();

    /**
     * @brief Switches the editor mode to "visual" mode.
     */
    void change2visual();

    /**
     * @brief Switches the editor mode to "find" mode.
     */
    void change2find();

    /**
     * @brief Restores the previous state of the editor.
     */
    void restore();

    // Utility function to print a message directly to the terminal (outsmvimStarter of ncurses mode)
    void print_to_terminal(int message);

    void switch_to_next_buffer();

    void switch_to_previous_buffer();

    void new_buffer();

    void resize();

  };

  namespace visual
  {
    /**
     * @brief Highlights the text in a specified range.
     * @param start_row The starting row of the highlight.
     * @param end_row The ending row of the highlight.
     * @param start_col The starting column of the highlight.
     * @param end_col The ending column of the highlight.
     */
    void highlight(int start_row, int end_row, int start_col, int end_col, color highlight_color = highlightedTextColor);

    /**
     * @brief Highlights the text in the visual selection range.
     * The function iterates through the selected rows and columns and changes the background
     * color of the highlighted text to indicate the selection.
     */
    void highlight_selected();

    /**
     * @brief Highlights the text in the specified row with the given color scheme.
     * @param row The row to highlight.
     * @param start_col The starting column of the highlight.
     * @param end_col The ending column of the highlight.
     * @param color_scheme The color scheme to apply for highlighting.
     */
    void highlight_row_portion(int row, int start_col, int end_col, color highlight_color = highlightedTextColor);

    /**
     * @brief Copies the highlighted text based on the visual selection.
     * The copied text is stored in the `copy_paste_buffer`.
     * It handles cases where the selection spans one or multiple rows and adjusts accordingly.
     * After copying, it switches the mode back to normal mode.
     */
    void copy_highlighted();

    /**
     * @brief Highlights all occurrences of the keywords in the buffer.
     */
    void highlight_keywords();

    /**
     * @brief Deletes and copies the highlighted text based on the visual selection.
     */
    void delete_highlighted();

    /**
      * @brief highlight a block of rows
      * @param from first row
      * @param to last row
     */
    void highlight_block(int from, int to);

    void insert_brackets(char opening_bracket, char closing_bracket);

    void select_all();
  };

  namespace find
  {
    /**
     * @brief Highlights all occurrences of the word within the visible rows.
     * The function iterates through the found occurrences and highlights the word in the visible rows.
     */
    void highlight_searched_word();

    /**
     * @brief Moves the cursor to the next occurrence of the word in the buffer.
     * The function cycles through the found occurrences and updates the cursor position to the next occurrence.
     */
    void go_to_next_occurrence();

    /**
     * @brief Moves the cursor to the previous occurrence of the word in the buffer.
     * The function cycles through the found occurrences and updates the cursor position to the previous occurrence.
     */
    void go_to_previous_occurrence();

    /**
     * @brief Initiates the find action by prompting the user to input a search term.
     * The function displays a form to input the search term and initiates the search process.
     */
    void find();

    /**
     * @brief Find all occurrences of a word in the buffer.
     * The function searches for the word in each row of the buffer and stores the row and column
     * positions of each occurrence in the `found_occurrences` vector.
     * @param word The word to search for in the buffer.
     */
    void find_all_occurrence(const std::string& word);
  };
};