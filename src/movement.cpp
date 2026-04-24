#include "../include/editor/movement.hpp"
#include "../include/editor/system.hpp"      // Needed for change2insert()
#include "../include/globals/mvimResources.h" // Needed for global cursor, buffer, max_row, etc.
#include "../include/utils.hpp"

#include <string>
#include <cctype>    // Needed for isalpha()
#include <algorithm> // Needed for std::max()

void editor::movement::move2Y(int y, bool center_view) {
    pointed_row = y;

    if(center_view) {
      if (pointed_row > max_row / 2)
      {
          starting_row = pointed_row - max_row / 2;
      }
      else
      {
          starting_row = 0;
      }
    }

    cursor.setY(y - starting_row);
}

void editor::movement::move2X(int x){
  pointed_col = x;

  if(x < starting_col || x > starting_col + (max_col - 1))
  {
    if(pointed_col > max_col/2) {
      starting_col = pointed_col - max_col/2;

      if (starting_col < buffer[pointed_row].length()) {
          while (starting_col > 0 && is_continuation(buffer[pointed_row][starting_col])) {
              starting_col--;
          }
      }
    }
    else {
      starting_col = 0;
    }
  }

  int visual_x = 0;
  std::string& row = buffer[pointed_row];
  for (int i = starting_col; i < pointed_col && i < row.length(); ++i) {
      if (!is_continuation(row[i])) {
          visual_x++;
      }
  }
  cursor.setX(visual_x);
}

void editor::movement::move_up()
{
  if (!(starting_row == 0 && pointed_row == 0))
  {
    std::string prev_row = buffer[pointed_row - 1];
    if (cursor.getY() > SCROLL_START_THRESHOLD || starting_row == 0)
    {
      cursor.move_up();
    }
    
    if (prev_row.length() <= pointed_col)
    {
      move2X(prev_row.length());
    }
    else 
    {
        while(pointed_col > 0 && is_continuation(prev_row[pointed_col])) {
            pointed_col--;
        }
        move2X(pointed_col);
    }

    if (starting_row > 0 &&
        pointed_row <= starting_row + SCROLL_START_THRESHOLD)
    {
      starting_row--;
    }
    pointed_row--;
  }
}

void editor::movement::move_down()
{
  if (pointed_row < buffer.getSize() - 1)
  {
    std::string next_row = buffer.get_string_row(pointed_row + 1);
    if (cursor.getY() < max_row - SCROLL_START_THRESHOLD - 1 ||
        pointed_row >= buffer.getSize() - SCROLL_START_THRESHOLD - 1)
    {

      cursor.move_down();
    }

    if (pointed_col >= next_row.length())
    {
      move2X(next_row.length());
    }
    else 
    {
        while (pointed_col > 0 && pointed_col < next_row.length() && is_continuation(next_row[pointed_col])) {
            pointed_col--;
        }
        move2X(pointed_col);
    }

    if (pointed_row - starting_row >= max_row - SCROLL_START_THRESHOLD - 1)
    {
       if (starting_row + max_row < buffer.getSize()) {
          starting_row++;
       }
    }
    pointed_row++;
  }
}

void editor::movement::move_left()
{
  if (pointed_col == 0 && cursor.getY() > 0)
  {
    pointed_row--;
    int currRowLen = buffer[pointed_row].length();
    move2X(currRowLen);
    cursor.setY(pointed_row - starting_row);
    return;
  }
  
  if (pointed_col > 0)
  {
    int bytes_to_jump = 1;
    while (pointed_col - bytes_to_jump > 0 && 
           is_continuation(buffer[pointed_row][pointed_col - bytes_to_jump])) 
    {
        bytes_to_jump++;
    }

    if (starting_col >= pointed_col) 
    {
        starting_col -= bytes_to_jump;
        if (starting_col < 0) starting_col = 0;
    }
    else 
    {
        cursor.move_left();
    }
    
    pointed_col -= bytes_to_jump;
  }

  if (starting_row > 0 && pointed_row == starting_row)
  {
    starting_row--;
  }
}

