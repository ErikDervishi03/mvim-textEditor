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
    /* --- INSERT MODE --- */
    insertMap[KEY_UP] = editor::movement::move_up;
    insertMap[KEY_DOWN] = editor::movement::move_down;
    insertMap[KEY_LEFT] = editor::movement::move_left;
    insertMap[KEY_RIGHT] = editor::movement::move_right;
    insertMap[KEY_ENTER_] = editor::modify::new_line;
    insertMap[KEY_BACKSPACE] = editor::modify::delete_letter;
    insertMap[KEY_BACKSPACE_LEGACY] = editor::modify::delete_letter;
    insertMap[KEY_TAB] = editor::modify::tab;
    insertMap[ESC] = editor::system::change2normal;

    // Copy operations
    insertMap[ctrl('c')] = editor::visual::copy_line; 
    insertMap[ctrl('z')] = editor::modify::undo;            
    insertMap[ctrl('v')] = editor::modify::paste;           
    insertMap[ctrl('a')] = editor::visual::select_all;      
    insertMap[ctrl('f')] = editor::system::change2find;     
    insertMap[ctrl('n')] = editor::system::new_buffer;  
    insertMap[ctrl(KEY_BACKSPACE)] = editor::modify::delete_word_backyard;
    insertMap[ctrl(KEY_BACKSPACE_LEGACY)] = editor::modify::delete_word_backyard;

    /* --- NORMAL MODE --- */
    normalMap[KEY_UP] = editor::movement::move_up;
    normalMap[KEY_DOWN] = editor::movement::move_down;
    normalMap[KEY_LEFT] = editor::movement::move_left;
    normalMap[KEY_RIGHT] = editor::movement::move_right;
    
    // Vim Standard Bindings
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
    normalMap['w'] = editor::movement::move_to_next_word;
    normalMap['e'] = editor::file::file_selection_menu;
    normalMap['q'] = editor::system::exit_ide;
    normalMap['s'] = editor::file::save;

    // Editing
    normalMap['x'] = editor::modify::normal_delete_letter;
    normalMap['d'] = editor::modify::delete_row;
    normalMap['u'] = editor::modify::undo;                  
    normalMap['p'] = editor::modify::paste;
    
    // Mode Switching
    normalMap['i'] = editor::system::change2insert;
    normalMap['v'] = editor::system::change2visual;
    normalMap['f'] = editor::find::find;

    // Copy operations
    normalMap[ctrl('c')] = editor::visual::copy_line; 
    normalMap['y'] = editor::visual::copy_line;      

    // Config Shortcuts 
    normalMap[ctrl('a')] = editor::visual::select_all;      
    normalMap[ctrl('z')] = editor::modify::undo;            
    normalMap[ctrl('v')] = editor::modify::paste;           
    normalMap[ctrl('f')] = editor::system::change2find;     

    // Buffers
    normalMap[ctrl('n')] = editor::system::new_buffer;              
    normalMap['n'] = editor::system::switch_to_next_buffer;   
    normalMap['m'] = editor::system::switch_to_previous_buffer; 


    /* --- VISUAL MODE --- */
    // Basic Navigation
    visualMap[KEY_UP] = editor::movement::move_up;
    visualMap[KEY_DOWN] = editor::movement::move_down;
    visualMap[KEY_LEFT] = editor::movement::move_left;
    visualMap[KEY_RIGHT] = editor::movement::move_right;
    
    // Vim Standard Bindings (Synced with Normal Mode)
    visualMap['h'] = editor::movement::move_left;
    visualMap['j'] = editor::movement::move_down;
    visualMap['k'] = editor::movement::move_up;
    visualMap['l'] = editor::movement::move_right;
    visualMap['w'] = editor::movement::move_to_next_word;

    visualMap['a'] = editor::movement::move_to_end_of_line;       
    visualMap['A'] = editor::movement::move_to_beginning_of_line; 
    visualMap['g'] = editor::movement::move_to_end_of_file;       
    visualMap['G'] = editor::movement::move_to_beginning_of_file; 

    visualMap[ESC] = editor::system::change2normal;

    // Config Shortcuts 
    visualMap[ctrl('a')] = editor::visual::select_all;      
    visualMap[ctrl('c')] = editor::visual::copy_highlighted;    
    visualMap[ctrl('x')] = editor::visual::delete_highlighted;  
    
    // Standard Vim Visual keys
    visualMap['d'] = visualMap[KEY_BACKSPACE] = editor::visual::delete_highlighted;
    visualMap['y'] = editor::visual::copy_highlighted;


    /* --- FIND MODE --- */
    findMap['n'] = editor::find::go_to_next_occurrence;
    findMap['m'] = editor::find::go_to_previous_occurrence;
    findMap['r'] = editor::modify::replace;

    findMap[ESC] = editor::system::change2normal;

    /* --- SPECIAL KEYS (Global) --- */
    specialKeys[ctrl('s')] = editor::file::save;
    specialKeys[ctrl('q')] = editor::system::exit_ide;        
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