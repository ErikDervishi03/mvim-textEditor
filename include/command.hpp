#pragma once
#include "action.hpp"

#define ctrl(x) ((x) & 0x1f)
#define isPrintable(c) (isalpha(c) || isdigit(c) || isSpecialChar(c))
#define isOpenBracket(c) (c == '{' || c == '[' || c == '(')


class Command
{

private:
  keymap insertMap;
  keymap commandMap;
  keymap normalMap;
  keymap visualMap;
  keymap findMap;
  keymap specialKeys; /*ctrl s, ctrl w ...*/
public:
  Command()
  {

    /*insert*/
    insertMap[KEY_UP] = action::movement::move_up;
    insertMap[KEY_DOWN] = action::movement::move_down;
    insertMap[KEY_LEFT] = action::movement::move_left;
    insertMap[KEY_RIGHT] = action::movement::move_right;
    insertMap[KEY_ENTER_] = action::modify::new_line;

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
    normalMap['u'] = action::modify::undo;

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
    visualMap['d'] = visualMap[KEY_BACKSPACE] = action::visual::delete_highlighted;

    visualMap['y'] = action::visual::copy_highlighted;

    visualMap[ESC] = action::system::change2normal;


    /*find*/
    findMap['n'] = action::find::go_to_next_occurrence;
    findMap['N'] = action::find::go_to_previous_occurrence;
    findMap['r'] = action::modify::replace;

    findMap[ESC] = action::system::change2normal;

    /*special keys*/
    specialKeys[ctrl('s')] = action::file::save;
  }

  static bool isSpecialChar(char c)
  {
    std::set<char> specialChars = { '[', ']', '%', '!', '@', '#',
                                    '$', '^', '&', '*', '(', ')',
                                    '{', '}', '<', '>', '/', '\\',
                                    '|', '~', '-', '+', '=', '_',
                                    ':', ' ' };

    return specialChars.find(c) != specialChars.end();
  }

  void execute(int key)
  {
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
        action::modify::delete_word_backyard();
      }
      else if (isPrintable(key))
      {
        action::modify::insert_letter(key);
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
        action::visual::insert_brackets(key, getClosingBracketOf(key));
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
    {
      // Optional: handle unknown mode
      break;
    }
    }
  }

  void bind(int key, std::function<void()> action, Mode mode_)
  {
    switch (mode_)
    {
    case insert:
    {
      insertMap[key] = action;
      break;
    }

    case normal:
    {
      normalMap[key] = action;
      break;
    }

    case command:
    {
      commandMap[key] = action;
      break;
    }

    default:
    {
      // Optional: handle unknown mode
      break;
    }
    }
  }

  // Overload bind to take an array of characters
  void bind(const std::vector<int>& keys, std::function<void()> action, Mode mode_)
  {
      for (int key : keys)
      {
          bind(key, action, mode_);  // Call the original bind function for each key
      }
  }

  static int getClosingBracketOf(int bracket){
    if (bracket == '{') return '}';
    if(bracket == '[') return ']';
    if(bracket == '(') return ')';
    else return '?';
  }


  void menu()
  {
    // TODO
  }

  void save()
  {
    // TODO
  }
};
