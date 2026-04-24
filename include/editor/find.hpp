#pragma once

#include <string>

namespace editor {
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
}