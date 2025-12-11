#include "../include/configParser.hpp"
#include "../include/command.hpp"
#include "../include/editor.hpp"
#include <fstream>
#include <algorithm>
#include <ncurses.h> 

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
    
    if (key.substr(0, 5) == "Ctrl-" && key.length() == 6) {
        return ctrl(key[5]);
    }

    static std::map<std::string, int> specialKeys = {
        {"KEY_UP", KEY_UP}, {"KEY_DOWN", KEY_DOWN},
        {"KEY_LEFT", KEY_LEFT}, {"KEY_RIGHT", KEY_RIGHT},
        {"KEY_BACKSPACE", KEY_BACKSPACE}, {"KEY_ENTER", 10},
        {"TAB", 9}, {"ESC", 27}
    };

    if (specialKeys.find(key) != specialKeys.end()) {
        return specialKeys[key];
    }

    return ERR; 
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
        {"paste_visual", editor::modify::paste_in_visual}, 

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
        {"select_all", editor::visual::select_all},

        // Find specific
        {"find_next", editor::find::go_to_next_occurrence},
        {"find_prev", editor::find::go_to_previous_occurrence},
        {"replace", editor::modify::replace}
    };
}

void ConfigParser::loadKeyBindings(Command& command, const std::string& filename) {
    std::vector<std::string> searchPaths = {
        filename,                                   // Current working directory (e.g., .mvimrc)
        "/usr/local/share/mvim/" + filename,        // System install path
        "/usr/share/mvim/" + filename               // Fallback system path
    };

    std::string configPath = "";

    // Iterate to find the first existing file
    for (const auto& path : searchPaths) {
        if (std::filesystem::exists(path)) {
            configPath = path;
            break;
        }
    }

    // If no file found, exit (or optionally log a warning)
    if (configPath.empty()) {
        ErrorHandler::instance().report(ErrorLevel::WARNING, "Configuration file not found.");
        return;
    }

    std::ifstream file(configPath);

    if (!file.is_open()) return;

    std::string line;
    int lineNumber = 0;
    std::string currentSection = "NORMAL";
    auto actions = getActionMap();

    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            std::transform(currentSection.begin(), currentSection.end(), currentSection.begin(), ::toupper);
            
            if (currentSection != "NORMAL" && currentSection != "INSERT" && 
                currentSection != "VISUAL" && currentSection != "FIND") {
                ErrorHandler::instance().report(ErrorLevel::WARNING, 
                    "Config Error Line " + std::to_string(lineNumber) + ": Unknown section [" + currentSection + "]");
                currentSection = "INVALID";
            }
            continue;
        }

        if (currentSection == "INVALID") continue;

        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) continue;

        std::string keyStr = trim(line.substr(0, delimiterPos));
        std::string actionStr = trim(line.substr(delimiterPos + 1));

        int keyCode = parseKey(keyStr);
        if (keyCode == ERR) {
            ErrorHandler::instance().report(ErrorLevel::WARNING, 
                "Config Error Line " + std::to_string(lineNumber) + ": Unknown key '" + keyStr + "'");
            continue;
        }

        if (actions.find(actionStr) == actions.end()) {
            ErrorHandler::instance().report(ErrorLevel::WARNING, 
                "Config Error Line " + std::to_string(lineNumber) + ": Unknown action '" + actionStr + "'");
            continue;
        }

        Mode mode = Mode::normal;
        if (currentSection == "INSERT") mode = Mode::insert;
        else if (currentSection == "VISUAL") mode = Mode::visual;
        else if (currentSection == "FIND") mode = Mode::find;

        command.bind(keyCode, actions[actionStr], mode);
    }
}