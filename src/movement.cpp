#include "../include/action.hpp"

  void action::movement::new_line(){
      buffer.new_row("", pointed_row + 1);
      
      //here we are breaking down the line.
      //All the text from the current cursor
      //position to the end of the line go in 
      //the next line
      
      const int curr_row_length = buffer.get_string_row(pointed_row).length();
      if (cursor.getX() != curr_row_length) {
        std::string line_break = buffer.slice_row(pointed_row, cursor.getX(), curr_row_length);
        buffer.row_append(pointed_row + 1, line_break);
      }
      
      if (cursor.getY() >= max_row - SCROLL_START_THRESHOLD - 1 && 
          !buffer.is_void_row(max_row) && 
          pointed_row < buffer.get_number_rows()) {

          starting_row++;
      }else if(cursor.getY() < max_row - 1){
        cursor.move_down();
      }

      cursor.setX(0);
      pointed_row++;
  }

  void action::movement::move_up(){
    if (!(starting_row == 0 && pointed_row == 0)) {
      std::string prev_row = buffer.get_string_row(pointed_row - 1);
      if(cursor.getY() > SCROLL_START_THRESHOLD || starting_row == 0){
        cursor.move_up();
      }
      if (prev_row.length() < cursor.getX()) {
        cursor.setX(prev_row.length());
      } 
      if(starting_row > 0 && pointed_row <= starting_row + SCROLL_START_THRESHOLD){
        starting_row--;
      }
      pointed_row--;
    }
  }

  void action::movement::move_down(){
    if (pointed_row < buffer.get_number_rows() - 1) {
      std::string next_row = buffer.get_string_row(pointed_row + 1);
      if(cursor.getY() < max_row - SCROLL_START_THRESHOLD - 1 || 
        pointed_row >= buffer.get_number_rows() - SCROLL_START_THRESHOLD - 1){
        
        cursor.move_down();
      }
      if(cursor.getX() >= next_row.length()){
        cursor.setX(next_row.length());
      }
      if(pointed_row - starting_row == max_row - SCROLL_START_THRESHOLD - 1 &&
        pointed_row < buffer.get_number_rows() - SCROLL_START_THRESHOLD - 1){
        
        starting_row++;
      }
      pointed_row++;
    } 
  }

  void action::movement::move_left(){
    if(cursor.getX() == 0 && cursor.getY() > 0){
      cursor.set(buffer.get_string_row(pointed_row - 1).length(), cursor.getY() - 1);
      pointed_row--;
    }else if(cursor.getX() > 0){
      cursor.move_left();
    }
    if(starting_row > 0 && pointed_row == starting_row){
      starting_row--;
    }
  }

  void action::movement::move_right(){
    if((cursor.getX() <= buffer.get_string_row(pointed_row).length() - 1) && !buffer.get_string_row(pointed_row).empty()){  
      cursor.move_right();
    }
  }

  void action::movement::go_down_creating_newline() {
    // Create a new line below the current pointed_row
    buffer.new_row("", pointed_row + 1);

    // Move the cursor to the new line
    cursor.setX(0); // Set cursor to the beginning of the new line
    pointed_row++; // Move the pointed_row down

    // Adjust starting_row if needed
    if (cursor.getY() >= max_row - SCROLL_START_THRESHOLD - 1 &&
        !buffer.is_void_row(max_row) &&
        pointed_row < buffer.get_number_rows()) {
        starting_row++;
    } else if (cursor.getY() < max_row - 1) {
        cursor.move_down();
    }
  }

  void action::movement::move_to_end_of_line() {
    // Get the length of the current row
    const int curr_row_length = buffer.get_string_row(pointed_row).length();
    
    // Set the cursor to the end of the current line
    cursor.setX(curr_row_length);
  }

  void action::movement::move_to_next_word() {
      std::string current_row = buffer.get_string_row(pointed_row);
      int row_length = current_row.length();

      // if we are in the middle of a word we go at the end of it
      while(current_row[cursor.getX()] != ' '){
        action::movement::move_right();

        if(cursor.getX() >= row_length){
          if(pointed_row < buffer.get_number_rows() - 1){
            action::movement::move_down();
            cursor.setX(0);
          }
          return;
        }
      }

      //lets go ahead until we find the next word
      while(current_row[cursor.getX()] == ' '){
        action::movement::move_right();
        
        if(cursor.getX() >= row_length){
          if(pointed_row < buffer.get_number_rows() - 1){
            action::movement::move_down();
            cursor.setX(0);
          }
          return;
        }else if(isalnum(current_row[cursor.getX()])){
          return ;
        }
      }
  }

  void action::movement::move_to_end_of_file() {
    // point to the last row
    pointed_row = buffer.get_number_rows() - 1;

    starting_row = std::max((int)(buffer.get_number_rows() - max_row),0);
    cursor.set(buffer.get_string_row(pointed_row).length(), pointed_row - starting_row);
  }


  void action::movement::move_to_beginning_of_file() {  
    starting_row = 0;
    pointed_row = 0;
    cursor.set(0, 0);
  }