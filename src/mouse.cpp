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

#define isDragging() (pointed_row != visual_start_row || pointed_col != visual_start_col)

namespace Mouse {

    static bool is_dragging = false;
    static int last_mouse_y = 0;
    static int last_mouse_x = 0;

    void reset_dragging() {
        printf("\033[?1002h");
        fflush(stdout);
    }

    // Helper to sync column after vertical/horizontal movement
    void sync_cursor_column() {
        int target_col = 0;
        
        // FIX: If in margin, snap to 'starting_col' (visible left edge), NOT 0.
        if (last_mouse_x < (span + 1)) {
            target_col = starting_col; 
        } else {
            target_col = last_mouse_x - (span + 1) + starting_col;
        }

        int line_len = buffer[pointed_row].length();
        if (target_col > line_len) target_col = line_len;
        
        // Ensure we don't go before the visible area if simply syncing
        if (target_col < starting_col) target_col = starting_col;

        pointed_col = target_col;
        
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
    // Action Processing (Click, Drag, Release, Double/Triple)
    // ---------------------------------------------------------
    static void process_mouse_action(mmask_t bstate, int target_row, int target_col) {
        
        // --- 1. TRIPLE CLICK: Select Whole Line ---
        if (bstate & BUTTON1_TRIPLE_CLICKED) {
            mode = Mode::visual;
            is_dragging = false;

            editor::movement::move2Y(target_row);

            // Start at beginning of line
            visual_start_row = target_row;
            visual_start_col = 0;

            // End at end of line
            int line_len = buffer[target_row].length();
            
            // Move cursor to end of line
            editor::movement::move2X(line_len); 
            pointed_col = line_len; // Ensure pointed_col captures the full length
            pointed_row = target_row;
        }

        // --- 2. DOUBLE CLICK: Select Word ---
        else if (bstate & BUTTON1_DOUBLE_CLICKED) {
            mode = Mode::visual;
            is_dragging = false;

            editor::movement::move2Y(target_row);
            
            std::string& line = buffer[target_row];
            int len = line.length();
            int start = target_col;
            int end = target_col;

            // Safety check
            if (target_col < len) {
                // Determine if we are clicking on a word char (alphanum or _) or delimiter
                bool is_word_char = (isalnum(line[target_col]) || line[target_col] == '_');

                // Find start of the word/segment
                while (start > 0) {
                    bool prev_char_is_word = (isalnum(line[start - 1]) || line[start - 1] == '_');
                    if (prev_char_is_word != is_word_char) break;
                    start--;
                }

                // Find end of the word/segment
                while (end < len) {
                    bool curr_char_is_word = (isalnum(line[end]) || line[end] == '_');
                    if (curr_char_is_word != is_word_char) break;
                    end++;
                }
            }

            visual_start_row = target_row;
            visual_start_col = start;

            // Point cursor to the end of the found word (exclusive index usually becomes inclusive in visual logic)
            // Adjust -1 if your visual mode is inclusive of the pointed character
            int cursor_target = (end > 0) ? end - 1 : 0; 
            if (end == len) cursor_target = len; // Handle end of line case

            editor::movement::move2X(cursor_target);
        }

        // --- 3. SINGLE CLICK: Start Normal Selection ---
        else if ((bstate & BUTTON1_CLICKED) || (bstate & BUTTON1_PRESSED)) {
            mode = Mode::normal;
            is_dragging = false;

            editor::movement::move2Y(target_row);
            editor::movement::move2X(target_col);

            visual_start_row = pointed_row;
            visual_start_col = pointed_col;
        }

        // --- 4. RELEASE: Stop Dragging ---
        else if (bstate & BUTTON1_RELEASED) {
            is_dragging = false;
        }

        // --- 5. DRAGGING: Update Selection ---
        else if (bstate & REPORT_MOUSE_POSITION) {
            is_dragging = true;

            editor::movement::move2Y(target_row);
            editor::movement::move2X(target_col);

            if (isDragging()) {
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
            target_col = starting_col; 
        } else {
            target_col = x - (span + 1) + starting_col;
            if (target_col > line_len) target_col = line_len;
            if (target_col < 0) target_col = starting_col;
        }


        // 4. Apply Actions (Click/Drag logic)
        process_mouse_action(event.bstate, target_row, target_col);
    }

    // ---------------------------------------------------------
    // Timer Logic: Handles Dragging Outside Bounds
    // ---------------------------------------------------------
    void behavior_timer() {
        if (!is_dragging) return;

        bool scrolled = false;

        // --- Vertical Scrolling ---
        if (last_mouse_y == 0) {
            editor::movement::move_up();
            scrolled = true;
        } 
        else if (last_mouse_y >= (int)max_row - 1) {
            editor::movement::move_down();
            scrolled = true;
        }

        // --- Horizontal Scrolling ---
        // Left Edge: Mouse is in the line number area or at col 0
        if ((last_mouse_x <= (span + 1) && starting_col > 0)) {
            editor::movement::move_left();
            scrolled = true;
        }

        // Right Edge: Mouse is at the rightmost visible text column
        // (max_col is the text width, span is the offset for line numbers)
        else if (last_mouse_x >= (max_col + span)) {
             editor::movement::move_right();
             scrolled = true;
        }

        // If any scroll happened, re-sync the selection
        if (scrolled) {
            sync_cursor_column();

            if (isDragging()) {
                mode = Mode::visual;
            }
        }
    }
}