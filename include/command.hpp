#pragma once
#include "editor.hpp"
#include "configParser.hpp" 
#include "globals/consts.h"
#include <ncurses.h>
#include <cctype> 

#define ctrl(x) ((x) & 0x1f)
#define isOpenBracket(c) (c == '{' || c == '[' || c == '(')

class Command
{
private:
  keymap insertMap;
  keymap commandMap;
  keymap normalMap;
  keymap visualMap;
  keymap findMap;
  keymap specialKeys; 

public:
  Command()
  {
    /*insert*/
    insertMap[KEY_UP] = editor::movement::move_up;
    insertMap[KEY_DOWN] = editor::movement::move_down;
    insertMap[KEY_LEFT] = editor::movement::move_left;
    insertMap[KEY_RIGHT] = editor::movement::move_right;
    insertMap[KEY_ENTER_] = editor::modify::new_line;

    insertMap[KEY_BACKSPACE] = editor::modify::delete_letter;
    insertMap[KEY_BACKSPACE_LEGACY] = editor::modify::delete_letter;

    insertMap[KEY_TAB] = editor::modify::tab;

    insertMap[ESC] = editor::system::change2normal;

    /*normal*/
    normalMap[KEY_UP] = editor::movement::move_up;
    normalMap[KEY_DOWN] = editor::movement::move_down;
    normalMap[KEY_LEFT] = editor::movement::move_left;
    normalMap[KEY_RIGHT] = editor::movement::move_right;
    normalMap['h'] = editor::movement::move_left;
    normalMap['j'] = editor::movement::move_down;
    normalMap['k'] = editor::movement::move_up;
    normalMap['l'] = editor::movement::move_right;
    normalMap['o'] = editor::movement::go_down_creating_newline;
    normalMap['O'] = editor::movement::go_up_creating_newline;
    normalMap['a'] = editor::movement::move_to_end_of_line;
    normalMap['A'] = editor::movement::move_to_beginning_of_line;
    normalMap['g'] = editor::movement::move_to_end_of_file;
    normalMap['G'] = editor::movement::move_to_beginning_of_file;
    normalMap['p'] = editor::modify::paste;
    normalMap['w'] = editor::movement::move_to_next_word;
    normalMap['e'] = editor::file::file_selection_menu;
    normalMap['u'] = editor::modify::undo;

    normalMap['x'] = editor::modify::normal_delete_letter;
    normalMap['d'] = editor::modify::delete_row;

    normalMap['s'] = editor::file::save;
    normalMap['q'] = editor::system::exit_ide;

    normalMap['i'] = editor::system::change2insert;
    normalMap['v'] = editor::system::change2visual;

    normalMap['N'] = editor::system::new_buffer;
    normalMap['n'] = editor::system::switch_to_next_buffer;
    normalMap['m'] = editor::system::switch_to_previous_buffer;

    normalMap['f'] = editor::find::find;

    /*visual*/
    visualMap[KEY_UP] = editor::movement::move_up;
    visualMap[KEY_DOWN] = editor::movement::move_down;
    visualMap[KEY_LEFT] = editor::movement::move_left;
    visualMap[KEY_RIGHT] = editor::movement::move_right;
    visualMap['h'] = editor::movement::move_left;
    visualMap['j'] = editor::movement::move_down;
    visualMap['k'] = editor::movement::move_up;
    visualMap['l'] = editor::movement::move_right;
    visualMap['a'] = editor::movement::move_to_end_of_line;
    visualMap['A'] = editor::movement::move_to_beginning_of_line;
    visualMap['g'] = editor::movement::move_to_end_of_file;
    visualMap['G'] = editor::movement::move_to_beginning_of_file;
    visualMap['w'] = editor::movement::move_to_next_word;
    visualMap['d'] = visualMap[KEY_BACKSPACE] = editor::visual::delete_highlighted;
    visualMap['y'] = editor::visual::copy_highlighted;

    visualMap[ESC] = editor::system::change2normal;

    /*find*/
    findMap['n'] = editor::find::go_to_next_occurrence;
    findMap['N'] = editor::find::go_to_previous_occurrence;
    findMap['r'] = editor::modify::replace;

    findMap[ESC] = editor::system::change2normal;

    /*special keys*/
    specialKeys[ctrl('s')] = editor::file::save;
  }

  void loadConfig(const std::string& filename) {
      ConfigParser::loadKeyBindings(*this, filename);
  }

  void execute(int key)
  {
    if(key == KEY_RESIZE){
      editor::system::resize();
    }

    if (specialKeys.find(key) != specialKeys.end())
    {
      specialKeys[key]();
      return;
    }

    switch (mode)
    {
    case insert:
    {
      if (insertMap.find(key) != insertMap.end())
      {
        insertMap[key]();
        return;
      }

      if (key == ctrl('w'))
      {
        editor::modify::delete_word_backyard();
      }
      else if (key < 256 && isprint(key)) 
      {
        editor::modify::insert_letter(key);
      }
      break;
    }
    case normal:
    {
      if (normalMap.find(key) != normalMap.end())
      {
        normalMap[key]();
      }
      break;
    }
    case visual:
    { 
      if(isOpenBracket(key)){
        editor::visual::insert_brackets(key, getClosingBracketOf(key));
        return;
      }
      if (visualMap.find(key) != visualMap.end())
      {
        visualMap[key]();
      }
      break;
    }
    case find:
    {
      if (findMap.find(key) != findMap.end())
      {
        findMap[key]();
      }
      break;
    }
    default:
      break;
    }
  }

  void bind(int key, std::function<void()> editor, Mode mode_)
  {
    switch (mode_)
    {
    case insert: insertMap[key] = editor; break;
    case normal: normalMap[key] = editor; break;
    case command: commandMap[key] = editor; break;
    case visual: visualMap[key] = editor; break;
    case find: findMap[key] = editor; break;
    default: break;
    }
  }

  void bind(const std::vector<int>& keys, std::function<void()> editor, Mode mode_)
  {
      for (int key : keys)
      {
          bind(key, editor, mode_);
      }
  }

  static int getClosingBracketOf(int bracket){
    if (bracket == '{') return '}';
    if(bracket == '[') return ']';
    if(bracket == '(') return ')';
    else return '?';
  }

  void menu() {}
  void save() {}
};