#include "../include/mouse.hpp"
#include "../include/globals/mvimResources.h"
#include "../include/editor.hpp"

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

    void handle_event() {
        MEVENT event;

        if (getmouse(&event) != OK) {
            return;
        }

        // --- Handle Scrolling (Wheel) ---
        if (event.bstate & BUTTON4_PRESSED) { // Scroll Wheel Up
            editor::movement::move_up();
            editor::movement::move_up(); 
            editor::movement::move_up(); 
            return;
        }
        
        if (event.bstate & BUTTON5_PRESSED) { // Scroll Wheel Down
            editor::movement::move_down();
            editor::movement::move_down();
            editor::movement::move_down();
            return;
        }

        // --- Handle Clicks & Dragging ---
        
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

        // --- Event Logic ---

        if (event.bstate & BUTTON1_CLICKED) {
            mode = Mode::normal;
            pointed_row = target_row;
            pointed_col = target_col;
            visual_start_row = pointed_row;
            visual_start_col = pointed_col;

            cursor.setY(pointed_row - starting_row);
            cursor.setX(pointed_col - starting_col);

        }else if (event.bstate & BUTTON1_RELEASED) {
            is_dragging = false;
        }
        else if (event.bstate & REPORT_MOUSE_POSITION) {

            is_dragging = true;

            pointed_row = target_row;
            pointed_col = target_col;

            cursor.setY(pointed_row - starting_row);
            cursor.setX(pointed_col - starting_col);

            if (pointed_row != visual_start_row || pointed_col != visual_start_col) {
                mode = Mode::visual;
            }
        }
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