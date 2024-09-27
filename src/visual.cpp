#include "../include/action.hpp"

  void action::visual::highlight_text() {
    int row_end_col;
    int row_start_col;
    // Clear the previous selection
    for (int i = 0; i <= abs(visual_end_row - visual_start_row); i++) {
        int curr_row = (visual_end_row > visual_start_row) ? visual_start_row + i : visual_start_row - i;
        // Get the length of the row text
        int row_length = buffer.get_string_row(curr_row + starting_row).length();

        if (visual_end_row == visual_start_row) {
          // Get the start and end columns for the current row
          row_start_col = visual_start_col;
          row_end_col   = visual_end_col;
        }else {
          if (i == 0) {
            // Get the start and end columns for the first row
            row_start_col = visual_start_col;
            row_end_col   = (visual_end_row < visual_start_row) ? span : row_length + span + 1;

          } else if (i == abs(visual_end_row - visual_start_row)) {
            row_start_col = (visual_end_row < visual_start_row) ? row_length + span + 1 : span + 1;
            row_end_col   = visual_end_col;
          } else {
            // Get the start and end columns for the middle rows
            row_start_col = span + 1;
            row_end_col   = row_length + span + 1;
          }
        }

        // Highlight the text in the current row
        for (int j = 0; j < abs(row_end_col - row_start_col); j++) {
            // Get the current position on the screen
            int screen_x = (row_end_col > row_start_col) ? row_start_col + j : row_start_col - j;
            int screen_y = (visual_end_row > visual_start_row) ? visual_start_row + i : visual_start_row - i;

            // Definisci la coppia di colori (colore testo, colore sfondo)
            init_pair(1, COLOR_WHITE, COLOR_RED); // Testo bianco, sfondo rosso

            // Cambia il carattere a coordinate specificate con sfondo rosso
            mvchgat(screen_y, screen_x, 1, A_NORMAL, 1, NULL);
        }
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
        //std::cout << "start row : " << start_row << std::endl;
        if(end_col > start_col)
          copy_paste_buffer = buffer.get_string_row(start_row + starting_row).substr(start_col, end_col - start_col);
        else
          copy_paste_buffer = buffer.get_string_row(start_row + starting_row).substr(end_col, start_col - end_col);
    }
    // Case 2: Highlight spans multiple rows
    else if(end_row > start_row){

        copy_paste_buffer = buffer.get_string_row(start_row + starting_row).substr(start_col);

        // Copy the middle rows entirely
        for (int row = 0; row < abs(start_row - end_row) - 1; ++row) {
            int curr_row = start_row + row + 1;
            copy_paste_buffer += '\n' + buffer.get_string_row(curr_row);
        }

        copy_paste_buffer += '\n' + buffer.get_string_row(end_row + starting_row).substr(0, end_col);

    }else{
        copy_paste_buffer = buffer.get_string_row(end_row + starting_row).substr(end_col);

        // Copy the middle rows entirely
        for (int row = 0; row < abs(start_row - end_row) - 1; ++row) {
            int curr_row = end_row + row + 1;
            copy_paste_buffer += '\n' + buffer.get_string_row(curr_row);
        }

        copy_paste_buffer += '\n' + buffer.get_string_row(start_row + starting_row).substr(0,start_col);

    }
    
    action::system::change2normal();
  }