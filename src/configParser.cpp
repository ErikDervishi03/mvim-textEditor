#include "../include/configParser.hpp"
#include "../include/command.hpp"
#include "../include/editor.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <ncurses.h> // For KEY_ constants

// Helper macro for Ctrl keys
#ifndef ctrl
#define ctrl(x) ((x) & 0x1f)
#endif

std::string ConfigParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

int ConfigParser::parseKey(const std::string& keyStr) {
    std::string key = trim(keyStr);
    
    if (key.length() == 1) {
        return key[0];
    }
    
    // Handle Ctrl keys (e.g., "Ctrl-s")
    if (key.substr(0, 5) == "Ctrl-" && key.length() == 6) {
        return ctrl(key[5]);
    }

    // Handle Special Keys
    static std::map<std::string, int> specialKeys = {
        {"KEY_UP", KEY_UP},
        {"KEY_DOWN", KEY_DOWN},
        {"KEY_LEFT", KEY_LEFT},
        {"KEY_RIGHT", KEY_RIGHT},
        {"KEY_BACKSPACE", KEY_BACKSPACE},
        {"KEY_ENTER", 10},
        {"TAB", 9},
        {"ESC", 27}
    };

    if (specialKeys.find(key) != specialKeys.end()) {
        return specialKeys[key];
    }

    return ERR; // Return Ncurses Error code if key not found
}

std::map<std::string, std::function<void()>> ConfigParser::getActionMap() {
    return {
        // Movement
        {"move_up", editor::movement::move_up},
        {"move_down", editor::movement::move_down},
        {"move_left", editor::movement::move_left},
        {"move_right", editor::movement::move_right},
        {"goto_start_line", editor::movement::move_to_beginning_of_line},
        {"goto_end_line", editor::movement::move_to_end_of_line},
        {"goto_start_file", editor::movement::move_to_beginning_of_file},
        {"goto_end_file", editor::movement::move_to_end_of_file},
        {"next_word", editor::movement::move_to_next_word},

        // Modify
        {"insert_newline_below", editor::movement::go_down_creating_newline},
        {"insert_newline_above", editor::movement::go_up_creating_newline},
        {"delete_char", editor::modify::normal_delete_letter},
        {"delete_row", editor::modify::delete_row},
        {"paste", editor::modify::paste},
        {"undo", editor::modify::undo},
        {"save", editor::file::save},
        
        // System / Modes
        {"quit", editor::system::exit_ide},
        {"mode_insert", editor::system::change2insert},
        {"mode_visual", editor::system::change2visual},
        {"mode_normal", editor::system::change2normal},
        {"mode_find", editor::find::find},
        
        // Buffers
        {"buffer_next", editor::system::switch_to_next_buffer},
        {"buffer_prev", editor::system::switch_to_previous_buffer},
        {"buffer_new", editor::system::new_buffer},
        
        // Visual specific
        {"copy_selection", editor::visual::copy_highlighted},
        {"delete_selection", editor::visual::delete_highlighted},

        // Find specific
        {"find_next", editor::find::go_to_next_occurrence},
        {"find_prev", editor::find::go_to_previous_occurrence},
        {"replace", editor::modify::replace}
    };
}

void ConfigParser::loadKeyBindings(Command& command, const std::string& filename) {
    std::ifstream file(filename);
    
    // It's okay if the config file doesn't exist, just return
    if (!file.is_open()) return;

    std::string line;
    int lineNumber = 0;
    std::string currentSection = "NORMAL"; // Default section
    auto actions = getActionMap();

    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        if (line.empty() || line[0] == '#') continue; // Skip empty lines and comments

        // Check for section headers [SECTION]
        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            std::transform(currentSection.begin(), currentSection.end(), currentSection.begin(), ::toupper);
            
            // Validate Section Name
            if (currentSection != "NORMAL" && currentSection != "INSERT" && 
                currentSection != "VISUAL" && currentSection != "FIND") {
                
                std::string msg = "Config Error Line " + std::to_string(lineNumber) + 
                                  ": Unknown section [" + currentSection + "]";
                ErrorHandler::instance().report(ErrorLevel::WARNING, msg);
                
                // We don't change 'currentSection' to an invalid one to avoid binding subsequent keys to the wrong mode
                currentSection = "INVALID";
            }
            continue;
        }

        // If we are in an invalid section, skip lines until the next section
        if (currentSection == "INVALID") continue;

        // Parse key = action
        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) {
            std::string msg = "Config Error Line " + std::to_string(lineNumber) + 
                              ": Malformed line (missing '='): " + line;
            ErrorHandler::instance().report(ErrorLevel::WARNING, msg);
            continue;
        }

        std::string keyStr = trim(line.substr(0, delimiterPos));
        std::string actionStr = trim(line.substr(delimiterPos + 1));

        if (keyStr.empty() || actionStr.empty()) {
            std::string msg = "Config Error Line " + std::to_string(lineNumber) + ": Empty key or action";
            ErrorHandler::instance().report(ErrorLevel::WARNING, msg);
            continue;
        }

        int keyCode = parseKey(keyStr);
        if (keyCode == ERR) {
            std::string msg = "Config Error Line " + std::to_string(lineNumber) + 
                              ": Unknown key identifier '" + keyStr + "'";
            ErrorHandler::instance().report(ErrorLevel::WARNING, msg);
            continue;
        }

        if (actions.find(actionStr) == actions.end()) {
            std::string msg = "Config Error Line " + std::to_string(lineNumber) + 
                              ": Unknown action '" + actionStr + "'";
            ErrorHandler::instance().report(ErrorLevel::WARNING, msg);
            continue;
        }

        // Determine Mode
        Mode mode = Mode::normal;
        if (currentSection == "INSERT") mode = Mode::insert;
        else if (currentSection == "VISUAL") mode = Mode::visual;
        else if (currentSection == "FIND") mode = Mode::find;

        // Bind the key
        command.bind(keyCode, actions[actionStr], mode);
    }
}