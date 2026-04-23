#include "../include/editor.hpp"
#include "../include/clipboardManager.hpp"
#include <ncurses.h>

// Helper to detect UTF-8 continuation bytes (10xxxxxx)
static bool is_continuation(char c) {
    return (c & 0xC0) == 0x80;
}

// Helper to get the length of the UTF-8 character starting at 'index'
static int get_utf8_char_len(const std::string& line, int index) {
    if (index >= line.length()) return 1;
    unsigned char c = (unsigned char)line[index];
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

// Helper function used in older versions
static void reverse_insert(int row, int col)
{
  cursor.setY(row);
  editor::movement::move2X(col + 1);
  pointed_row = row;

  if (pointed_row > max_row / 2)
  {
    starting_row = pointed_row - max_row / 2;
  }
  else
  {
    starting_row = 0;
  }
}

void editor::modify::insert_letter(int letter)
{
  if (!is_undoing) {
    bool chain_action = false;
    
    // Check if we should chain this action to the previous one
    if (!editor::action_history.empty()) {
        const Action& last_action = editor::action_history.top();
        
        // Only chain if the previous action was also inserting a character
        if (last_action.type == ActionType::INSERT_CHAR) {
            bool current_is_space = (letter == ' ');
            bool last_is_space = (last_action.letter == ' ');

            // Chain if both are spaces OR both are not spaces (letters/symbols)
            if (current_is_space == last_is_space) {
                chain_action = true;
            }
        }
    }

    editor::action_history.push({ActionType::INSERT_CHAR, (int)pointed_row, (int)pointed_col, (char)letter, "", chain_action});
  }
  
  status = Status::unsaved;

  buffer.insert_letter(pointed_row, pointed_col, letter);

  // UTF-8: Check if we are inserting a continuation byte
  bool is_cont = is_continuation((char)letter);

  // Only move the VISUAL cursor if it is NOT a continuation byte
  if (!is_cont) 
  {
      if (cursor.getX() == max_col - 1)
      {
        // UTF-8 SCROLL: 
        // Instead of blindly doing starting_col++, we must skip the entire 
        // character that is currently at the left edge (starting_col).
        int char_len = 1;
        if (starting_col < buffer[pointed_row].length()) {
            char_len = get_utf8_char_len(buffer[pointed_row], starting_col);
        }
        starting_col += char_len;
      }
      else
      {
        cursor.move_right();
      }
  }
  
  // Always increment the internal byte buffer position
  pointed_col++;
}

void editor::modify::new_line()
{
  if (!is_undoing) {
    // New lines always break the chain
    editor::action_history.push({ActionType::INSERT_NEWLINE, (int)pointed_row, (int)pointed_col, 0, "", false});
  }

  status = Status::unsaved;
  buffer.new_row("", pointed_row + 1);

  const int curr_row_length = buffer[pointed_row].length();
  if (pointed_col != curr_row_length)
  {
    std::string line_break = buffer.slice_row(pointed_row,
                                              pointed_col,
                                             curr_row_length);

    buffer.row_append(pointed_row + 1, line_break);
  }

  // Logic: If we are at the visual threshold AND the file is taller than the screen, scroll.
  if (cursor.getY() >= max_row - SCROLL_START_THRESHOLD - 1 && 
      buffer.getSize() > max_row)
  {
    starting_row++;
  }
  else if (cursor.getY() < max_row - 1)
  {
    cursor.move_down();
  }

  editor::movement::move2X(0);
  pointed_row++;
}


void editor::modify::delete_letter()
{
  status = Status::unsaved;

  if (pointed_col == 0 && (cursor.getY() > 0 || starting_row > 0))
  {
    if (!is_undoing) {
       editor::action_history.push({ActionType::DELETE_NEWLINE, (int)pointed_row-1, (int)buffer[pointed_row-1].length(), 0, "", false});
    }

    editor::movement::move_up();
    editor::movement::move2X(buffer[pointed_row].length());
    buffer.merge_rows(pointed_row, pointed_row+1);
  }
  else if (pointed_col > 0) 
  {
    // UTF-8: Identify how many bytes to delete (Backwards scan)
    int bytes_to_delete = 0;
    int temp_col = pointed_col;
    
    // Step back as long as we see continuation bytes
    while (temp_col > 0 && is_continuation(buffer[pointed_row][temp_col - 1])) {
        temp_col--;
        bytes_to_delete++;
    }
    // Add 1 for the start byte
    temp_col--;
    bytes_to_delete++;

    // Update visual cursor (move left once per character, not per byte)
    // UTF-8 SCROLL BACK:
    if(starting_col > 0 && pointed_col <= starting_col + 1) // At/Near left edge
    {
         // Find start of char before starting_col to scroll back correctly
         int prev = starting_col - 1;
         while(prev > 0 && is_continuation(buffer[pointed_row][prev])) prev--;
         starting_col = prev;
    }
    else {
        cursor.move_left();
    }
    
    // Move internal pointer to start of char
    pointed_col = temp_col; 

    // Delete ALL bytes of this character
    // Chain the deletions for UNDO so they restore as one block
    for(int i=0; i < bytes_to_delete; i++) {
        int del_index = pointed_col + bytes_to_delete - 1 - i;
        char char_to_delete = buffer[pointed_row][del_index]; 
        
        // The first byte deleted (last in loop) starts the chain. 
        // Wait: The loop deletes from right to left (end of char to start).
        // The first action pushed (highest index) should NOT be chained to previous random actions.
        // Subsequent bytes (lower indexes) SHOULD be chained to this one.
        bool chain = (i > 0); 
        
        if (!is_undoing) {
            editor::action_history.push({ActionType::DELETE_CHAR, (int)pointed_row, del_index, char_to_delete, "", chain});
        }
        buffer.delete_letter(pointed_row, del_index);
    }
    // Reset pointed_col to where it should be (already done via temp_col logic, but ensure it matches buffer state)
    // Actually the loop above deletes at `del_index`. pointed_col remains at `temp_col`.
  }
}

void editor::modify::normal_delete_letter()
{
  if (buffer[pointed_row].length() == 0 || pointed_col >= buffer[pointed_row].length()) return;

  status = Status::unsaved;

  // UTF-8: Identify bytes to delete (Forward scan)
  int bytes_to_delete = 1;
  while (pointed_col + bytes_to_delete < buffer[pointed_row].length() && 
         is_continuation(buffer[pointed_row][pointed_col + bytes_to_delete])) 
  {
      bytes_to_delete++;
  }

  // Delete all bytes
  for(int i=0; i<bytes_to_delete; i++) {
      // Always delete at pointed_col (buffer shifts left)
      char char_to_delete = buffer[pointed_row][pointed_col];
      bool chain = (i > 0);

      if (!is_undoing) {
          editor::action_history.push({ActionType::DELETE_CHAR, (int)pointed_row, (int)pointed_col, char_to_delete, "", chain});
      }
      buffer.delete_letter(pointed_row, pointed_col);
  }
}

void editor::modify::tab()
{
  status = Status::unsaved;
  for (int i = 0; i < tab_size; i++)
  {
    editor::modify::insert_letter(' ');
  }
}

void editor::modify::delete_row()
{
  status = Status::unsaved;

  if (!buffer.is_void())
  {
    if (!is_undoing) {
        editor::action_history.push({ActionType::DELETE_ROW, (int)pointed_row, 0, 0, buffer.get_string_row(pointed_row), false});
    }
    buffer.del_row(pointed_row);
  }

  if (buffer.is_void())
  {
    editor::movement::move2X(0);             
    cursor.setY(0);
    pointed_row = 0;             
    starting_row = 0;            
  }
  else
  {
    editor::movement::move_up();
    editor::movement::move2X(buffer[pointed_row].length());       
  }
}


void editor::modify::paste()
{
  copy_paste_buffer = ClipboardManager::getSystemClipboard();

  if (copy_paste_buffer.length() > 0)
  {
    status = Status::unsaved;

    if (!is_undoing) {
      editor::action_history.push({ActionType::PASTE, (int)pointed_row, (int)pointed_col, 0, copy_paste_buffer, false});
    }

    bool was_undoing = is_undoing;
    is_undoing = true;  

    for (char c : copy_paste_buffer)
    {
      if (c == '\n') editor::modify::new_line();
      else editor::modify::insert_letter(c);
    }
    is_undoing = was_undoing; 
  }
}

void editor::modify::paste_in_visual()
{
  if (copy_paste_buffer.empty() || mode != Mode::visual) return;
  
  status = Status::unsaved;
  std::string text_to_paste = copy_paste_buffer;

  editor::visual::delete_highlighted();
  copy_paste_buffer = text_to_paste;
  editor::modify::paste();
  editor::system::change2normal();
}

void editor::modify::replace()
{
  std::string replace_term = editor::system::text_form("Replace with: ");
  if (replace_term.empty()) {
      return;
  }

  int replace_len = replace_term.length();
  
  bool first_action_pushed = false;

  for (auto& occ : found_occurrences)
  {
    int row = occ.row;
    int col = occ.col;
    int match_len = occ.length; 

    if (!is_undoing) {
        bool chain_delete = first_action_pushed; 
        editor::action_history.push({
            ActionType::DELETE_SELECTION, 
            row, col, 0, 
            buffer[row].substr(col, match_len), 
            chain_delete
        });
        
        first_action_pushed = true; 

        editor::action_history.push({
            ActionType::PASTE, 
            row, col, 0, 
            replace_term, 
            true
        });
    }

    buffer[row].replace(col, match_len, replace_term); 

    if (replace_len != match_len)
    {
      int shift = replace_len - match_len;
      for (auto& occ_update : found_occurrences)
      {
        if (occ_update.row == row && occ_update.col > col)
        {
          occ_update.col += shift;
        }
      }
    }
  }

  editor::system::change2normal();
}

void editor::modify::delete_selection(int start_row, int end_row, int start_col, int end_col)
{
  status = Status::unsaved;
  copy_paste_buffer.clear();    

  if (start_row == end_row)
  {
    int copy_start = std::min(start_col, end_col);
    int num_chars_to_copy = std::min(abs(end_col - start_col) + 1, (int) buffer[start_row].length());     

    copy_paste_buffer = buffer.slice_row(start_row, copy_start, copy_start + num_chars_to_copy);

    if (!is_undoing) {
        editor::action_history.push({ActionType::DELETE_SELECTION, start_row, copy_start, 0, copy_paste_buffer, false});
    }

    editor::movement::move2X(copy_start);      
    return;
  }

  if (end_row < start_row)
  {
    std::swap(start_row, end_row);
    std::swap(start_col, end_col);
  }

  copy_paste_buffer = buffer.slice_row(start_row, start_col, buffer[start_row].length());

  for (int i = 0; i < end_row - start_row - 1; ++i)
  {
    copy_paste_buffer += '\n' + buffer[start_row + 1];
    buffer.del_row(start_row + 1);      
  }

  copy_paste_buffer += '\n' + buffer.slice_row(start_row + 1, 0, end_col);

  buffer.merge_rows(start_row, start_row + 1);

  if (!is_undoing) {
      editor::action_history.push({ActionType::DELETE_SELECTION, start_row, start_col, 0, copy_paste_buffer, false});
  }

  pointed_row = start_row;

  if (pointed_row > max_row / 2) starting_row = pointed_row - max_row / 2;
  else starting_row = 0;

  editor::movement::move2X(start_col);
  cursor.setY(start_row - starting_row);
}

void editor::modify::delete_word_backyard()
{
  if (pointed_col == 0) {
    delete_letter();
    return;
  }

  status = Status::unsaved;
  char curr_char_pointed = buffer[pointed_row][pointed_col - 1];

  while (curr_char_pointed == ' ')
  {
    editor::modify::delete_letter();
    if (pointed_col == 0) return;
    curr_char_pointed = buffer[pointed_row][pointed_col - 1];
  }

  while (curr_char_pointed != ' ' && pointed_col > 0)
  {
    editor::modify::delete_letter();
    curr_char_pointed = buffer[pointed_row][pointed_col - 1];
  }
}

void editor::modify::delete_word()
{
  int row_length = buffer[pointed_row].length();
  if (pointed_col == row_length) return;

  status = Status::unsaved;
  char curr_char_pointed = buffer[pointed_row][pointed_col + 1];

  while (curr_char_pointed != ' ')
  {
    editor::modify::normal_delete_letter();
    row_length--;
    if (pointed_col == row_length) return;
    curr_char_pointed = buffer[pointed_row][pointed_col + 1];
  }

  while (curr_char_pointed == ' ' && pointed_col < row_length)
  {
    editor::modify::normal_delete_letter();
    curr_char_pointed = buffer[pointed_row][pointed_col + 1];
  }
}

void editor::modify::undo()
{
  if (editor::action_history.empty()) return;

  is_undoing = true;
  status = Status::unsaved;

  bool keep_undoing = true;
  
  while (keep_undoing && !editor::action_history.empty())
  {
      Action last_action = editor::action_history.top();
      editor::action_history.pop();
      
      keep_undoing = last_action.is_chained;

      pointed_row = last_action.row;
      pointed_col = last_action.col;
      
      if (pointed_row > max_row / 2) starting_row = pointed_row - max_row / 2;
      else starting_row = 0;
      
      editor::movement::move2X(last_action.col);
      cursor.setY(pointed_row - starting_row);

      switch (last_action.type)
      {
      case ActionType::INSERT_CHAR:
        buffer.delete_letter(last_action.row, last_action.col);
        break;
      case ActionType::DELETE_CHAR:
        buffer.insert_letter(last_action.row, last_action.col, last_action.letter);
        pointed_col++;
        editor::movement::move2X(pointed_col);
        break;
      case ActionType::INSERT_NEWLINE:
        buffer.merge_rows(last_action.row, last_action.row + 1);
        break;
      case ActionType::DELETE_NEWLINE:
        editor::modify::new_line();
        break;
      case ActionType::DELETE_ROW:
        if (buffer.is_void()) {
           if (last_action.row == 0) buffer[0] = last_action.text;
           else buffer.new_row(last_action.text, last_action.row);
        } else {
            buffer.new_row(last_action.text, last_action.row);
        }
        break;
      case ActionType::DELETE_SELECTION:
      {
        std::string old_clipboard = copy_paste_buffer;
        copy_paste_buffer = last_action.text;
        editor::modify::paste();
        copy_paste_buffer = old_clipboard;
        break;
      }
      case ActionType::PASTE:
      {
        int rows = 0;
        int last_line_len = 0;
        for (char c : last_action.text) {
            if (c == '\n') { rows++; last_line_len = 0; } 
            else { last_line_len++; }
        }
        int end_row = last_action.row + rows;
        int end_col = (rows == 0) ? last_action.col + last_line_len - 1 : last_line_len;
        
        editor::modify::delete_selection(last_action.row, end_row, last_action.col, end_col);
        editor::system::change2normal();
        break;
      }
      default:
        break;
      }
  }
  
  is_undoing = false;
}