#pragma once
#include <string>
#include <fstream>
#include <cstdio>
#include <memory>
#include <array>
#include <cstdlib>

class ClipboardManager {
public:

    // Pushes text to the System Clipboard
    static void setSystemClipboard(const std::string& text) {
        // Open a pipe to write directly to xclip's standard input
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("xclip -selection clipboard -i 2>/dev/null", "w"), pclose);
        
        if (pipe) {
            // Write the copied text directly to the process
            fwrite(text.c_str(), 1, text.length(), pipe.get());
        }
    }

    // Pulls text from the System Clipboard
    static std::string getSystemClipboard() {
        std::string result;
        std::array<char, 128> buffer;
        
        // Open a pipe to read from xclip
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("xclip -selection clipboard -o 2>/dev/null", "r"), pclose);
        
        if (!pipe) {
            return ""; // Failed to open pipe
        }

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        
        return result;
    }
};