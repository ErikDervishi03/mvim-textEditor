#include "../include/ide.hpp"

void redirect_output_to_null() {
    // Open /dev/null
    int dev_null = open("/dev/null", O_WRONLY);

    if (dev_null != -1) {
        // Redirect stdout to /dev/null
        dup2(dev_null, STDOUT_FILENO);
        // Redirect stderr to /dev/null
        dup2(dev_null, STDERR_FILENO);

        close(dev_null);  // Close the file descriptor, it's no longer needed
    }
}

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