void editor::movement::move_right()
{
  std::string& row = buffer[pointed_row];
  
  if (pointed_col < row.length() && !row.empty())
  {
    int bytes_to_jump = 1;
    while (pointed_col + bytes_to_jump < row.length() && 
           is_continuation(row[pointed_col + bytes_to_jump])) 
    {
        bytes_to_jump++;
    }
    
    if(cursor.getX() >= max_col - span - 2) 
    {
        starting_col += bytes_to_jump;
    }
    else 
    {
        cursor.move_right();
    }

    pointed_col += bytes_to_jump;
  }
}

void editor::movement::go_down_creating_newline()
{
  buffer.new_row("", pointed_row + 1);
  editor::movement::move_down();
  editor::movement::move2X(0);
  editor::system::change2insert();
}

void editor::movement::go_up_creating_newline()
{
  if (pointed_row > 0)
  {
    buffer.new_row("", pointed_row);
  }
  else
  {
    buffer.new_row("", pointed_row + 1);
    buffer.swap_rows(pointed_row, pointed_row + 1);
  }
  move2X(0);
  editor::system::change2insert();
}

void editor::movement::move_to_end_of_line()
{
  editor::movement::move2X(buffer[pointed_row].length());
}

void editor::movement::move_to_beginning_of_line()
{
  editor::movement::move2X(0);
}

void editor::movement::move_to_next_word()
{
  int row_length = buffer[pointed_row].length();
  int target_col = pointed_col;

  // 1. Skip the current word (traverse letters forward)
  while (target_col < row_length && isalpha(buffer[pointed_row][target_col])) {
      target_col++;
  }

  // 2. Skip EVERYTHING that isn't a letter (spaces, symbols, punctuation)
  while (target_col < row_length && !isalpha(buffer[pointed_row][target_col])) {
      target_col++;
  }

  // 3. Jump to the new position or wrap to the next line
  if (target_col >= row_length) {
      if (pointed_row < buffer.getSize() - 1) {
          // Wrap to the beginning of the next line
          editor::movement::move_down();
          editor::movement::move2X(0);
      } else {
          // Reached the very end of the file
          editor::movement::move2X(row_length); 
      }
  } else {
      // Jump directly to the start of the next word
      editor::movement::move2X(target_col);
  }
}

void editor::movement::move_to_previous_word()
{
  // Safety check: already at the beginning of the file
  if (pointed_row == 0 && pointed_col == 0) return;

  int target_row = pointed_row;
  int target_col = pointed_col - 1;

  // 1. Skip non-letters backwards (spaces, symbols, punctuation) 
  // and handle line wrapping safely
  while (true)
  {
    if (target_col < 0) 
    {
      if (target_row == 0)
      {
        // Reached the very beginning of the file while scanning
        editor::movement::move2Y(0);
        editor::movement::move2X(0);
        return;
      }
      // Wrap to the end of the previous line
      target_row--;
      target_col = buffer[target_row].length() - 1;
      continue;
    }

    if (isalpha(buffer[target_row][target_col]))
    {
      break; // Found the end of the previous actual word
    }
    target_col--;
  }

  // 2. Skip letters backwards to find the exact start of the word
  while (target_col >= 0 && isalpha(buffer[target_row][target_col]))
  {
    target_col--;
  }

  // target_col is now pointing to the character right before the word.
  // Add 1 to point to the first letter of the word.
  target_col++;

  // 3. Jump directly to the calculated coordinates
  if (target_row != pointed_row)
  {
    editor::movement::move2Y(target_row);
  }
  
  editor::movement::move2X(target_col);
}

void editor::movement::move_to_end_of_file()
{
  pointed_row = buffer.getSize() - 1;
  starting_row = std::max((int) (buffer.getSize() - max_row), 0);
  move2X(buffer[pointed_row].length());
  cursor.setY(pointed_row - starting_row);
}

void editor::movement::move_to_beginning_of_file()
{
  editor::movement::move2Y(0, true); 
  editor::movement::move2X(0);
}
