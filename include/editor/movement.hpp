#pragma once

namespace editor {
  namespace movement
  {
    void move2Y(int y, bool center_view = false);

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

    void move_to_previous_word();

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
}