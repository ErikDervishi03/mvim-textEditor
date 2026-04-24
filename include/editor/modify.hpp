#pragma once

namespace editor {
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
}