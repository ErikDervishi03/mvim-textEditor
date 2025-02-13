#include "../include/editor.hpp"

void editor::find::find_all_occurrence(const std::string& word)
{
    found_occurrences.clear();      // Clear any previous search results

    int word_len = word.length();

    for (int row = 0; row < buffer.getSize(); ++row)
    {
        std::string& buffer_row = buffer[row];  // Get the row content

        // Find all occurrences of the word in the current row
        size_t found_pos = buffer_row.find(word);

        while (found_pos != std::string::npos)
        {
            // Store the occurrence position
            found_occurrences.push_back({ row, static_cast<int>(found_pos) });

            // Search for the next occurrence in the same row
            found_pos = buffer_row.find(word, found_pos + word_len);
        }
    }
}

void editor::find::highlight_searched_word()
{
    if (mode != Mode::find)
    {
        return;
    }
    // Get visible start and end rows
    int visible_start_row = starting_row;
    int visible_end_row = std::min(static_cast<int>(starting_row + max_row), buffer.getSize() - 1);

    for (const auto& occ : found_occurrences)
    {
        int row = occ.first;
        int col = occ.second;

        // Highlight only occurrences within the visible range
        if (row >= visible_start_row && row <= visible_end_row)
        {
            // Highlight the found word in the visible row
            editor::visual::highlight(row, 
                                      row, 
                                      (col + span + 1) - starting_col,
                                      (col + current_searched_word_length + span + 1) - starting_col);
        }
    }
}

void editor::find::go_to_previous_occurrence()
{
    // Move to the previous occurrence
    current_occurrence_index = (current_occurrence_index - 1 + found_occurrences.size()) % found_occurrences.size();
    int row = found_occurrences[current_occurrence_index].first;
    int col = found_occurrences[current_occurrence_index].second;

    // Set the cursor to the new occurrence
    pointed_row = row;

    // Maintain the current selected word at the center of the screen
    if (pointed_row > max_row / 2)
    {
        starting_row = pointed_row - max_row / 2;
    }
    else
    {
        starting_row = 0;
    }
    movement::move2X(col);
    cursor.setY(row - starting_row);
}

void editor::find::go_to_next_occurrence()
{
    // Move to the next occurrence
    current_occurrence_index = (current_occurrence_index + 1) % found_occurrences.size();
    int row = found_occurrences[current_occurrence_index].first;
    int col = found_occurrences[current_occurrence_index].second;

    // Set the cursor to the new occurrence
    pointed_row = row;

    // Maintain the current selected word at the center of the screen
    if (pointed_row > max_row / 2)
    {
        starting_row = pointed_row - max_row / 2;
    }
    else
    {
        starting_row = 0;
    }

    movement::move2X(col);
    cursor.setY(row - starting_row);
}

void editor::find::find()
{
    std::string search_term = editor::system::text_form("Search term: ");
    if (search_term.empty())
    {
        return;
    }
    current_occurrence_index = -1;
    current_searched_word_length = search_term.length();

    // Find all occurrences in the entire buffer
    find_all_occurrence(search_term);

    // If any occurrences were found, move the cursor to the first one
    if (!found_occurrences.empty())
    {
        // Highlight occurrences in the visible rows
        go_to_next_occurrence();
        editor::system::change2find();
    }
}
