#include "../include/action.hpp"

void action::movement::move_up()
{
  if (!(starting_row == 0 && pointed_row == 0))
  {
    std::string prev_row = buffer[pointed_row - 1];
    if (cursor.getY() > SCROLL_START_THRESHOLD || starting_row == 0)
    {
      cursor.move_up();
    }
    if (prev_row.length() < cursor.getX())
    {
      cursor.setX(prev_row.length());
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
    if (cursor.getX() >= next_row.length())
    {
      cursor.setX(next_row.length());
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
  if (cursor.getX() == 0 && cursor.getY() > 0)
  {
    cursor.set(buffer.get_string_row(pointed_row - 1).length(),
               cursor.getY() - 1);
    pointed_row--;
  }
  else if (cursor.getX() > 0)
  {
    cursor.move_left();
  }
  if (starting_row > 0 && pointed_row == starting_row)
  {
    starting_row--;
  }
}

void action::movement::move_right()
{
  if ((cursor.getX() <= buffer.get_string_row(pointed_row).length() - 1) &&
      !buffer.get_string_row(pointed_row).empty())
  {
    cursor.move_right();
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

  cursor.setX(0);
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
  cursor.setX(0);
  action::system::change2insert();
}

void action::movement::move_to_end_of_line()
{
  // Get the length of the current row
  const int curr_row_length = buffer.get_string_row(pointed_row).length();
  // Set the cursor to the end of the current line
  cursor.setX(curr_row_length);
}

void action::movement::move_to_beginning_of_line()
{
  std::string current_row = buffer.get_string_row(pointed_row);
  int row_length = current_row.length();
  int count = 0;

  while (count < row_length && current_row[count] == ' ')
  {
    count++;
  }

  if (count != row_length)
  {
    cursor.setX(count);
  }
  else
  {
    cursor.setX(0);
  }
}

void action::movement::move_to_next_word()
{
  std::string current_row = buffer[pointed_row];
  int row_length = current_row.length();

  // if we are in the middle of a word we go at the end of it
  while (current_row[cursor.getX()] != ' ')
  {
    action::movement::move_right();

    if (cursor.getX() >= row_length)
    {
      if (pointed_row < buffer.getSize() - 1)
      {
        action::movement::move_down();
        cursor.setX(0);
      }
      return;
    }
  }

  // lets go ahead until we find the next word
  while (current_row[cursor.getX()] == ' ')
  {
    action::movement::move_right();

    if (cursor.getX() >= row_length)
    {
      if (pointed_row < buffer.getSize() - 1)
      {
        action::movement::move_down();
        cursor.setX(0);
      }
      return;
    }
    else if (isalnum(current_row[cursor.getX()]))
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
  cursor.set(buffer.get_string_row(pointed_row).length(),
             pointed_row - starting_row);
}

void action::movement::move_to_beginning_of_file()
{
  starting_row = 0;
  pointed_row = 0;
  cursor.set(0, 0);
}
