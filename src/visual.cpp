#include "../include/editor.hpp"
#include "../include/syntax.hpp"
#include "../include/clipboardManager.hpp"

// Check if the character before the found position is a valid boundary (whitespace or delimiter)
#define IS_LEFT_BOUNDARY_VALID(pos) \
  ((pos == 0) || isspace(buffer_row[pos - 1]) || strchr("(){}[]", buffer_row[pos - 1]))

// Check if the character after the keyword is a valid boundary (whitespace, delimiter, or operator)
#define IS_RIGHT_BOUNDARY_VALID(pos, len) \
  ((pos + len == buffer_row.size()) || isspace(buffer_row[pos + len]) || \
   strchr("(){}[]", buffer_row[pos + len]) || strchr("+-*/%=", buffer_row[pos + len]))

#define IS_VISIBLE_HORIZONTALLY(c) (c >= starting_col && c < starting_col+max_col)


void editor::visual::highlight(int start_row, int end_row, int start_col, int end_col, color highlight_color)
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
      curr_start_col = std::max(start_col - (int)starting_col, span+1);
      curr_end_col = end_col - starting_col;
    }
    else if (curr_row == start_row)
    {
      // First row of the selection
      curr_start_col = std::max(start_col - (int)starting_col,span+1);
      curr_end_col = (end_row > start_row) ? std::min((int)max_col, row_length - (int)starting_col) + span + 1 : span + 1;
    }
    else if (i == row_to_highlight)
    {
      // Last row of the selection
      curr_start_col = (end_row > start_row) ? span + 1 : std::min((int)max_col, row_length - (int)starting_col) + span + 1;
      curr_end_col = end_col - starting_col;
    }
    else
    {
      // Intermediate rows
      curr_start_col = span + 1;
      curr_end_col = std::min((int)max_col, std::max(row_length - (int)starting_col, 0)) + span + 1;
    }

    // Ensure at least one character is highlighted
    int highlight_length = (curr_end_col != curr_start_col) ? abs(curr_end_col - curr_start_col) + 1 : 1;

    // Highlight the current row
    mvwchgat(pointed_window,curr_row - starting_row,
            std::min(curr_start_col, curr_end_col),
            highlight_length, A_NORMAL, highlight_color, NULL);
  }
}

void editor::visual::highlight_selected()
{
  if (mode != Mode::visual)
  {
    return;
  }

  highlight(visual_start_row, pointed_row, visual_start_col + span + 1, pointed_col + span + 1);
}

void editor::visual::highlight_block(int from, int to){
  highlight(from, to, 0, buffer[to].length());
}

void editor::visual::highlight_row_portion(int row, int start_col, int end_col, color color_scheme)
{
  highlight(row, row, start_col, end_col ,color_scheme);
}

