#pragma once
#include "action.hpp"

/*
  need to map keys for the specific mode
*/

typedef std::map<int, std::function<void()>> keymap;

class Command {

private:
  keymap insertMap;
  keymap commandMap;
  keymap normalMap;

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
    normalMap['a'] = action::movement::move_to_end_of_line;
    normalMap['g'] = action::movement::move_to_end_of_file;
    normalMap['G'] = action::movement::move_to_beginning_of_file;

    normalMap['x'] = action::modify::normal_delete_letter;
    normalMap['d'] = action::modify::delete_row;

    normalMap['s'] = action::file::save;

    normalMap['q'] = action::system::exit_ide;
    normalMap['m'] = action::system::helpMenu;

    normalMap['i'] = action::system::change2insert;

    /*command*/

    commandMap['w'] = action::file::save;
    // insertMap[ESC] = action::system::change2normal;
  }

  void execute(int key) {
    if (mode == insert) {
      if (insertMap.find(key) != insertMap.end()) {
        insertMap[key]();
      } else {
        action::modify::insert_letter(key);
      }
      return;
    }

    if (mode == normal) {
      if (normalMap.find(key) != normalMap.end()) {
        normalMap[key]();
      }
      return;
    }

    if (mode == command) {
      if (commandMap.find(key) != commandMap.end()) {
        commandMap[key]();
      }
      return;
    }
  }

  void bind(int key, std::function<void()> action, Mode mode_) {
    if (mode_ == insert) {
      insertMap[key] = action;
    }

    else if (mode_ == normal) {
      normalMap[key] = action;
    }

    else if (mode_ == command) {
      commandMap[key] = action;
    }
  }

  void menu() {
    // TODO
  }

  void save() {
    // TODO
  }
};