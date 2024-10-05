#pragma once
#include <ncurses.h>

#pragma once
#include <ncurses.h>

/**
 * @class Cursor
 * @brief A class to represent and manipulate the cursor position in a text editor interface.
 *
 * The Cursor class manages the x and y coordinates of a cursor within a text editor,
 * allowing for movement in four directions as well as setting and retrieving the current position.
 * It interacts with the ncurses library to visually update the cursor position on the screen.
 */
class Cursor
{
private:
    int x; ///< The current x-coordinate (horizontal position) of the cursor.
    int y; ///< The current y-coordinate (vertical position) of the cursor.

    /**
     * @brief Updates the visual cursor position on the screen.
     * This function is responsible for moving the cursor to its current x and y coordinates
     * using the ncurses `move` function.
     */
    void update_cursor();

public:
    /**
     * @brief Constructs a new Cursor instance at the initial position (0, 0).
     */
    Cursor();

    /**
     * @brief Gets the current x-coordinate of the cursor.
     * @return The x-coordinate of the cursor.
     */
    int getX() const;

    /**
     * @brief Gets the current y-coordinate of the cursor.
     * @return The y-coordinate of the cursor.
     */
    int getY() const;

    /**
     * @brief Moves the cursor one position to the right.
     */
    void move_right();

    /**
     * @brief Moves the cursor one position to the left.
     */
    void move_left();

    /**
     * @brief Moves the cursor one position up.
     */
    void move_up();

    /**
     * @brief Moves the cursor one position down.
     */
    void move_down();

    /**
     * @brief Sets the cursor position to specified x and y coordinates.
     * @param x The new x-coordinate of the cursor.
     * @param y The new y-coordinate of the cursor.
     */
    void set(int x, int y);

    /**
     * @brief Sets the x-coordinate of the cursor.
     * @param x The new x-coordinate to set.
     */
    void setX(int x);

    /**
     * @brief Sets the y-coordinate of the cursor.
     * @param y The new y-coordinate to set.
     */
    void setY(int y);

    /**
     * @brief Restores the cursor position with an optional horizontal span.
     * This method moves the cursor to its current position, adjusted by a specified span.
     * @param span An optional parameter to adjust the x position. Default is 0.
     */
    void restore(int span = 0);
};
