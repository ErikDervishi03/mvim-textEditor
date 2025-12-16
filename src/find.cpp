#include "../include/editor.hpp"
#include <regex>

void editor::find::find_all_occurrence(const std::string& pattern_str)
{
    found_occurrences.clear();      // Clear any previous search results

    // OPTION 1: Try Regex Search
    try {
        // Compile the regex pattern
        std::regex pattern(pattern_str);

        for (int row = 0; row < buffer.getSize(); ++row)
        {
            const std::string& buffer_row = buffer[row]; 
            
            // Use iterator to find all regex matches in the line
            auto begin = std::sregex_iterator(buffer_row.begin(), buffer_row.end(), pattern);
            auto end = std::sregex_iterator();

            for (std::sregex_iterator i = begin; i != end; ++i)
            {
                std::smatch match = *i;
                // Store row, column (position), and the specific length of this match
                found_occurrences.push_back({ row, static_cast<int>(match.position()), static_cast<int>(match.length()) });
            }
        }
    }
    // OPTION 2: Fallback to Literal Search if Regex fails (e.g., user typed "[")
    catch (const std::regex_error& e) {
        
        int word_len = pattern_str.length();

        for (int row = 0; row < buffer.getSize(); ++row)
        {
            std::string& buffer_row = buffer[row];

            // Standard string find (not regex)
            size_t found_pos = buffer_row.find(pattern_str);

            while (found_pos != std::string::npos)
            {
                // Store the occurrence position with the fixed length
                found_occurrences.push_back({ row, static_cast<int>(found_pos), word_len });

                // Search for the next occurrence in the same row
                found_pos = buffer_row.find(pattern_str, found_pos + word_len); // Move past this match
            }
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
        // Unpack the struct
        int row = occ.row;
        int col = occ.col;
        int len = occ.length; // Use the specific length of this match

        // Highlight only occurrences within the visible range
        if (row >= visible_start_row && row <= visible_end_row)
        {
            // Highlight the found word in the visible row using its specific length
            editor::visual::highlight_row_portion(row,
                                      (col + span + 1),
                                      (col + len + span));
        }
    }
}

void editor::find::go_to_previous_occurrence()
{
    if (found_occurrences.empty()) return;

    // Move to the previous occurrence
    current_occurrence_index = (current_occurrence_index - 1 + found_occurrences.size()) % found_occurrences.size();
    
    // Access via struct members
    int row = found_occurrences[current_occurrence_index].row;
    int col = found_occurrences[current_occurrence_index].col;

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
    if (found_occurrences.empty()) return;

    // Move to the next occurrence
    current_occurrence_index = (current_occurrence_index + 1) % found_occurrences.size();
    
    // Access via struct members
    int row = found_occurrences[current_occurrence_index].row;
    int col = found_occurrences[current_occurrence_index].col;

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
    std::string search_term = editor::system::text_form("Search: ");
    if (search_term.empty())
    {
        return;
    }
    current_occurrence_index = -1;

    // Find all occurrences (Regex first, then Literal)
    find_all_occurrence(search_term);

    // If any occurrences were found, move the cursor to the first one
    if (!found_occurrences.empty())
    {
        // Highlight occurrences in the visible rows
        go_to_next_occurrence();
        editor::system::change2find();
    }
}