#include "../include/ide.hpp"

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
