#include "../include/mouse.hpp"
#include "../include/globals/mvimResources.h"
#include "../include/editor.hpp"
#include "../include/bufferManager.hpp"

// Definitions for scroll wheel buttons if not present in older ncurses versions
#if !defined(BUTTON4_PRESSED)
#define BUTTON4_PRESSED NCURSES_BUTTON_PRESSED(4)
#endif
#if !defined(BUTTON5_PRESSED)
#define BUTTON5_PRESSED NCURSES_BUTTON_PRESSED(5)
#endif

namespace Mouse {

    static bool is_dragging = false;
    static int last_mouse_y = 0;
    static int last_mouse_x = 0;

    void reset_dragging() {
        printf("\033[?1002h");
        fflush(stdout);
    }
    // Helper to sync column after vertical movement
    void sync_cursor_column() {
        // Calculate target column based on last known X
        int target_col = 0;
        if (last_mouse_x < (span + 1)) {
            target_col = 0;
        } else {
            target_col = last_mouse_x - (span + 1) + starting_col;
        }

        // Clamp to current row length
        int line_len = buffer[pointed_row].length();
        if (target_col > line_len) target_col = line_len;
        if (target_col < 0) target_col = 0;

        pointed_col = target_col;
        
        // Update visual cursor
        cursor.setY(pointed_row - starting_row);
        cursor.setX(pointed_col - starting_col);
    }

    static bool handle_scrolling(const MEVENT& event) {
        if (event.bstate & BUTTON4_PRESSED) { // Scroll Up
            editor::movement::move_up();
            editor::movement::move_up(); 
            editor::movement::move_up(); 
            return true;
        }
        
        if (event.bstate & BUTTON5_PRESSED) { // Scroll Down
            editor::movement::move_down();
            editor::movement::move_down();
            editor::movement::move_down();
            return true;
        }
        return false;
    }

    static void switch_window_context(const MEVENT& event) {
        // Use the WindowManager to find which window object is at the click coordinates.
        auto* target_window = WindowManager::getInstance().getWindowAt(event.y, event.x);

        if (target_window) {
            // Pass ONLY the window object to the BufferManager.
            // This function handles the context switch (save old -> load new).
            BufferManager::instance().switchToWindowBuffer(target_window);
        }
    }

    // ---------------------------------------------------------
    // Action Processing (Click, Drag, Release)
    // ---------------------------------------------------------
    static void process_mouse_action(mmask_t bstate, int target_row, int target_col) {
        
        // Left Click: Start Selection
        if (bstate & BUTTON1_CLICKED) {
            is_dragging = false;

            pointed_row = target_row;
            pointed_col = target_col;
            visual_start_row = pointed_row;
            visual_start_col = pointed_col;

            cursor.setY(pointed_row - starting_row);
            cursor.setX(pointed_col - starting_col);
        }
        // Left Release: End Dragging
        else if (bstate & BUTTON1_RELEASED) {
            is_dragging = false;
        }
        // Mouse Dragging
        else if (bstate & REPORT_MOUSE_POSITION) {
            is_dragging = true;

            pointed_row = target_row;
            pointed_col = target_col;

            cursor.setY(pointed_row - starting_row);
            cursor.setX(pointed_col - starting_col);

            // Trigger visual mode if selection range exists
            if (pointed_row != visual_start_row || pointed_col != visual_start_col) {
                mode = Mode::visual;
            }
        }
    }

    void handle_event() {
        MEVENT event;

        if (getmouse(&event) != OK) {
            return;
        }

        // 1. Handle Scrolling (Wheel) - return early if scrolled
        if (handle_scrolling(event)) {
            return;
        }
        
        // 2. Window Identification & Buffer Switching
        switch_window_context(event);

        // 3. Handle Clicks & Dragging 
        
        int y = event.y;
        int x = event.x;

        // Store raw window coordinates for timer logic
        // We only update these if the mouse is actually inside the window
        if (wmouse_trafo(pointed_window, &y, &x, false)) {
            last_mouse_y = y;
            last_mouse_x = x;
        } else {
            // If mouse is outside, we return.
            return;
        }

        // 2. Calculate target Buffer Row
        int target_row = y + starting_row;
        if (target_row < 0) target_row = 0;
        if (target_row >= buffer.getSize()) target_row = buffer.getSize() - 1;

        // 3. Calculate target Buffer Column
        int line_len = buffer[target_row].length();
        int target_col = 0;

        if (x < (span + 1)) {
            target_col = 0; 
        } else {
            target_col = x - (span + 1) + starting_col;
            if (target_col > line_len) target_col = line_len;
            if (target_col < 0) target_col = 0;
        }


        // 4. Apply Actions (Click/Drag logic)
        process_mouse_action(event.bstate, target_row, target_col);
    }

    void behavior_timer() {
        if (!is_dragging) return;

        bool scrolled = false;

        // Check if mouse is at the top edge
        if (last_mouse_y == 0) {
            // Move up (this handles scrolling internal to movement.cpp)
            editor::movement::move_up();
            scrolled = true;
        } 
        // Check if mouse is at the bottom edge
        else if (last_mouse_y == (int)max_row - 1) {
            // Move down
            editor::movement::move_down();
            scrolled = true;
        }

        if (scrolled) {
            // If we scrolled, the row under the mouse changed.
            // We must re-sync the pointed_col to the mouse's X position on the NEW row.
            // pointed_row is already updated by move_up/down.
            sync_cursor_column();

            // Ensure we stay in visual mode during drag
            if (pointed_row != visual_start_row || pointed_col != visual_start_col) {
                mode = Mode::visual;
            }
        }
    }
}