#pragma once

#include <map>
#include <string>
#include <variant>

struct _win_st;           
typedef struct _win_st WINDOW;

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
    static WindowManager& getInstance();

    // Public interface remains the same
    WINDOW* get_window(const WindowName& name);

    const std::map<std::string, WINDOW*>& get_windows() const;

    int create_window(const WindowName& name);

    int kill_window(const WindowName& name);   

    void refresh_separators();

    // Helper methods
    void resize_windows();



WINDOW* getWindowAt(int y, int x) ;

private:
    // Private members
    std::map<std::string, WINDOW*> windows;

    // Private constructor and destructor for Singleton
    WindowManager();

    ~WindowManager();

    // Delete copy constructor and assignment operator
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    int destroy_window(const WindowName& name);

    std::string to_string(const WindowName& name);
};