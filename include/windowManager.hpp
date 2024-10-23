#include <cstdlib>
#include <ncurses.h>
#include <map>
#include <string>
#include <iostream>
#include <variant>  

// Alias for window names to accept both string and integer types
using WindowName = std::variant<std::string, int>;

/**
 * @class WindowManager
 * @brief Manages ncurses windows, including creation, destruction, and resizing.
 *
 * This class provides an interface for managing multiple windows in an ncurses
 * environment. Windows can be created, resized, and destroyed. The class
 * automatically adjusts the size of windows to fit the screen.
 */
class WindowManager {
public:
    /**
     * @brief Constructor that initializes the ncurses environment.
     *
     * Initializes the ncurses mode, disables line buffering, enables keypad mode, and
     * suppresses input echo. Refreshes the standard screen to ensure it's ready for use.
     */
    WindowManager() {
        initscr();             // Initialize ncurses mode
        cbreak();              // Disable line buffering
        keypad(stdscr, TRUE);  // Enable keypad
        noecho();              // Don't echo input
        refresh();             // Refresh the standard screen
    }

    /**
     * @brief Destructor that cleans up ncurses windows and ends ncurses mode.
     *
     * Deletes all windows stored in the internal map and ends the ncurses mode to restore
     * the terminal to its normal state.
     */
    ~WindowManager() {
        for (auto& pair : windows) {
            delwin(pair.second);
        }
        endwin();  // End ncurses mode
    }

    /**
     * @brief Converts a WindowName (variant) to a string representation.
     * 
     * @param name The WindowName (either a string or an integer).
     * @return A string representation of the WindowName.
     */
    std::string to_string(const WindowName& name) {
        if (std::holds_alternative<std::string>(name)) {
            return std::get<std::string>(name);
        } else {
            return std::to_string(std::get<int>(name));
        }
    }

    /**
     * @brief Returns a pointer to the ncurses window corresponding to the given name.
     *
     * @param name The name of the window, either as a string or integer.
     * @return A pointer to the ncurses window, or nullptr if the window doesn't exist.
     */
    WINDOW* get_window(const WindowName& name) {
        std::string key = to_string(name);
        auto it = windows.find(key);
        if (it != windows.end()) {
            return it->second;
        }
        return nullptr;  // Return nullptr if window doesn't exist
    }

    /**
     * @brief Creates a new ncurses window and resizes all existing windows to fit the screen.
     *
     * If a window with the same name already exists, the method returns EXIT_FAILURE
     * without creating a new window. The new window is initially created with a temporary
     * size and is then resized along with all other windows.
     * 
     * @param name The name of the window, which can be specified as either a string 
     *             or an integer. This name is used as a key in the internal window map.
     * @return Returns EXIT_SUCCESS if the window was created successfully; 
     *         otherwise, it returns EXIT_FAILURE if a window with the same name 
     *         already exists or if window creation fails.
     */
    int create_window(const WindowName& name) {
        std::string key = to_string(name);
        if (windows.find(key) != windows.end()) {
            return EXIT_FAILURE;
        }

        // Create a temporary window before setting its position and size
        WINDOW* win = newwin(1, 1, 0, 0);  // Temporary size
        if (win == nullptr) {
            return EXIT_FAILURE;
        }

        windows[key] = win;  // Add the window to the map
        resize_windows();    // Resize all windows
        return EXIT_SUCCESS;
    }


    /**
     * @brief Alias for destroy_window.
     *
     * Destroys the window with the given name and resizes the remaining windows.
     * 
     * @param name The name of the window, either as a string or integer.
     * @return EXIT_SUCCESS if the window was successfully destroyed, otherwise EXIT_FAILURE.
     */
    int kill_window(const WindowName& name) {
        return destroy_window(name);  // Alias for destroy_window
    }

private:
    std::map<std::string, WINDOW*> windows;  // Map of window names to ncurses windows

    /**
     * @brief Resizes all windows to evenly distribute them across the screen.
     *
     * The function calculates the width of each window and positions them
     * side-by-side, ensuring that the entire screen is used.
     */
    void resize_windows() {
        int num_windows = windows.size();
        if (num_windows == 0) return;

        int maxHeight, maxWidth;
        getmaxyx(stdscr, maxHeight, maxWidth);  // Get screen dimensions

        // Clear the entire screen before resizing
        clear();  // Clears stdscr
        refresh();  // Refresh to apply the clearing

        int win_width = (maxWidth - num_windows + 1) / num_windows;  // Adjust window width for borders

        int i = 0;
        for (auto& pair : windows) {
            int starty = 0;
            int startx = i * (win_width + 1);  // Calculate start position considering the border
            wclear(pair.second);
            wresize(pair.second, maxHeight, win_width);  // Resize window
            mvwin(pair.second, starty, startx);       // Move window to correct position
            if (i < windows.size() - 1)
                wborder(pair.second, ' ', '|', ' ', ' ', ' ', ' ', ' ', ' ');
            wrefresh(pair.second);                    // Refresh the window to show it
            i++;
        }
    }

    /**
     * @brief Destroys a window and resizes the remaining windows.
     *
     * If the window doesn't exist, it returns EXIT_FAILURE.
     * 
     * @param name The name of the window, either as a string or integer.
     * @return EXIT_SUCCESS if the window was successfully destroyed, otherwise EXIT_FAILURE.
     */
    int destroy_window(const WindowName& name) {
        std::string key = to_string(name);
        auto it = windows.find(key);
        if (it == windows.end()) {
            return EXIT_FAILURE;
        }

        delwin(it->second);   // Delete the window
        windows.erase(it);    // Remove it from the map
        resize_windows();     // Resize the remaining windows
        return EXIT_SUCCESS;
    }
};
