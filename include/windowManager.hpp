#include <ncurses.h>
#include <map>
#include <string>
#include <iostream>
#include <variant>  

// Alias for window names to accept both string and integer types
using WindowName = std::variant<std::string, int>;

class WindowManager {
public:
    WindowManager() {
        initscr();             // Initialize ncurses mode
        cbreak();              // Disable line buffering
        keypad(stdscr, TRUE);  // Enable keypad
        noecho();              // Don't echo input
        refresh();             // Refresh the standard screen
    }

    ~WindowManager() {
        for (auto& pair : windows) {
            delwin(pair.second);
        }
        endwin();  // End ncurses mode
    }

    // Helper function to convert WindowName to string (for map key)
    std::string to_string(const WindowName& name) {
        if (std::holds_alternative<std::string>(name)) {
            return std::get<std::string>(name);
        } else {
            return std::to_string(std::get<int>(name));
        }
    }

    // Create a new window, auto-resizes based on number of windows
    void create_window(const WindowName& name) {
        std::string key = to_string(name);
        if (windows.find(key) != windows.end()) {
            std::cerr << "Window with name '" << key << "' already exists." << std::endl;
            return;
        }

        // Create a temporary window before setting its position and size
        WINDOW* win = newwin(1, 1, 0, 0);  // Temporary size
        if (win == nullptr) {
            std::cerr << "Failed to create window." << std::endl;
            return;
        }

        windows[key] = win;  // Add the window to the map
        resize_windows();    // Resize all windows
    }

    // Destroy a window by its name and resize the remaining windows
    void destroy_window(const WindowName& name) {
        std::string key = to_string(name);
        auto it = windows.find(key);
        if (it == windows.end()) {
            std::cerr << "Window with name '" << key << "' does not exist." << std::endl;
            return;
        }
        delwin(it->second);   // Delete the window
        windows.erase(it);    // Remove it from the map
        resize_windows();     // Resize the remaining windows
    }

    void resize_windows() {
        int num_windows = windows.size();
        if (num_windows == 0) return;

        int height, width;
        getmaxyx(stdscr, height, width);  // Get screen dimensions

        // Clear the entire screen before resizing
        clear();  // Clears stdscr
        refresh();  // Refresh to apply the clearing

        int win_width = (width - num_windows + 1) / num_windows;  // Adjust window width for borders

        int i = 0;
        for (auto& pair : windows) {
            int starty = 0;
            int startx = i * (win_width + 1);  // Calculate start position considering the border
            wclear(pair.second);
            wresize(pair.second, height, win_width);  // Resize window
            mvwin(pair.second, starty, startx);       // Move window to correct position
            box(pair.second, 0, 0);                   // Add a border around the window
            wrefresh(pair.second);                    // Refresh the window to show it
            i++;
        }
    }

    // Kill a window by its name
    void kill_window(const WindowName& name) {
        destroy_window(name);  // Alias for destroy_window
    }

    // Display a message inside a window
    void display_message(const WindowName& name, const std::string& message) {
        std::string key = to_string(name);
        auto it = windows.find(key);
        if (it == windows.end()) {
            std::cerr << "Window with name '" << key << "' does not exist." << std::endl;
            return;
        }

        WINDOW* win = it->second;
        wclear(win);  // Clear the window
        box(win, 0, 0);  // Re-draw the border
        mvwprintw(win, 1, 1, "%s", message.c_str());  // Display the message
        wrefresh(win);  // Refresh the window to show the message
    }

    // Return a window pointer given a window's name
    WINDOW* get_window(const WindowName& name) {
        std::string key = to_string(name);
        auto it = windows.find(key);
        if (it != windows.end()) {
            return it->second;
        }
        return nullptr;  // Return nullptr if window doesn't exist
    }

    // Toggle the window border given the name
    void toggle_border(const WindowName& name, bool enable) {
        WINDOW* win = get_window(name);
        if (win == nullptr) {
            std::cerr << "Window with name '" << to_string(name) << "' does not exist." << std::endl;
            return;
        }
        toggle_border(win, enable);  // Call the other version with window pointer
    }

    // Toggle the window border given a window pointer
    void toggle_border(WINDOW* win, bool enable) {
        if (win == nullptr) {
            std::cerr << "Invalid window pointer." << std::endl;
            return;
        }

        if (enable) {
            box(win, 0, 0);  // Add border
        } else {
            wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');  // Remove border
        }
        wrefresh(win);  // Refresh the window to show the change
    }

private:
    std::map<std::string, WINDOW*> windows;  // Map of window names to ncurses windows
};

/*
    ====example====
    
int main() {
    WindowManager wm;

    // Create windows using both string and integer names
    wm.create_window("left");
    wm.create_window(2);

    // Display some messages
    wm.display_message("left", "Left window");
    wm.display_message(2, "Window 2");

    // Wait for user input
    getch();

    // Kill a window
    wm.kill_window("left");

    // Wait for user input before exiting
    getch();

    // Create new windows
    wm.create_window(3);
    wm.create_window("right");

    // Wait for user input before exiting
    getch();

    return 0;
}
*/