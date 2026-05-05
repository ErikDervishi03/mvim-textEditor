#pragma once
#include "../colorManager.hpp"

namespace editor {
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

    void copy_line();

    void copy_selection(int start_row, int start_col, int end_row, int end_col);

    void delete_copy_highlighted();
  };
}