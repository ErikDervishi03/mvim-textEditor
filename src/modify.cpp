#include "../include/action.hpp"
#include <fcntl.h>
#include <ncurses.h>

void action::modify::insert_letter(int letter)
{
  status = Status::unsaved;

  if (cursor.getX() == max_col - 7)
  {
    buffer.new_row("", pointed_row + 1);
    if (cursor.getY() >= max_row - SCROLL_START_THRESHOLD - 1 &&
        !buffer.is_void_row(max_row) &&
        pointed_row < buffer.getSize())
    {

      starting_row++;
    }
    else if (cursor.getY() < max_row - 1)
    {
      cursor.move_down();
    }

    cursor.setX(0);
    pointed_row++;
  }
  buffer.insert_letter(pointed_row, cursor.getX(), letter);
  cursor.move_right();

  action_history.push({ ActionType::INSERT, static_cast<int>(pointed_row),
                        cursor.getX(), static_cast<char>(letter), "" });
}

void action::modify::new_line()
{
  status = Status::unsaved;
  buffer.new_row("", pointed_row + 1);

  // here we are breaking down the line.
  // All the text from the current cursor
  // position to the end of the line go in
  // the next line

  const int curr_row_length = buffer.get_string_row(pointed_row).length();
  if (cursor.getX() != curr_row_length)
  {
    std::string line_break = buffer.slice_row(pointed_row, cursor.getX(), curr_row_length);
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

  cursor.setX(0);
  pointed_row++;
}


void action::modify::delete_letter()
{

  status = Status::unsaved;

  if (cursor.getX() == 0 && (cursor.getY() > 0 || starting_row > 0))
  {
    action::movement::move_up();
    cursor.setX(buffer[pointed_row - 1].length());
    buffer.merge_rows(pointed_row - 1, pointed_row);
    pointed_row--;
  }
  else if (cursor.getX() > 0)
  {
    action::movement::move_left();
    buffer.delete_letter(pointed_row, cursor.getX());
  }
}

void action::modify::normal_delete_letter()
{
  if (buffer[pointed_row].length() == 0)
  {
    return;
  }

  status = Status::unsaved;

  if (buffer[pointed_row].length() == cursor.getX())
  {
    action::movement::move_left();
  }
  buffer.delete_letter(pointed_row, cursor.getX());

}

void action::modify::tab()
{
  status = Status::unsaved;

  for (int i = 0; i < tab_size; i++)
  {
    buffer.insert_letter(pointed_row, cursor.getX(), ' ');
    cursor.move_right();
  }
}

void action::modify::delete_row()
{
  status = Status::unsaved;

  // Verifica se il buffer non è vuoto
  if (!buffer.is_void())
  {
    // Cancella la riga puntata
    buffer.del_row(pointed_row);
  }

  // Gestisci la posizione del cursore
  if (buffer.is_void())
  {
    // Se il buffer è vuoto, reinizializzalo con una riga vuota e reimposta il cursore
    cursor.setX(0);              // Reimposta il cursore all'inizio della riga
    cursor.setY(0);
    pointed_row = 0;             // Reimposta la riga puntata a 0
    starting_row = 0;            // Reimposta lo scrolling
  }
  else
  {
    action::movement::move_up();
    cursor.setX(buffer.get_string_row(pointed_row).length());       // Posiziona alla fine della riga precedentess
  }
}


void action::modify::paste()
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
        action::modify::new_line();
      }
      else
      {
        // Inserisci il carattere e sposta la posizione
        action::modify::insert_letter(c);
      }
    }
  }
}

void action::modify::replace()
{
  char* replace_term = action::system::text_form("Replace with: ");
  if (replace_term == NULL || strlen(replace_term) == 0)
  {
    free(replace_term);
    return;
  }

  status = Status::unsaved;
  int replace_len = strlen(replace_term);

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

  action::system::change2normal();

  // Clean up
  free(replace_term);
}

void action::modify::delete_selection(int start_row, int end_row, int start_col, int end_col)
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

    cursor.setX(copy_start);      // Set the cursor to the start of the highlighted text

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

  cursor.set(start_col, start_row - starting_row);
}


static void reverse_insert(int row, int col)
{
  // Set the cursor to the position of the last action
  cursor.set(col + 1, row);
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
}

void action::modify::delete_word_backyard()
{
  if (cursor.getX() == 0)
  {
    return;
  }

  status = Status::unsaved;

  char curr_char_pointed = buffer[pointed_row][cursor.getX() - 1];

  // erase all spaces
  while (curr_char_pointed == ' ')
  {
    action::modify::delete_letter();
    if (cursor.getX() == 0)
    {
      return;
    }
    curr_char_pointed = buffer[pointed_row][cursor.getX() - 1];
  }

  while (curr_char_pointed != ' ' && cursor.getX() > 0)
  {
    action::modify::delete_letter();
    curr_char_pointed = buffer[pointed_row][cursor.getX() - 1];
  }
}

void action::modify::delete_word()
{
  int row_length = buffer[pointed_row].length();
  if (cursor.getX() == row_length)
  {
    return;
  }

  status = Status::unsaved;
  char curr_char_pointed = buffer[pointed_row][cursor.getX() + 1];

  // erase all chars
  while (curr_char_pointed != ' ')
  {
    action::modify::normal_delete_letter();
    row_length--;
    if (cursor.getX() == row_length)
    {
      return;
    }
    curr_char_pointed = buffer[pointed_row][cursor.getX() + 1];
  }

  while (curr_char_pointed == ' ' && cursor.getX() < row_length)
  {
    action::modify::normal_delete_letter();
    curr_char_pointed = buffer[pointed_row][cursor.getX() + 1];
  }
}

void action::modify::undo()
{

  if (!action::action_history.empty())
  {
    status = Status::unsaved;
    Action last_action = action::action_history.top();
    action::action_history.pop();

    switch (last_action.type)
    {
    case ActionType::INSERT:
    {
      reverse_insert(last_action.row, last_action.col);

      while (!action::action_history.empty() &&
             action::action_history.top().type == ActionType::INSERT)
      {
        // Retrieve the last action
        Action last_action = action::action_history.top();

        reverse_insert(last_action.row, last_action.col);

        // Undo the insert action by deleting the letter
        action::modify::delete_letter();

        // Pop the action from the history stack
        action::action_history.pop();
      }

      break;
    }
    default:
    {
      break;
    }
    }
  }
}
