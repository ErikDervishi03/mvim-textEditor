#pragma once

#include <deque>
#include <ncurses.h>
#include <string>
#include <chrono>
#include <vector>

/**
 * @class Screen
 * @brief A singleton class to manage the screen interface for the text editor.
 *
 * The Screen class handles the initialization and management of the ncurses screen,
 * providing methods for drawing, updating, and printing content. It also includes
 * functionality to retrieve the current dimensions of the screen and to manage
 * the display of command and buffer information.
 */
class Screen {

private:
    /**
     * @brief Private constructor to prevent direct instantiation.
     */
    Screen();

        /**
     * @brief Deleted copy constructor to prevent copying.
     */
    Screen(const Screen&) = delete;

    /**
     * @brief Deleted assignment operator to prevent assignment.
     * @return A reference to the current instance.
     */
    Screen& operator=(const Screen&) = delete;

    /**
     * @brief Destructor for the Screen class.
     */
    ~Screen();

    std::string status_message;
    std::chrono::steady_clock::time_point message_timestamp;
    int message_color_pair;
public:
    /**
     * @brief Gets the singleton instance of the Screen class.
     * @return A reference to the single instance of the Screen.
     */
    static Screen& getScreen() {
        static Screen screen_;
        return screen_;
    }
    
    /**
     * @brief Initializes the screen for use, setting up ncurses.
     */
    void start();

    /**
     * @brief Restores the screen to a previous state.
     */
    void restore();

    /**
     * @brief Gets the current width of the screen.
     * @return The width of the screen in characters.
     */
    int get_width();

    /**
     * @brief Gets the current height of the screen.
     * @return The height of the screen in characters.
     */
    int get_height();

    /**
     * @brief Draws a rectangle on the screen.
     * @param y1 The starting y-coordinate of the rectangle.
     * @param x1 The starting x-coordinate of the rectangle.
     * @param y2 The ending y-coordinate of the rectangle.
     * @param x2 The ending x-coordinate of the rectangle.
     */
    void draw_rectangle(int y1, int x1, int y2, int x2);

    /**
     * @brief Updates the screen with the latest buffer content.
     */
    void update();

    /**
     * @brief Prints the text buffer content on the screen.
     */
    void print_buffer();

    /**
     * @brief Prints the status bar at the bottom of the screen.
     */
    void draw_status_bar();

    /**
     * @brief Prints a multiline string starting at specified coordinates.
     * @param start_y The starting y-coordinate for printing.
     * @param start_x The starting x-coordinate for printing.
     * @param str The string to print.
     */
    void print_multiline_string(int start_y, int start_x, const char *str);

    void print_buffer(WINDOW* window);

    void print_buffer(const std::deque<std::string> &buffer, WINDOW* window, size_t starting_row, size_t starting_col, size_t max_col);

    void refresh_all_buffers(); 

    void set_status_message(const std::string& msg, int color_pair = 1);
};