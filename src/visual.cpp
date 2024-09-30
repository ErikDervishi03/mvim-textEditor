#include "../include/action.hpp"

void action::visual::highlight_text() {

    int start_col;
    int end_col;
    int curr_row;

    // Imposta il tetto minimo e massimo per visual_start_row e visual_end_row
    int visible_start_row = std::max((int)visual_start_row, (int)starting_row); // Assicura che non vada sotto la prima riga visibile
    int visible_end_row = std::min((int)visual_end_row, (int)(max_row + starting_row)); // Assicura che non vada sopra l'ultima riga visibile

    // Calcola il numero di righe da evidenziare, basato sui valori visibili
    int row_to_highlight = abs(visible_end_row - visible_start_row);

    // Ciclo attraverso le righe visibili da evidenziare
    for (int i = 0; i <= row_to_highlight; i++) {
        curr_row = (visible_end_row > visible_start_row) ? visible_start_row + i : visible_start_row - i;

        // Lunghezza della riga corrente
        int row_length = buffer[curr_row].length();

        // Tratta le righe vuote come se avessero almeno un carattere per l'evidenziazione
        if (row_length == 0) {
            row_length = 1;  // Tratta la riga vuota come se contenesse un carattere
        }

        if (visible_end_row == visible_start_row) {
            // Ottieni la colonna iniziale e finale per la riga corrente (stessa riga)
            start_col = visual_start_col;
            end_col = visual_end_col;
        } else if (i == 0) {
            // Prima riga della selezione
            start_col = visual_start_col;
            end_col = (visible_end_row < visible_start_row) ? span + 1 : row_length + span + 1;
        } else if (i == row_to_highlight) {
            // Ultima riga della selezione
            start_col = (visible_end_row < visible_start_row) ? row_length + span + 1 : span + 1;
            end_col = visual_end_col;
        } else {
            // Righe intermedie
            start_col = span + 1;
            end_col = row_length + span + 1;
        }

        // Assicurati che almeno un carattere venga evidenziato
        int highlight_length = std::max(abs(end_col - start_col), 1); 

        // Evidenziazione della riga corrente
        mvchgat(curr_row - starting_row, std::min(start_col, end_col), highlight_length, A_NORMAL, 1, NULL);
    }
}



// Function to copy the highlighted text based on visual mode selection
void action::visual::copy_highlighted() {

    copy_paste_buffer = "";

    int start_row = visual_start_row;
    int end_row = visual_end_row;
    int start_col = visual_start_col - span - 1;
    int end_col = visual_end_col - span - 1;

    // Case 1: Highlight is within a single row
    if (start_row == end_row) {
        // the copying process must occur from left to right so we start from the column that is furthest to the left
        int copy_start   = std::min(start_col, end_col);
        int char_to_copy = std::min(abs(end_col - start_col) + 1, (int)buffer[start_row].length()); // number of characters to copy, at least 1

        copy_paste_buffer = buffer[start_row].substr(copy_start, char_to_copy); 

        action::system::change2normal();
        return;
    }

    // Case 2: Highlight spans multiple rows
    if (end_row < start_row) {
        std::swap(start_row, end_row);
        std::swap(start_col, end_col);
    } // swap the rows if the end row is before the start row

    copy_paste_buffer = buffer[start_row].substr(start_col);

    // Copy the middle rows entirely
    for (int i = 0; i < abs(start_row - end_row) - 1; ++i) {
        int curr_row = start_row + i + 1;
        copy_paste_buffer += '\n' + buffer[curr_row];
    }

    copy_paste_buffer += '\n' + buffer[end_row].substr(0, end_col);

    action::system::change2normal();
}

// Function to delete and copy the highlighted text based on visual mode selection
void action::visual::delete_highlighted() {

    copy_paste_buffer = "";  // Clear the buffer before copying the highlighted text

    int start_row = visual_start_row;
    int end_row = visual_end_row;
    int start_col = visual_start_col - span - 1;
    int end_col = visual_end_col - span - 1;

    // Case 1: Highlight is within a single row
    if (start_row == end_row) {
        // The copying and deletion process must occur from left to right
        int copy_start   = std::min(start_col, end_col);
        int char_to_copy = std::min(abs(end_col - start_col) + 1, (int)buffer[start_row].length());  // Number of characters to copy and delete

        // Copy and delete the text
        copy_paste_buffer = buffer.slice_row(start_row, copy_start, copy_start + char_to_copy);

        cursor.setX(std::min(start_col, end_col));  // Set the cursor to the start of the highlighted text
        action::system::change2normal();
        return;

    } 
    // Case 2: Highlight spans multiple rows
    if (end_row < start_row) {
        std::swap(start_row, end_row);
        std::swap(start_col, end_col);
    } // swap the rows if the end row is before the start row

    copy_paste_buffer = buffer.slice_row(start_row, start_col, buffer[start_row].length());

    // Copy the middle rows entirely
    for (int i = 0; i < end_row - start_row - 1; ++i) {
        copy_paste_buffer += '\n' + buffer[start_row + 1];
        buffer.del_row(start_row + 1);  // Delete the current row after copying it
    }

    copy_paste_buffer += '\n' + buffer.slice_row(start_row + 1, 0, end_col);
    
    buffer.merge_rows(start_row, start_row + 1);

    pointed_row = start_row;

    cursor.set(start_col, start_row - starting_row);
    
    // Switch back to normal mode after deletion and copying
    action::system::change2normal();
}
