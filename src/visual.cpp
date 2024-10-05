#include "../include/action.hpp"
#include <ncurses.h>

void action::visual::highlight(int start_row, int end_row, int start_col, int end_col)
{
  int curr_row, curr_start_col, curr_end_col;;

  // Ensure the highlighting respects the visible range
  int visible_start_row = std::max(start_row, (int) starting_row);
  int visible_end_row = std::min(end_row, (int) (max_row + starting_row));

  int row_to_highlight = abs(visible_end_row - visible_start_row);

  // Loop through visible rows to highlight
  for (int i = 0; i <= row_to_highlight; ++i)
  {
    curr_row = (visible_end_row > visible_start_row) ? visible_start_row + i : visible_start_row - i;

    // Length of the current row
    int row_length = buffer[curr_row].length();

    // Logic to determine start and end columns for each row
    if (end_row == start_row)
    {
      // Single-row selection
      curr_start_col = start_col;
      curr_end_col = end_col;
    }
    else if (i == 0)
    {
      // First row of the selection
      curr_start_col = start_col;
      curr_end_col = (end_row < start_row) ? span + 1 : row_length + span + 1;
    }
    else if (i == row_to_highlight)
    {
      // Last row of the selection
      curr_start_col = (end_row < start_row) ? row_length + span + 1 : span + 1;
      curr_end_col = end_col;
    }
    else
    {
      // Intermediate rows
      curr_start_col = span + 1;
      curr_end_col = row_length + span + 1;
    }

    // Ensure at least one character is highlighted
    int highlight_length = std::max(abs(curr_end_col - curr_start_col), 1);

    // Highlight the current row
    mvchgat(curr_row - starting_row, std::min(curr_start_col, curr_end_col), highlight_length, A_NORMAL, 1, NULL);
  }
}

void action::visual::highlight_selected()
{
  highlight(visual_start_row, visual_end_row, visual_start_col, visual_end_col);
}



// Function to copy the highlighted text based on visual mode selection
void action::visual::copy_highlighted()
{

  copy_paste_buffer = "";

  int start_row = visual_start_row;
  int end_row = visual_end_row;
  int start_col = visual_start_col - span - 1;
  int end_col = visual_end_col - span - 1;

  // Case 1: Highlight is within a single row
  if (start_row == end_row)
  {
    // the copying process must occur from left to right so we start from the column that is furthest to the left
    int copy_start = std::min(start_col, end_col);
    int char_to_copy = std::min(abs(end_col - start_col) + 1, (int) buffer[start_row].length());    // number of characters to copy, at least 1

    copy_paste_buffer = buffer[start_row].substr(copy_start, char_to_copy);

    action::system::change2normal();
    return;
  }

  // Case 2: Highlight spans multiple rows
  if (end_row < start_row)
  {
    std::swap(start_row, end_row);
    std::swap(start_col, end_col);
  }   // swap the rows if the end row is before the start row

  copy_paste_buffer = buffer[start_row].substr(start_col);

  // Copy the middle rows entirely
  for (int i = 0; i < abs(start_row - end_row) - 1; ++i)
  {
    int curr_row = start_row + i + 1;
    copy_paste_buffer += '\n' + buffer[curr_row];
  }

  copy_paste_buffer += '\n' + buffer[end_row].substr(0, end_col);

  action::system::change2normal();
}

// Function to delete and copy the highlighted text based on visual mode selection
void action::visual::delete_highlighted()
{

  copy_paste_buffer = "";    // Clear the buffer before copying the highlighted text

  int start_row = visual_start_row;
  int end_row = visual_end_row;
  int start_col = visual_start_col - span - 1;
  int end_col = visual_end_col - span - 1;

  // Case 1: Highlight is within a single row
  if (start_row == end_row)
  {
    // The copying and deletion process must occur from left to right
    int copy_start = std::min(start_col, end_col);
    int char_to_copy = std::min(abs(end_col - start_col) + 1, (int) buffer[start_row].length());     // Number of characters to copy and delete

    // Copy and delete the text
    copy_paste_buffer = buffer.slice_row(start_row, copy_start, copy_start + char_to_copy);

    cursor.setX(std::min(start_col, end_col));      // Set the cursor to the start of the highlighted text
    action::system::change2normal();
    return;

  }
  // Case 2: Highlight spans multiple rows
  if (end_row < start_row)
  {
    std::swap(start_row, end_row);
    std::swap(start_col, end_col);
  }   // swap the rows if the end row is before the start row

  copy_paste_buffer = buffer.slice_row(start_row, start_col, buffer[start_row].length());

  // Copy the middle rows entirely
  for (int i = 0; i < end_row - start_row - 1; ++i)
  {
    copy_paste_buffer += '\n' + buffer[start_row + 1];
    buffer.del_row(start_row + 1);      // Delete the current row after copying it
  }

  copy_paste_buffer += '\n' + buffer.slice_row(start_row + 1, 0, end_col);

  buffer.merge_rows(start_row, start_row + 1);

  pointed_row = start_row;

  cursor.set(start_col, start_row - starting_row);

  // Switch back to normal mode after deletion and copying
  action::system::change2normal();
}

void action::visual::highlight_row_selected(int row, int start_col, int end_col, int color_scheme)
{
  int highlight_length = std::max(abs(end_col - start_col), 1);
  mvchgat(row - starting_row, std::min(start_col, end_col), highlight_length, A_NORMAL, color_scheme, NULL);
}

void action::visual::highlight_keywords()
{
  int visible_start_row = starting_row;
  int visible_end_row = std::min((int) (starting_row + max_row), buffer.get_number_rows() - 1);

  // Loop through each visible row
  for (int row = visible_start_row; row <= visible_end_row; ++row)
  {
    std::string& buffer_row = buffer[row];

    // Check types_old keywords
    for (int i = 0; types_old[i] != nullptr; ++i)
    {
      const char* keyword = types_old[i];
      size_t keyword_len = strlen(keyword);

      size_t found_pos = buffer_row.find(keyword);

      while (found_pos != std::string::npos)
      {
        // Check for whitespace or delimiters around the keyword
        if ((found_pos == 0 || isspace(buffer_row[found_pos - 1]) ||
             strchr("(){}[]", buffer_row[found_pos - 1])) &&        // Check left boundary
            (found_pos + keyword_len == buffer_row.size() ||
             isspace(buffer_row[found_pos + keyword_len]) ||
             strchr("(){}[]", buffer_row[found_pos + keyword_len]) ||
             strchr("+-*/%=", buffer_row[found_pos + keyword_len])))           // Check right boundary
        {
          action::visual::highlight_row_selected(row, found_pos + span + 1, found_pos + keyword_len + span + 1, 2);
        }
        found_pos = buffer_row.find(keyword, found_pos + keyword_len);
      }
    }

    // Check brackets
    for (int j = 0; brackets[j] != nullptr; ++j)
    {
      const char* bracket = brackets[j];
      size_t found_pos = buffer_row.find(bracket);

      while (found_pos != std::string::npos)
      {
        // Highlight bracket
        action::visual::highlight_row_selected(row, found_pos + span + 1, found_pos + 1 + span + 1, 3);
        found_pos = buffer_row.find(bracket, found_pos + 1);
      }
    }

    // Check for single-line comments (//)
    size_t single_line_comment_pos = buffer_row.find("//");
    if (single_line_comment_pos != std::string::npos)
    {
      action::visual::highlight_row_selected(row, single_line_comment_pos + span + 1, buffer_row.size() + span + 1, 4);
    }

  }
}
