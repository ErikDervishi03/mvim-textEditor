#include "../include/editor.hpp"

// Helper function used in older versions, kept if needed for reference or other logic
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
    editor::action_history.push({ActionType::INSERT_CHAR, (int)pointed_row, (int)pointed_col, (char)letter, ""});
  }
  
  status = Status::unsaved;

  buffer.insert_letter(pointed_row, pointed_col, letter);

  if (cursor.getX() == max_col - 1)
  {
    starting_col++;
  }
  else
  {
    cursor.move_right();
  }
  pointed_col++;
}

void editor::modify::new_line()
{
  if (!is_undoing) {
    editor::action_history.push({ActionType::INSERT_NEWLINE, (int)pointed_row, (int)pointed_col, 0, ""});
  }

  status = Status::unsaved;
  buffer.new_row("", pointed_row + 1);

  // Break the line: move text from cursor to end of line to the next line
  const int curr_row_length = buffer[pointed_row].length();
  if (pointed_col != curr_row_length)
  {
    std::string line_break = buffer.slice_row(pointed_row,
                                              pointed_col,
                                             curr_row_length);

    buffer.row_append(pointed_row + 1, line_break);
  }

  if (cursor.getY() >= max_row - SCROLL_START_THRESHOLD - 1 &&
      !buffer.is_void_row(max_row) && pointed_row < buffer.getSize())
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
    // Merging rows (Backspacing at start of line)
    if (!is_undoing) {
       // We record the position at the end of the previous line where the merge happens
       editor::action_history.push({ActionType::DELETE_NEWLINE, (int)pointed_row-1, (int)buffer[pointed_row-1].length(), 0, ""});
    }

    editor::movement::move_up();
    editor::movement::move2X(buffer[pointed_row].length());
    buffer.merge_rows(pointed_row, pointed_row+1);
  }
  else if (pointed_col > 0) // normal deleting
  {
    if(starting_col + 1 == pointed_col && starting_col != 0) starting_col--;
    else cursor.move_left();
    pointed_col--;

    char char_to_delete = buffer[pointed_row][pointed_col];
    if (!is_undoing) {
        editor::action_history.push({ActionType::DELETE_CHAR, (int)pointed_row, (int)pointed_col, char_to_delete, ""});
    }

    buffer.delete_letter(pointed_row, pointed_col);
  }
}

void editor::modify::normal_delete_letter()
{
  if (buffer[pointed_row].length() == 0)
  {
    return;
  }

  status = Status::unsaved;

  if (buffer[pointed_row].length() == pointed_col)
  {
    editor::movement::move_left();
  }

  char char_to_delete = buffer[pointed_row][pointed_col];
  if (!is_undoing) {
      editor::action_history.push({ActionType::DELETE_CHAR, (int)pointed_row, (int)pointed_col, char_to_delete, ""});
  }

  buffer.delete_letter(pointed_row, pointed_col);
}

void editor::modify::tab()
{
  status = Status::unsaved;

  for (int i = 0; i < tab_size; i++)
  {
    // Use insert_letter to ensure the action is recorded
    editor::modify::insert_letter(' ');
  }
}

void editor::modify::delete_row()
{
  status = Status::unsaved;

  // Verifica se il buffer non è vuoto
  if (!buffer.is_void())
  {
    if (!is_undoing) {
        editor::action_history.push({ActionType::DELETE_ROW, (int)pointed_row, 0, 0, buffer.get_string_row(pointed_row)});
    }
    // Cancella la riga puntata
    buffer.del_row(pointed_row);
  }

  // Gestisci la posizione del cursore
  if (buffer.is_void())
  {
    // Se il buffer è vuoto, reinizializzalo con una riga vuota e reimposta il cursore
    editor::movement::move2X(0);             // Reimposta il cursore all'inizio della riga
    cursor.setY(0);
    pointed_row = 0;             // Reimposta la riga puntata a 0
    starting_row = 0;            // Reimposta lo scrolling
  }
  else
  {
    editor::movement::move_up();
    editor::movement::move2X(buffer[pointed_row].length());       // Posiziona alla fine della riga precedentess
  }
}


void editor::modify::paste()
{
  if (copy_paste_buffer.length() > 0)
  {
    status = Status::unsaved;

    // Itera su ogni carattere della stringa
    for (char c : copy_paste_buffer)
    {
      if (c == '\n')
      {
        // Inserisci newline e sposta alla riga successiva
        editor::modify::new_line();
      }
      else
      {
        // Inserisci il carattere e sposta la posizione
        editor::modify::insert_letter(c);
      }
    }
  }
}

void editor::modify::paste_in_visual()
{
  if (copy_paste_buffer.empty() || mode != Mode::visual)
  {
    return;
  }
  
  status = Status::unsaved;

  // Preserve the text you want to paste
  std::string text_to_paste = copy_paste_buffer;

  // Delete the current selection (this switches mode to Normal)
  editor::visual::delete_highlighted();

  // Restore the original clipboard content
  copy_paste_buffer = text_to_paste;

  // Paste the text
  editor::modify::paste();

  // Ensure we are in normal mode
  editor::system::change2normal();
}

