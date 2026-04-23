#include "../include/editor.hpp"

static bool is_continuation(char c) {
    return (c & 0xC0) == 0x80;
}

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
  
  if (cursor.getY() >= max_row - SCROLL_START_THRESHOLD - 1 &&
      !buffer.is_void_row(max_row) && pointed_row < buffer.getSize())
  {
    starting_row++;
  }
  else if (cursor.getY() < max_row - 1)
  {
    cursor.move_down();
  }

  move2X(0);
  pointed_row++;

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
  const int currRowLen = buffer[pointed_row].length();
  move2X(currRowLen);
  cursor.setY(pointed_row - starting_row);
}

void editor::movement::move_to_beginning_of_line()
{
  std::string current_row = buffer[pointed_row];
  int row_length = current_row.length();
  int count = 0;

  while (count < row_length && current_row[count] == ' ')
  {
    count++;
  }

  if (count != row_length)
  {
    move2X(count);
  }
  else
  {
    move2X(0);
  }
}

void editor::movement::move_to_next_word()
{
  if (pointed_row >= buffer.getSize()) return;

  std::string current_row = buffer[pointed_row];
  int row_length = current_row.length();

  while (pointed_col < row_length && current_row[pointed_col] != ' ')
  {
    editor::movement::move_right();
    if (pointed_col >= buffer[pointed_row].length())
    {
      if (pointed_row < buffer.getSize() - 1)
      {
        editor::movement::move_down();
        move2X(0);
      }
      return;
    }
  }

  while (pointed_col < buffer[pointed_row].length() && buffer[pointed_row][pointed_col] == ' ')
  {
    editor::movement::move_right();
    
    if (pointed_col >= buffer[pointed_row].length())
    {
      if (pointed_row < buffer.getSize() - 1)
      {
        editor::movement::move_down();
        move2X(0);
      }
      return;
    }
    else if (isalnum(buffer[pointed_row][pointed_col]))
    {
      return;
    }
  }
}

void editor::movement::move_to_previous_word()
{
  if (pointed_row == 0 && pointed_col == 0) return;

  editor::movement::move_left();

  while (true) 
  {
      if (pointed_row < 0) return;
      
      char c = ' ';
      if (pointed_col < buffer[pointed_row].length()) {
          c = buffer[pointed_row][pointed_col];
      }
      
      if (c != ' ') break; 
      
      if (pointed_row == 0 && pointed_col == 0) return;
      
      editor::movement::move_left();
  }

  while (true) 
  {
      if (pointed_col == 0) return; 
      
      int prev_idx = pointed_col - 1;
      while (prev_idx > 0 && is_continuation(buffer[pointed_row][prev_idx])) {
          prev_idx--;
      }
      
      char prev_c = ' ';
      if (prev_idx < buffer[pointed_row].length()) {
          prev_c = buffer[pointed_row][prev_idx];
      }
      
      if (prev_c == ' ') break; 
      
      editor::movement::move_left();
  }
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
  starting_row = pointed_row = 0;
  starting_col = pointed_col = 0;
  cursor.set(0, 0);
}
