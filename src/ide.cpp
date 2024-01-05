#include "ide.hpp" 
#define KEY_ENTER_ 10
#define KEY_TAB 9
#define SCROLL_START_THRESHOLD 5

// remember every time you modify the buffer and scroll you have to update the screen

/*
  to fix : 
    when i delete and press enter i don't want
    the cursor to go down the  SCROLL_START_THRESHOLD, this can
    append only in the case the file is on top or at the bottom.
    if i'm pushing don't a text with enter, the cursor is 
    at max_row  - SCROLL_START_THRESHOLD - 1 and the text
    length arrive until the end of the screen i simple scroll
    down. the inverse when i use KEY_BACKSPACE to go up
 */

/*
  COMMAND:
    ...
*/

/*
  FEATURE:
    1)add clipboard
    2)cursor accelaeration
    3)options
    4)save file
    5)open file
*/

/*
  IDEA:
   1) when scroll_up instead of pointed_row == starting_row do pointed_row == starting_row + NUMBER_LINES_START_SCROLLING,
    in order to speed up the scrolling 
    do the same with scroll_down DONE
   2) for undo command you should create a stack per all ACTIONS that are have been done(so when you unstack
    you start from the last ACTIONS done) and a reverse_action function that takes a ACTION and reverse it
    for example : 
    stack{delete(letter, row=3,letter=5),...}
    unstack delete(letter, row=3,letter=5)
    and than call reverse_action(delete(letter, row=3, letter=5)) 
   3) Highligth text from a set of KEYWORD like {return, int, ...}
   4) Auto close brackets () [] {} 
   5) create ACTIONS like : {delete, go_up, go_down, enter ...}
      and create a tree of MOTIONS modellable like : {

                    :
                   / \
                  d   c
                 / \   \
                d   ..  copy(action)
      }
      all leafs should be a ACTION 
*/

/*
  REFACTORING:
    1)refactor handle_input,in order to increase readability
*/

/*
  MODES:
    need to add a READ ONLY MODE, that permit only to do "visualization key"
    that is the arrow keys, the page up and page down, the home and the end ...
    you can add a flag for this purpose
*/

/*
  update_screen(); in this function at row 29 resolve the problem that 
 :%w !pbcopy the old write is still on the screen
*/

/*
  PROBLEM :
    1) when i delete for more times when stating_row is > 0 the program crash
*/

Ide::Ide(){
  pointed_row = 0;
  starting_row = 0;
}

void Ide::init_screen(){
  initscr();
  timeout(0);
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  wbkgd(stdscr, COLOR_PAIR(1));
}

void Ide::open(){
  getmaxyx(stdscr, max_row, max_col);
  cursor.set(0,0);
  read_file("hello.txt");
  while(true){
    int input = getch();
    handle_input(input);
    show_info();
    restore_cursor();
  }
  write_buffer_into_file("hello.txt");
  endwin();
}

