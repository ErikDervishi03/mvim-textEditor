#pragma once
#include <string>

namespace editor {
  enum ActionType { INSERT_CHAR, DELETE_CHAR, INSERT_NEWLINE, DELETE_NEWLINE, DELETE_ROW, PASTE, DELETE_SELECTION };

  struct Action {
    ActionType type;
    int row;
    int col;
    char letter;              
    std::string text;         
    bool is_chained = false;
  };

  struct SearchMatch {
      int row;
      int col;
      int length;
  };
}