void editor::visual::highlight_keywords()
{
  // 1. Get the current language rules
  const Language* lang = SyntaxHighlighter::instance().getCurrentLanguage();
  
  // If no language is detected (plain text), do nothing
  if (!lang) return;

  int visible_start_row = starting_row;
  int visible_end_row = std::min((int)(starting_row + max_row), buffer.getSize() - 1);

  // --- Pre-calculate if we start inside a multi-line comment ---
  bool in_multiline_comment = false;
  if (!lang->multiLineCommentStart.empty() && !lang->multiLineCommentEnd.empty()) {
      for (int r = 0; r < visible_start_row; ++r) {
          std::string& r_str = buffer[r];
          size_t pos = 0;
          while (pos < r_str.length()) {
              if (!in_multiline_comment) {
                  pos = r_str.find(lang->multiLineCommentStart, pos);
                  if (pos != std::string::npos) {
                      in_multiline_comment = true;
                      pos += lang->multiLineCommentStart.length();
                  } else break;
              } else {
                  pos = r_str.find(lang->multiLineCommentEnd, pos);
                  if (pos != std::string::npos) {
                      in_multiline_comment = false;
                      pos += lang->multiLineCommentEnd.length();
                  } else break;
              }
          }
      }
  }

  for (int row = visible_start_row; row <= visible_end_row; ++row)
  {
    std::string& buffer_row = buffer[row];

    /* 2. Highlight Keywords Groups */
    for (const auto& group : lang->syntaxGroups)
    {
        for (const std::string& keyword : group.keywords)
        {
            size_t keyword_len = keyword.length();
            size_t found_pos = buffer_row.find(keyword);

            while (found_pos != std::string::npos)
            {
                if (IS_LEFT_BOUNDARY_VALID(found_pos) && IS_RIGHT_BOUNDARY_VALID(found_pos, keyword_len))        
                {
                    editor::visual::highlight_row_portion(row,
                                                          found_pos + span + 1,
                                                          found_pos + keyword_len + span,
                                                          group.color);
                }
                found_pos = buffer_row.find(keyword, found_pos + keyword_len);
            }
        }
    }

    /* 3. Highlight Brackets */
    if (!lang->brackets.empty()) {
        for (char bracketChar : lang->brackets) {
            std::string bracket(1, bracketChar); 
            size_t found_pos = buffer_row.find(bracket);

            while (found_pos != std::string::npos)
            {
                editor::visual::highlight_row_portion(row, 
                                                      found_pos + span + 1,
                                                      found_pos + span + 1, 
                                                      bracketsColor);

                found_pos = buffer_row.find(bracket, found_pos + 1);
            }
        }
    }

    /* 4. Highlight Single Line Comments */
    if (!lang->singleLineComment.empty()) {
        size_t single_line_comment_pos = buffer_row.find(lang->singleLineComment);

        if (single_line_comment_pos != std::string::npos)
        {
            if(IS_VISIBLE_HORIZONTALLY(single_line_comment_pos) || single_line_comment_pos < starting_col){ 
                editor::visual::highlight_row_portion(row,
                                                      single_line_comment_pos + span + 1,
                                                      buffer_row.size() + span, 
                                                      commentsColor);
            }
        }
    }

    /* 5. Highlight Multi-line Comments */
    if (!lang->multiLineCommentStart.empty() && !lang->multiLineCommentEnd.empty()) {
        if (in_multiline_comment && buffer_row.empty()) {
            // Keep empty lines highlighted if they are inside a comment block
            editor::visual::highlight_row_portion(row, span + 1, span + 1, commentsColor);
        } else {
            size_t pos = 0;
            while (pos < buffer_row.length()) {
                if (!in_multiline_comment) {
                    size_t start_pos = buffer_row.find(lang->multiLineCommentStart, pos);
                    if (start_pos == std::string::npos) break; // No comment starts on this line
                    
                    in_multiline_comment = true;
                    size_t end_pos = buffer_row.find(lang->multiLineCommentEnd, start_pos + lang->multiLineCommentStart.length());
                    
                    if (end_pos != std::string::npos) {
                        // Comment starts and ends on this line
                        editor::visual::highlight_row_portion(row, start_pos + span + 1, end_pos + lang->multiLineCommentEnd.length() + span, commentsColor);
                        in_multiline_comment = false;
                        pos = end_pos + lang->multiLineCommentEnd.length();
                    } else {
                        // Comment starts on this line, and continues downwards
                        editor::visual::highlight_row_portion(row, start_pos + span + 1, buffer_row.length() + span, commentsColor);
                        break; 
                    }
                } else {
                    size_t end_pos = buffer_row.find(lang->multiLineCommentEnd, pos);
                    if (end_pos != std::string::npos) {
                        // Comment ends on this line
                        editor::visual::highlight_row_portion(row, pos + span + 1, end_pos + lang->multiLineCommentEnd.length() + span, commentsColor);
                        in_multiline_comment = false;
                        pos = end_pos + lang->multiLineCommentEnd.length();
                    } else {
                        // Comment covers the entire line
                        editor::visual::highlight_row_portion(row, pos + span + 1, buffer_row.length() + span, commentsColor);
                        break; 
                    }
                }
            }
        }
    }
  }
}

void editor::visual::delete_highlighted()
{
  int start_row = visual_start_row;
  int end_row = pointed_row;
  int start_col = visual_start_col; 
  int end_col = pointed_col;        

  editor::modify::delete_selection(start_row, end_row, start_col, end_col);
  editor::system::change2normal();
}


void editor::visual::copy_highlighted()
{
  copy_paste_buffer = "";

  int start_row = visual_start_row;
  int end_row = pointed_row;
  int start_col = visual_start_col; 
  int end_col = pointed_col;        

  if (start_row == end_row)
  {
    int copy_start = std::min(start_col, end_col);
    int char_to_copy = std::min(abs(end_col - start_col) + 1, (int)buffer[start_row].length());
    copy_paste_buffer = buffer[start_row].substr(copy_start, char_to_copy);
  }
  else
  {
    if (end_row < start_row)
    {
      std::swap(start_row, end_row);
      std::swap(start_col, end_col);
    }

    copy_paste_buffer = buffer[start_row].substr(start_col);

    for (int i = 0; i < abs(start_row - end_row) - 1; ++i)
    {
      int curr_row = start_row + i + 1;
      copy_paste_buffer += '\n' + buffer[curr_row];
    }

    copy_paste_buffer += '\n' + buffer[end_row].substr(0, end_col);
  }

  ClipboardManager::setSystemClipboard(copy_paste_buffer);

  editor::system::change2normal();
}

void editor::visual::insert_brackets(char opening_bracket, char closing_bracket) {
    int start_row = visual_start_row;
    int end_row = pointed_row;
    int start_col = visual_start_col; 
    int end_col = pointed_col;

    if (end_row < start_row) {
        std::swap(start_row, end_row);
        std::swap(start_col, end_col);
    }

    if (start_row == end_row && end_col < start_col) {
        std::swap(start_col, end_col);
    }

    buffer.insert_letter(start_row, start_col, opening_bracket);

    if (start_row == end_row) {
        end_col++; 
    }
    
    if (end_col < buffer[end_row].length()) {
        end_col++;
    }

    buffer.insert_letter(end_row, end_col, closing_bracket);

    system::change2normal();
}


void editor::visual::select_all()
{
  editor::movement::move_to_beginning_of_file();

  visual_start_row = pointed_row; 
  visual_start_col = pointed_col; 

  mode = Mode::visual;

  editor::movement::move_to_end_of_file();
}

void editor::visual::copy_line()
{
  if (pointed_row < buffer.getSize())
  {
    // Copy the content of the current line to the buffer
    copy_paste_buffer = buffer[pointed_row]; 
  }
}