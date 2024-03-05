#pragma once
#include "action.hpp"

class Command{

  private:
    std::map<int, std::function<void()>> key2action;

  public:
    Command(){
      key2action[KEY_UP] = action::movement::move_up;
      key2action[KEY_DOWN] = action::movement::move_down;
      key2action[KEY_LEFT] = action::movement::move_left;
      key2action[KEY_RIGHT] = action::movement::move_right;
      key2action[10] = action::movement::new_line;

      key2action[KEY_BACKSPACE] = action::modify::delete_letter;
      key2action[KEY_TAB] = action::modify::tab;
    }

    void execute(int key){
      if(key2action.find(key) != key2action.end()){
        key2action[key]();
      }else{
        action::modify::insert_letter(key);
      }
    }

    void menu(){
      // TODO
    }

    void save(){
      // TODO
    }

    void bind(){
      // TODO
    }

};