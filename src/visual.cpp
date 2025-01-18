#include "../include/editor.hpp"

// Check if the character before the found position is a valid boundary (whitespace or delimiter)
#define IS_LEFT_BOUNDARY_VALID(pos) \
  ((pos == 0) || isspace(buffer_row[pos - 1]) || strchr("(){}[]", buffer_row[pos - 1]))

// Check if the character after the keyword is a valid boundary (whitespace, delimiter, or operator)
#define IS_RIGHT_BOUNDARY_VALID(pos, len) \
  ((pos + len == buffer_row.size()) || isspace(buffer_row[pos + len]) || \
   strchr("(){}[]", buffer_row[pos + len]) || strchr("+-*/%=", buffer_row[pos + len]))

#define IS_VISIBLE_HORIZONTALLY(c) (c > starting_col && c < starting_col+max_col)


void editor::visual::highlight(int start_row, int end_row, int start_col, int end_col)
{
  int curr_row, curr_start_col, curr_end_col;

  // Ensure the highlighting respects the visible range
  int visible_start_row = std::max(start_row, (int) starting_row);
  int visible_end_row = std::min(end_row, (int)(max_row + starting_row));

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
    mvwchgat(pointed_window,curr_row - starting_row,
            std::min(curr_start_col, curr_end_col),
            highlight_length, A_NORMAL, highlightedTextColor, NULL);
  }
}

void editor::visual::highlight_selected()
{
  if (mode != Mode::visual)
  {
    return;
  }

  highlight(visual_start_row, visual_end_row, visual_start_col, visual_end_col);
}

void editor::visual::highlight_block(int from, int to){
  highlight(from, to, 0, buffer[to].length());
}



void editor::visual::highlight_row_portion(int row, int start_col, int end_col, color color_scheme)
{
  int highlight_length = std::max(abs(end_col - start_col), 1);
  mvwchgat(pointed_window, row - starting_row,
          std::min(start_col, end_col),
          highlight_length, A_NORMAL,
          color_scheme, NULL);
}



