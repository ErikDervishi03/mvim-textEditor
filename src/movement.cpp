#include "../include/action.hpp"

void action::movement::move2X(int x){
  pointed_col = x;

  // if this condition is true means that the new position is out of the 
  // visible view of the user
  if(x < starting_col || x > starting_col + (max_col - 1))
  {
    if(pointed_col > max_col/2)
      starting_col = pointed_col - max_col/2;
    else
      starting_col = 0;
  }

  cursor.setX(pointed_col - starting_col);
}

void action::movement::move_up()
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
    if (starting_row > 0 &&
        pointed_row <= starting_row + SCROLL_START_THRESHOLD)
    {
      starting_row--;
    }
    pointed_row--;
  }
}

void action::movement::move_down()
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

    if (pointed_row - starting_row == max_row - SCROLL_START_THRESHOLD - 1 &&
        pointed_row < buffer.getSize() - SCROLL_START_THRESHOLD - 1)
    {

      starting_row++;
    }
    pointed_row++;
  }
}

void action::movement::move_left()
{
  if (pointed_col == 0 && cursor.getY() > 0)
  {
    pointed_row--;

    int currRowLen = buffer[pointed_row].length();

    move2X(currRowLen);

    cursor.setY(pointed_row - starting_row);
    system::print_to_terminal(cursor.getX());
    
  }
  else if (pointed_col > 0)
  {
    if(starting_col == pointed_col) starting_col--;
    else cursor.move_left();
    pointed_col--;
  }

  if (starting_row > 0 && pointed_row == starting_row)
  {
    starting_row--;
  }
}

void action::movement::move_right()
{
  if ((cursor.getX() <= (buffer[pointed_row].length() - 1 - starting_col)) &&
      !buffer[pointed_row].empty())
  {
    if(cursor.getX() == max_col - span - 2) starting_col++;
    else cursor.move_right();

    pointed_col++;
  }
}

void action::movement::go_down_creating_newline()
{
  // Create a new line below the current pointed_row
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

  action::system::change2insert();
}

void action::movement::go_up_creating_newline()
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
  action::system::change2insert();
}

void action::movement::move_to_end_of_line()
{
  const int currRowLen = buffer[pointed_row].length();

  move2X(currRowLen);

  cursor.setY(pointed_row - starting_row);
}

void action::movement::move_to_beginning_of_line()
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

void action::movement::move_to_next_word()
{
  std::string current_row = buffer[pointed_row];
  int row_length = current_row.length();

  // if we are in the middle of a word we go at the end of it
  while (current_row[pointed_col] != ' ')
  {
    action::movement::move_right();

    if (pointed_col >= row_length)
    {
      if (pointed_row < buffer.getSize() - 1)
      {
        action::movement::move_down();
        move2X(0);
      }
      return;
    }
  }

  // lets go ahead until we find the next word
  while (current_row[pointed_col] == ' ')
  {
    action::movement::move_right();

    if (pointed_col >= row_length)
    {
      if (pointed_row < buffer.getSize() - 1)
      {
        action::movement::move_down();
        move2X(0);
      }
      return;
    }
    else if (isalnum(current_row[pointed_col]))
    {
      return;
    }
  }
}

void action::movement::move_to_end_of_file()
{
  // point to the last row
  pointed_row = buffer.getSize() - 1;

  starting_row = std::max((int) (buffer.getSize() - max_row), 0);
  move2X(buffer[pointed_row].length());
  cursor.setY(pointed_row - starting_row);
}

void action::movement::move_to_beginning_of_file()
{
  starting_row = pointed_row = 0;
  starting_col = pointed_col = 0;
  cursor.set(0, 0);
}
