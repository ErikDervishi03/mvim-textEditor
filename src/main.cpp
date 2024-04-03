#include "ide.hpp"

/*
  if make give problem with dependencies, try to use the following command:
    make clean
  how to prove correctness of the text editor:
    a) write normal text
    b) check the right behaviour of the cursor
    c) check the right behaviour of the backspace
      c1) check the right behaviour of the backspace when the cursor is at the beginning of the row
      c2) check the right behaviour of the backspace when the cursor is in the row
    d) check the right behaviour of the enter
      d1) check the right behaviour of the enter when the cursor is at the beginning of the row
      d2) check the right behaviour of the enter when the cursor is in the row
    e) check the right behaviour of the tab
    f) check the right behaviour of the arrow keys
*/

int main(int argc, char* argv[]) {
    if (argc > 1) {
      Ide ide(argv[1]);
      ide.run();
    }else {
      Ide ide; 
      ide.run();
    }

    return 0;
}
