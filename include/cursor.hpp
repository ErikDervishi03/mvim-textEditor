#pragma once
#include <ncurses.h>

class Cursor{
  private:
    int x;
    int y;
    void update_cursor();
  public:
    Cursor();
    int getX() const;
    int getY() const;
    void move_right();
    void move_left();
    void move_up();
    void move_down();
    void set(int x, int y);
    void setX(int x);
    void setY(int y);
    void restore(int span);
};


