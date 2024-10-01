#pragma once
#include "action.hpp"
#include "globals/mode.h"
#include <ncurses.h>

/*
  need to map keys for the specific mode
*/

typedef std::map<int, std::function<void()>> keymap;

class Command {

private:
  keymap insertMap;
  keymap commandMap;
  keymap normalMap;
  keymap visualMap;
  keymap findMap;
public:
  Command() {
    /*insert*/
    insertMap[KEY_UP] = action::movement::move_up;
    insertMap[KEY_DOWN] = action::movement::move_down;
    insertMap[KEY_LEFT] = action::movement::move_left;
    insertMap[KEY_RIGHT] = action::movement::move_right;
    insertMap[KEY_ENTER_] = action::movement::new_line;

    insertMap[KEY_BACKSPACE] = action::modify::delete_letter;
    insertMap[KEY_TAB] = action::modify::tab;

    insertMap[ESC] = action::system::change2normal;

    /*normal*/

    normalMap[KEY_UP] = action::movement::move_up;
    normalMap[KEY_DOWN] = action::movement::move_down;
    normalMap[KEY_LEFT] = action::movement::move_left;
    normalMap[KEY_RIGHT] = action::movement::move_right;
    normalMap['h'] = action::movement::move_left;
    normalMap['j'] = action::movement::move_down;
    normalMap['k'] = action::movement::move_up;
    normalMap['l'] = action::movement::move_right;
    normalMap['o'] = action::movement::go_down_creating_newline;
    normalMap['O'] = action::movement::go_up_creating_newline;
    normalMap['a'] = action::movement::move_to_end_of_line;
    normalMap['A'] = action::movement::move_to_beginning_of_line;
    normalMap['g'] = action::movement::move_to_end_of_file;
    normalMap['G'] = action::movement::move_to_beginning_of_file;
    normalMap['p'] = action::modify::paste;
    normalMap['w'] = action::movement::move_to_next_word;
    normalMap['e'] = action::file::file_selection_menu;

    normalMap['x'] = action::modify::normal_delete_letter;
    normalMap['d'] = action::modify::delete_row;

    normalMap['s'] = action::file::save;

    normalMap['q'] = action::system::exit_ide;
    normalMap['m'] = action::system::helpMenu;

    normalMap['i'] = action::system::change2insert;

    normalMap['v'] = action::system::change2visual;

    normalMap['f'] = action::find::find;

    /*visual*/
    visualMap[KEY_UP] = action::movement::move_up;
    visualMap[KEY_DOWN] = action::movement::move_down;
    visualMap[KEY_LEFT] = action::movement::move_left;
    visualMap[KEY_RIGHT] = action::movement::move_right;
    visualMap['h'] = action::movement::move_left;
    visualMap['j'] = action::movement::move_down;
    visualMap['k'] = action::movement::move_up;
    visualMap['l'] = action::movement::move_right;
    visualMap['a'] = action::movement::move_to_end_of_line;
    visualMap['A'] = action::movement::move_to_beginning_of_line;
    visualMap['g'] = action::movement::move_to_end_of_file;
    visualMap['G'] = action::movement::move_to_beginning_of_file;
    visualMap['w'] = action::movement::move_to_next_word;
    visualMap['d'] = action::visual::delete_highlighted;

    visualMap['y'] = action::visual::copy_highlighted;

    visualMap[ESC] = action::system::change2normal;

    /*find*/
    findMap[ESC] = action::system::change2normal;
    findMap['n'] = action::find::go_to_next_occurrence;
    findMap['N'] = action::find::go_to_previous_occurrence;
    findMap['r'] = action::modify::replace;
  }

void execute(int key) {
  switch (mode) {
    case insert:
      if (insertMap.find(key) != insertMap.end()) {
          insertMap[key]();
      } else {
          action::modify::insert_letter(key);
      }
      break;

    case normal:
      if (normalMap.find(key) != normalMap.end()) {
          normalMap[key]();
      }
      break;

    case visual:
      if (visualMap.find(key) != visualMap.end()) {
          visualMap[key]();
      }
      break;

    case find:
      if (findMap.find(key) != findMap.end()) {
          findMap[key]();
      }
      break;

    default:
      // Optional: handle unknown mode
      break;
  }
}

void bind(int key, std::function<void()> action, Mode mode_) {
  switch (mode_) {
    case insert:
      insertMap[key] = action;
      break;

    case normal:
      normalMap[key] = action;
      break;

    case command:
      commandMap[key] = action;
      break;

    default:
      // Optional: handle unknown mode
      break;
  }
}


  void menu() {
    // TODO
  }

  void save() {
    // TODO
  }
};