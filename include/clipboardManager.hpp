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
        // Write to a temporary file first to avoid shell quoting issues
        std::ofstream clipboardFile("/tmp/mvim_clipboard_buffer.txt");
        if (clipboardFile.is_open()) {
            clipboardFile << text;
            clipboardFile.close();
            // Pipe the file into xclip's clipboard selection
            std::system("xclip -selection clipboard -i < /tmp/mvim_clipboard_buffer.txt");
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