void editor::visual::highlight_keywords()
{
  int visible_start_row = starting_row;
  int visible_end_row = std::min((int)(starting_row + max_row), buffer.getSize() - 1);

  // Loop through each visible row
  for (int row = visible_start_row; row <= visible_end_row; ++row)
  {
    std::string& buffer_row = buffer[row];

    /* KEYWORDS */
    for (int i = 0; types_old[i] != nullptr; ++i)
    {
      std::string keyword = types_old[i];
      size_t keyword_len = keyword.length();
      size_t found_pos = buffer_row.find(keyword);

      while (found_pos != std::string::npos)
      {
        // Ensure the keyword is surrounded by whitespace or delimiters
        if (IS_LEFT_BOUNDARY_VALID(found_pos) && IS_RIGHT_BOUNDARY_VALID(found_pos, keyword_len))        
        {
          editor::visual::highlight_row_portion(row,
                                                (found_pos + span + 1) - starting_col,
                                                (found_pos + keyword_len + span + 1) - starting_col,
                                                keyWordColor);
        }
        found_pos = buffer_row.find(keyword, found_pos + keyword_len);
      }
    }

    /* PREPROCESSOR */
    for (int j = 0; preprocessor_directives[j] != nullptr; ++j)
    {
      std::string directive = preprocessor_directives[j];
      size_t directive_len = directive.length();
      size_t found_pos = buffer_row.find(directive);

      while (found_pos != std::string::npos)
      {
        // Ensure the directive is at the start of the line or preceded by whitespace
        if (found_pos == 0 || isspace(buffer_row[found_pos - 1]))
        {
          editor::visual::highlight_row_portion(row,
                                                 (found_pos + span + 1) - starting_col,
                                                 (found_pos + directive_len + span + 1) - starting_col,
                                                 preprocessorColor);
        }
        found_pos = buffer_row.find(directive, found_pos + 1);
      }
    }

    /* BRACKETS */
    for (int j = 0; brackets[j] != nullptr; ++j)
    {
      std::string bracket = brackets[j];
      size_t found_pos = buffer_row.find(bracket);

      while (found_pos != std::string::npos)
      {
        // Highlight the found bracket
        editor::visual::highlight_row_portion(row, 
                                              (found_pos + span + 1) - starting_col,
                                              (found_pos + 1 + span + 1) - starting_col, 
                                              bracketsColor);

        found_pos = buffer_row.find(bracket, found_pos + 1);
      }
    }

    /* SINGLE LINE COMMENTS */
    size_t single_line_comment_pos = buffer_row.find("//");

    if (single_line_comment_pos != std::string::npos)
    {
      if(IS_VISIBLE_HORIZONTALLY(single_line_comment_pos)){ 
        editor::visual::highlight_row_portion(row,
                                              (single_line_comment_pos + span + 1) - starting_col,
                                              (buffer_row.size() + span + 1) - starting_col, 
                                              commentsColor);
      }else if(pointed_col > single_line_comment_pos){
        editor::visual::highlight_row_portion(row,
                                      span+1,
                                      (buffer_row.size() + span + 1) - starting_col, 
                                      commentsColor);
      }
    }

    /* MULTILINE COMMENTS */  //(/* ... */)
    size_t multi_line_comment_start = buffer_row.find("/*");
    size_t multi_line_comment_end = buffer_row.find("*/", multi_line_comment_start);

    // Multi-line comment that starts and ends on the same line
    if (multi_line_comment_start != std::string::npos && multi_line_comment_end != std::string::npos)
    {
      editor::visual::highlight_row_portion(row, 
                                            (multi_line_comment_start + span + 1) - starting_col,
                                            (multi_line_comment_end + 2 + span + 1) - starting_col,
                                            commentsColor);
    }
    // Multi-line comment that starts on this line and continues
    else if (multi_line_comment_start != std::string::npos)
    {
      editor::visual::highlight_row_portion(row,
                                            (multi_line_comment_start + span + 1) - starting_col,
                                            (buffer_row.size() + span + 1) - starting_col, 
                                            commentsColor);

      // Continue highlighting the next rows until we find the closing "*/"
      int next_row = row + 1;

      while (next_row <= visible_end_row && buffer[next_row].find("*/") == std::string::npos)
      {
        editor::visual::highlight_row_portion(next_row,
                                              (span + 1) - starting_col,
                                              (buffer[next_row].size() + span + 1) - starting_col, 
                                              commentsColor);
        next_row++;
      }

      // Highlight the closing part of the multi-line comment
      if (next_row <= visible_end_row)
      {
        size_t multi_line_comment_end_next_row = buffer[next_row].find("*/");
        editor::visual::highlight_row_portion(next_row, 
                                              (span + 1) - starting_col,
                                              (multi_line_comment_end_next_row + 2 + span + 1) - starting_col, 
                                              commentsColor);
      }
    }
  }
}

// Function to delete and copy the highlighted text based on visual mode selection
void editor::visual::delete_highlighted()
{
  int start_row = visual_start_row;
  int end_row = visual_end_row;
  int start_col = visual_start_col - span - 1;
  int end_col = visual_end_col - span - 1;

  editor::modify::delete_selection(start_row, end_row, start_col, end_col);
  editor::system::change2normal();
}

// Function to copy the highlighted text based on visual mode selection
void editor::visual::copy_highlighted()
{
  copy_paste_buffer = "";

  int start_row = visual_start_row;
  int end_row = visual_end_row;
  int start_col = visual_start_col - span - 1;
  int end_col = visual_end_col - span - 1;

  // Case 1: Highlight is within a single row
  if (start_row == end_row)
  {
    int copy_start = std::min(start_col, end_col);
    int char_to_copy = std::min(abs(end_col - start_col) + 1, 
                                (int)buffer[start_row].length()); // number of characters to copy, at least 1

    copy_paste_buffer = buffer[start_row].substr(copy_start, char_to_copy);
    editor::system::change2normal();
    return;
  }

  // Case 2: Highlight spans multiple rows
  if (end_row < start_row)
  {
    std::swap(start_row, end_row);
    std::swap(start_col, end_col);
  }

  copy_paste_buffer = buffer[start_row].substr(start_col);

  // Copy the middle rows entirely
  for (int i = 0; i < abs(start_row - end_row) - 1; ++i)
  {
    int curr_row = start_row + i + 1;
    copy_paste_buffer += '\n' + buffer[curr_row];
  }

  copy_paste_buffer += '\n' + buffer[end_row].substr(0, end_col);

  editor::system::change2normal();
}

void editor::visual::insert_brackets(char opening_bracket, char closing_bracket) {
    buffer.insert_letter(visual_start_row, visual_start_col - span, opening_bracket);
    buffer.insert_letter(visual_end_row, visual_end_col - span - 1, closing_bracket);
    system::change2normal();
}