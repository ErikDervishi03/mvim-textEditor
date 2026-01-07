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


    void refresh_separators() {
        int num_windows = windows.size();
        if (num_windows < 2) return;

        int maxHeight, maxWidth;
        getmaxyx(stdscr, maxHeight, maxWidth);
        int effectiveHeight = maxHeight - 1;

        // 1. Pulisce lo sfondo per rimuovere artefatti grafici
        // Questo è il segreto: simula il "ritorno dal menu"
        werase(stdscr); 

        int win_width = (maxWidth - (num_windows - 1)) / num_windows;
        int remainder = (maxWidth - (num_windows - 1)) % num_windows;

        int current_x = 0;
        int i = 0;

        for (auto& pair : windows) {
            int current_win_width = win_width + (i < remainder ? 1 : 0);

            if (i > 0) {
                // Ridisegna la linea nel punto esatto
                mvvline(0, current_x - 1, ACS_VLINE, effectiveHeight);
            }

            current_x += current_win_width + 1;
            i++;
        }

        // 2. Prepara lo sfondo per il rendering
        wnoutrefresh(stdscr);
    }


    // Helper methods
    void resize_windows() {
        int num_windows = windows.size();
        if (num_windows == 0) return;

        int maxHeight, maxWidth;
        getmaxyx(stdscr, maxHeight, maxWidth);

        // Reserve the last row for the status bar
        int effectiveHeight = maxHeight - 1;

        // 1. CLEAR STD SCR FIRST
        // Remove old artifacts/lines before calculating new positions
        wclear(stdscr); 

        int win_width = (maxWidth - (num_windows - 1)) / num_windows; // Subtract space for separators
        int remainder = (maxWidth - (num_windows - 1)) % num_windows;
        
        // --- PASS 1: RESIZE AND MOVE WINDOWS ---
        int current_x = 0;
        int i = 0;
        for (auto& pair : windows) {
            int current_win_width = win_width + (i < remainder ? 1 : 0);
            
            // Resize and move the window logic
            wresize(pair.second, effectiveHeight, current_win_width);
            mvwin(pair.second, 0, current_x);
            
            // Queue the window for refresh (but don't push to screen yet)
            wnoutrefresh(pair.second);

            // Advance X (Width + 1 for the separator gap)
            current_x += current_win_width + 1; 
            i++;
        }

        // --- PASS 2: DRAW SEPARATORS ON STDSCR ---
        // We do this AFTER processing windows so we know exactly where the gaps are.
        // Using ACS_VLINE as advised for better terminal compatibility.
        
        current_x = 0;
        i = 0;
        for (auto& pair : windows) {
            int current_win_width = win_width + (i < remainder ? 1 : 0);

            // Calculate where the separator should be (to the left of the window)
            // We skip the first window (index 0) because there's no line on the far left edge.
            if (i > 0) {
                // The separator is at (current_x - 1)
                mvvline(0, current_x - 1, ACS_VLINE, effectiveHeight);
            }
            
            current_x += current_win_width + 1;
            i++;
        }

    // --- REFRESH STDSCR ---
    // This effectively "prints" the lines we just drew onto the background layer
    wnoutrefresh(stdscr);
    
    // Push everything to the physical screen
    doupdate();
}


WINDOW* getWindowAt(int y, int x) {
        // Iterate through all windows
        for (auto it = windows.rbegin(); it != windows.rend(); ++it) {
            WINDOW* win = it->second;
            
            // 1. Get Window Position (Top-Left corner relative to screen)
            int winY, winX;
            getbegyx(win, winY, winX); 

            // 2. Get Window Dimensions (Height and Width)
            int winH, winW;
            getmaxyx(win, winH, winW);

            // 3. Perform Hit Test
            // Check if the click coordinates (y, x) are within this window's bounds
            if (x >= winX && x < (winX + winW) &&
                y >= winY && y < (winY + winH)) {
                return win;
            }
        }

        return nullptr; // No window found at these coordinates
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