void Ide::handle_input(int input){
   switch(input){
    case KEY_BACKSPACE:
        if (cursor.getX() == 0 && (cursor.getY() > 0 || pointed_row > 0)) {
          if (starting_row > 0 && cursor.getY() == SCROLL_START_THRESHOLD) {
              scroll_up();
          }else if(cursor.getY() > 0){
            cursor.move_up();
          }
          cursor.setX(buffer.get_string_row(pointed_row - 1).length());
          buffer.merge_rows(pointed_row - 1, pointed_row);
          update_screen();
          pointed_row--;
        } else if (cursor.getX() > 0) {
          cursor.move_left();
          buffer.delete_letter(pointed_row, cursor.getX());
          refresh_row(pointed_row);
        }
        break;
    case KEY_ENTER_:
        buffer.new_row("", pointed_row + 1);
        
        if (cursor.getX() != buffer.get_string_row(pointed_row).length()) {
          std::string line_break = buffer.slice_row(pointed_row, cursor.getX(), buffer.get_string_row(pointed_row).length());
          buffer.row_append(pointed_row + 1, line_break);
        }

        if (cursor.getY() >= max_row - SCROLL_START_THRESHOLD - 1 && 
            !buffer.is_void_row(max_row) && 
            pointed_row < buffer.get_number_rows()) {

          scroll_down();
        }else if(cursor.getY() < max_row - 1){
          cursor.move_down();
        }

        cursor.setX(0);
        update_screen();
        pointed_row++;

        break;

    case KEY_UP:
        if (!(starting_row == 0 && pointed_row == 0)) {
        std::string prev_row = buffer.get_string_row(pointed_row - 1);
        if(cursor.getY() > SCROLL_START_THRESHOLD || starting_row == 0){
          cursor.move_up();
        }
        if (prev_row.length() < cursor.getX()) {
          cursor.setX(prev_row.length());
        } 
      if(starting_row > 0 && pointed_row <= starting_row + SCROLL_START_THRESHOLD){
          scroll_up();
          update_screen();
        }
        pointed_row--;
      }
      break;

    case KEY_DOWN:
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
            
            scroll_down();
            update_screen();
          }
          pointed_row++;
      } 
      break;

    case KEY_LEFT:
      if(cursor.getX() == 0 && cursor.getY() > 0){
        cursor.set(buffer.get_string_row(pointed_row - 1).length(), cursor.getY() - 1);
        pointed_row--;
      }else if(cursor.getX() > 0){
        cursor.move_left();
      }
      if(starting_row > 0 && pointed_row == starting_row){
        scroll_up();
      }
      break;  
   
    case KEY_RIGHT:
      if((cursor.getX() <= buffer.get_string_row(pointed_row).length() - 1) && !buffer.get_string_row(pointed_row).empty()){  
        cursor.move_right();
      }
      break;

    case KEY_TAB:
      for(int i = 0; i < tab_size; i++){
        buffer.insert_letter(pointed_row, cursor.getX(), ' ');
        cursor.move_right();  
      }
      refresh_row(pointed_row);
      break;
    /*
      need to handle the case then the user type beyond the max col

      if you paste a text that is longer than the max col the behaviour is not correct
      add a orrizontal scroll?
      need to handle the cursor boundary on the right side
    */
    default:
      if(input != -1){
        buffer.insert_letter(pointed_row, cursor.getX(), input);
        cursor.move_right();
        refresh_row(pointed_row);
      }
      break;
  }
}

void Ide::restore_cursor(){
  move(cursor.getY(), cursor.getX() + span + 1);
}

void Ide::update_screen(){
  erase();
  print_buffer();
}

void Ide::scroll_up() {
  starting_row--; 
}

void Ide::scroll_down() {
  starting_row++;
}

void Ide::refresh_row(int row){
  clear_line(row);
  print_row(row);
}

void Ide::clear_line(int row){
  move(row - starting_row, span + 1);
  clrtoeol();
}

void Ide::show_info(){
  mvprintw(0, max_col - 100, "                                                                      ");
  mvprintw(0, max_col - 100,"pointed row : %d, size: %d, starting_row : %d, xcur : %d, ycur: %d", 
          pointed_row, buffer.get_number_rows(), starting_row, cursor.getX(), cursor.getY());
}


void Ide::print_buffer() {
  std::vector<std::string> buffer = this->buffer.get_buffer();
  for (int i = 0 ; (i + starting_row) < buffer.size() && i < max_row; i++) { 
    mvprintw(i, 0, "%d",i + starting_row + 1);
    mvprintw(i, span + 1, buffer[i + starting_row].c_str());
  }
}

void Ide::print_row(int row){
  mvprintw(row - starting_row, span + 1, buffer.get_string_row(row).c_str());
}

void Ide::write_buffer_into_file(const char* file_name) {
  std::ofstream myfile(file_name);
  for (int i = 0; i < buffer.get_number_rows(); i++) { 
    myfile << buffer.get_buffer()[i] << "\n";
  }
  myfile.close();
}

void Ide::read_file(const char* file_name){
  std::ifstream myfile(file_name);
  if (!myfile.is_open()) {
    std::cout << "error";
    return;
  }
  buffer.clear();
  std::string line;
  while (std::getline(myfile, line)) {
    buffer.push_back(line);
  }

  myfile.close();
  restore_cursor();
  print_buffer();
  update_screen();
}