void editor::modify::replace()
{
  std::string replace_term = editor::system::text_form("Replace with: ");
  if (replace_term.empty()) {
      return;
  }

  int replace_len = replace_term.length();


  // Replace occurrences
  for (auto& occ : found_occurrences)
  {
    int row = occ.first;
    int col = occ.second;

    // Replace the word in the buffer at the found position
    buffer[row].replace(col, current_searched_word_length, replace_term);

    // Update occurrences in the same row after replacement
    if (replace_len != current_searched_word_length)
    {
      int shift = replace_len - current_searched_word_length;
      for (auto& occ_update : found_occurrences)
      {
        if (occ_update.first == row && occ_update.second > col)
        {
          occ_update.second += shift;
        }
      }
    }
  }

  editor::system::change2normal();
}

void editor::modify::delete_selection(int start_row, int end_row, int start_col, int end_col)
{
  status = Status::unsaved;

  copy_paste_buffer.clear();    // Clear the buffer before copying the highlighted text

  // Case 1: Highlight is within a single row
  if (start_row == end_row)
  {
    // The copying and deletion process must occur from left to right
    int copy_start = std::min(start_col, end_col);
    int num_chars_to_copy = std::min(abs(end_col - start_col) + 1, (int) buffer[start_row].length());     // Number of characters to copy and delete

    // Copy and delete the text
    copy_paste_buffer = buffer.slice_row(start_row, copy_start, copy_start + num_chars_to_copy);

    editor::movement::move2X(copy_start);      // Set the cursor to the start of the highlighted text

    return;

  }
  // Case 2: Highlight spans multiple rows

  if (end_row < start_row)
  {
    std::swap(start_row, end_row);
    std::swap(start_col, end_col);
  }

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

  // Maintain the current selected word at the center of the screen
  if (pointed_row > max_row / 2)
  {
    starting_row = pointed_row - max_row / 2;
  }
  else
  {
    starting_row = 0;
  }

  editor::movement::move2X(start_col);

  cursor.setY(start_row - starting_row);
}

void editor::modify::delete_word_backyard()
{
  if (pointed_col == 0)
  {
    return;
  }

  status = Status::unsaved;

  char curr_char_pointed = buffer[pointed_row][pointed_col - 1];

  // erase all spaces
  while (curr_char_pointed == ' ')
  {
    editor::modify::delete_letter();
    if (pointed_col == 0)
    {
      return;
    }
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
  if (pointed_col == row_length)
  {
    return;
  }

  status = Status::unsaved;
  char curr_char_pointed = buffer[pointed_row][pointed_col + 1];

  // erase all chars
  while (curr_char_pointed != ' ')
  {
    editor::modify::normal_delete_letter();
    row_length--;
    if (pointed_col == row_length)
    {
      return;
    }
    curr_char_pointed = buffer[pointed_row][pointed_col + 1];
  }

  while (curr_char_pointed == ' ' && pointed_col < row_length)
  {
    editor::modify::normal_delete_letter();
    curr_char_pointed = buffer[pointed_row][pointed_col + 1];
  }
}

//CURRENTLY UNSTABLE
void editor::modify::undo()
{
  if (!editor::action_history.empty())
  {
    is_undoing = true;
    status = Status::unsaved;
    Action last_action = editor::action_history.top();
    editor::action_history.pop();
    
    // Restore Cursor Position to where the action occurred
    pointed_row = last_action.row;
    pointed_col = last_action.col;
    
    // Ensure viewport follows cursor
    if (pointed_row > max_row / 2) starting_row = pointed_row - max_row / 2;
    else starting_row = 0;
    
    editor::movement::move2X(last_action.col);
    cursor.setY(pointed_row - starting_row);

    switch (last_action.type)
    {
    case ActionType::INSERT_CHAR:
    {
      // Inverse: Delete the character
      buffer.delete_letter(last_action.row, last_action.col);
      break;
    }
    case ActionType::DELETE_CHAR:
    {
      // Inverse: Re-insert the character
      buffer.insert_letter(last_action.row, last_action.col, last_action.letter);
      // Advance cursor to simulate restoration of the state before backspace
      pointed_col++;
      editor::movement::move2X(pointed_col);
      break;
    }
    case ActionType::INSERT_NEWLINE:
    {
      // Inverse: The user split the line, so we merge them back
      buffer.merge_rows(last_action.row, last_action.row + 1);
      break;
    }
    case ActionType::DELETE_NEWLINE:
    {
      // Inverse: The user merged lines (backspace at start), so we split them back
      // new_line() splits at current pointed_col
      editor::modify::new_line();
      break;
    }
    case ActionType::DELETE_ROW:
    {
      // Inverse: Restore the deleted row
      buffer.new_row(last_action.text, last_action.row);
      break;
    }
    default:
    {
      break;
    }
    }
    
    is_undoing = false;
  }
}