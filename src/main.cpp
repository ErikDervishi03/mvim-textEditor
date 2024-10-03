#include <iostream>
#include <cstring>  // Include for std::strcmp
#include "../include/ide.hpp"  // Include your Ide class header

int main(int argc, char* argv[]) {
    const char* filename = nullptr;  // To hold the filename
    bool benchmark = false;           // Flag for benchmarking mode

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        if (filename == nullptr) {
            // The first non-option argument should be the filename
            filename = argv[i];
        } else if (std::strcmp(argv[i], "-b") == 0) {
            benchmark = true;  // Enable benchmarking
        }
    }

    // Check if a filename was provided
    if (!filename) {
        std::cerr << "Usage: " << argv[0] << " <filename> [-b]" << std::endl;
        return 1;  // Exit if no filename is provided
    }

    // Create an instance of Ide, passing the filename and benchmark flag
    Ide ide(filename, benchmark);

    // Run the IDE (this will only happen if not in benchmark mode)
    if (!benchmark) {
        ide.run();
    }

    return 0;  // Exit the program successfully
}
