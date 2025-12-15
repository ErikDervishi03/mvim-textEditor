#include <cstdlib>
#include <ncurses.h>
#include <map>
#include <string>
#include <variant>

// Alias for window names to accept both string and integer types
using WindowName = std::variant<std::string, int>;

class WindowManager {
public:
    /**
     * @brief Provides access to the singleton instance of the class.
     *
     * This method ensures that only one instance of the class exists.
     * If the instance does not yet exist, it will be created.
     *
     * @return A reference to the singleton instance of the class.
     */
    static WindowManager& getInstance() {
        static WindowManager instance; // Guaranteed to be lazy-initialized and thread-safe
        return instance;
    }

    // Public interface remains the same
    WINDOW* get_window(const WindowName& name) {
        std::string key = to_string(name);
        auto it = windows.find(key);
        if (it != windows.end()) {
            return it->second;
        }
        return nullptr;  // Return nullptr if window doesn't exist
    }

    const std::map<std::string, WINDOW*>& get_windows() const {
        return windows;
    }

    int create_window(const WindowName& name) {
        std::string key = to_string(name);
        if (windows.find(key) != windows.end()) {
            return EXIT_FAILURE;
        }

        WINDOW* win = newwin(1, 1, 0, 0); // Temporary size
        keypad(win, TRUE);
        if (win == nullptr) {
            return EXIT_FAILURE;
        }

        windows[key] = win;  // Add the window to the map
        resize_windows();    // Resize all windows
        return EXIT_SUCCESS;
    }

    int kill_window(const WindowName& name) {
        return destroy_window(name);  // Alias for destroy_window
    }   


    // Helper methods
    void resize_windows() {
        int num_windows = windows.size();
        if (num_windows == 0) return;

        int maxHeight, maxWidth;
        getmaxyx(stdscr, maxHeight, maxWidth);

        // Reserve the last row for the status bar
        int effectiveHeight = maxHeight - 1;

        clear();
        // refresh(); // You can remove this early refresh as we will refresh at the end

        int win_width = (maxWidth - num_windows + 1) / num_windows;

        int i = 0;
        for (auto& pair : windows) {
            int starty = 0;
            int startx = i * (win_width + 1);

            // --- START CHANGE ---
            // Draw a vertical line to the left of the window (if it's not the first one)
            if (i > 0) {
                // Draw '|' on the background (stdscr) in the gap column
                // Limit the line to effectiveHeight so it doesn't cross the status bar
                mvvline(0, startx - 1, '|', effectiveHeight); 
            }
            // --- END CHANGE ---

            wclear(pair.second);
            wresize(pair.second, effectiveHeight, win_width); // Resize to effectiveHeight
            mvwin(pair.second, starty, startx);
            wrefresh(pair.second);
            i++;
        }
        
        // --- START CHANGE ---
        refresh(); // Refresh stdscr to show the vertical lines
        // --- END CHANGE ---
    }

private:
    // Private members
    std::map<std::string, WINDOW*> windows;

    // Private constructor and destructor for Singleton
    WindowManager() {
        initscr();
        cbreak();
        keypad(stdscr, TRUE);
        noecho();
        raw();
        refresh();
    }

    ~WindowManager() {
        for (auto& pair : windows) {
            delwin(pair.second);
        }
        endwin();
    }

    // Delete copy constructor and assignment operator
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;


    int destroy_window(const WindowName& name) {
        std::string key = to_string(name);
        auto it = windows.find(key);
        if (it == windows.end()) {
            return EXIT_FAILURE;
        }

        delwin(it->second);
        windows.erase(it);
        resize_windows();
        return EXIT_SUCCESS;
    }

    std::string to_string(const WindowName& name) {
        if (std::holds_alternative<std::string>(name)) {
            return std::get<std::string>(name);
        } else {
            return std::to_string(std::get<int>(name));
        }
    }
};