#pragma once

#include "cursor.hpp"
#include "buffer.hpp"
#include "mode.h"

/*

  mem.h

  All file can access the resources of the text editor through this file.

*/

extern Cursor cursor;
extern Buffer buffer;
extern size_t max_row;
extern size_t max_col;
extern size_t pointed_row;
extern size_t starting_row;

/*class Mem {
private:
  Mem() {}
  ~Mem() {}

  static Mem* instance;

public:
  static Mem* getInstance() {
      if (!instance) {
          instance = new Mem();
      }
      return instance;
  }

  Cursor cursor;
  Buffer buffer;
  size_t max_row;
  size_t max_col;
};

Mem* Mem::instance = nullptr;
*/

