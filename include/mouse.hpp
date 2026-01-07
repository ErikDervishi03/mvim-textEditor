#pragma once
#include <ncurses.h>

namespace Mouse {
    /**
     * @brief Handles mouse events including clicks, dragging, and scrolling.
     * Checks boundaries to ensure the cursor stays within valid buffer limits.
     */
    void handle_event();

    /**
     * @brief Checks current mouse state and applies continuous actions
     * (like scrolling) if specific conditions are met (e.g., dragging at edge).
     * Should be called periodically from the main loop.
     */
    void behavior_timer();

    /**
     * @brief Resets mouse state and re-enables tracking. 
     * Essential when switching files or buffers.
     */
    void reset_dragging();
}