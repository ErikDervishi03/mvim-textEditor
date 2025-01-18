#include "../include/cursor.hpp"
#include <ncurses.h>

Cursor::Cursor() :
  x(0), y(0)
{

}

int Cursor::getX() const
{
  return x;
}

int Cursor::getY() const
{
  return y;
}

void Cursor::move_right()
{
  this->x += 1;
}

void Cursor::move_left()
{
  this->x -= 1;

}

void Cursor::move_up()
{
  this->y -= 1;

}

void Cursor::move_down()
{
  this->y += 1;
}

void Cursor::set(int x, int y)
{
  this->x = x;
  this->y = y;
}

void Cursor::setX(int x)
{
  this->x = x;

}

void Cursor::setY(int y)
{
  this->y = y;
}

void Cursor::update_cursor()
{
    // Move the cursor within the pointed window
    wmove(pointed_window, y, x);  // Move cursor to (y, x) in pointed_window
}

void Cursor::restore(int span)
{
    // Restore the cursor position within the pointed window
    wmove(pointed_window, y, x + span + 1);  // Restore at (y, x + span + 1) in pointed_window
}

void Cursor::pointToWindow(WINDOW* window){
    pointed_window = window;
}