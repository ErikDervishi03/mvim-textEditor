#include "../include/action.hpp"

  void action::modify::insert_letter(int letter){
    status = Status::unsaved;

    if(cursor.getX()==max_col-7){
      buffer.new_row("", pointed_row + 1);
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
    buffer.insert_letter(pointed_row, cursor.getX(), letter);
    cursor.move_right();
  }

  void action::modify::delete_letter(){

    status = Status::unsaved;

    if (cursor.getX() == 0 && (cursor.getY() > 0 || starting_row > 0)) {
      if (starting_row > 0 && cursor.getY() == SCROLL_START_THRESHOLD) {
          starting_row--;
      }else if(cursor.getY() > 0){
        cursor.move_up();
      }
      cursor.setX(buffer.get_string_row(pointed_row - 1).length());
      buffer.merge_rows(pointed_row - 1, pointed_row);
      pointed_row--;
    } else if (cursor.getX() > 0) {
      cursor.move_left();
      buffer.delete_letter(pointed_row, cursor.getX());
    }
  }

  void action::modify::normal_delete_letter(){

    status = Status::unsaved;

    if (cursor.getX() >= 0) {
      if(buffer.get_string_row(pointed_row).length() == cursor.getX() && cursor.getX() > 0)
        cursor.move_left();
      buffer.delete_letter(pointed_row, cursor.getX());
    }
  }

  void action::modify::tab(){

    status = Status::unsaved;

    for(int i = 0; i < tab_size; i++){
      buffer.insert_letter(pointed_row, cursor.getX(), ' ');
      cursor.move_right();  
    }
    // refresh_row(pointed_row);
  }

  void action::modify::delete_row() {
      status = Status::unsaved;

      // Verifica se il buffer non è vuoto
      if (!buffer.is_void()) {
          // Cancella la riga puntata
          buffer.del_row(pointed_row);
      }

      // Gestisci la posizione del cursore
      if (buffer.is_void()) {
          // Se il buffer è vuoto, reinizializzalo con una riga vuota e reimposta il cursore
          cursor.setX(0);        // Reimposta il cursore all'inizio della riga
          cursor.setY(0);
          pointed_row = 0;       // Reimposta la riga puntata a 0
          starting_row = 0;      // Reimposta lo scrolling
      } else{
          action::movement::move_up();
          cursor.setX(buffer.get_string_row(pointed_row).length()); // Posiziona alla fine della riga precedentess
      }
  }


  void action::modify::paste(){
    if(copy_paste_buffer.length() > 0){
      status = Status::unsaved;

      // Itera su ogni carattere della stringa
      for (char c : copy_paste_buffer) {
          if (c == '\n') {
              // Inserisci newline e sposta alla riga successiva
              action::movement::new_line();
          } else {
              // Inserisci il carattere e sposta la posizione
              action::modify::insert_letter(c);
          }
      }
    }
  }