#include "../include/cursor.hpp"

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
  move(y, x);
}

void Cursor::restore(int span = 0)
{
  move(y, x + span + 1);
}
