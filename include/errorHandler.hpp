#pragma once
#include <string>
#include <ncursesw/ncurses.h> 
#include <iostream>
#include "screen.hpp"

enum class ErrorLevel {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class ErrorHandler {
public:
    static ErrorHandler& instance() {
        static ErrorHandler instance;
        return instance;
    }

    void report(ErrorLevel level, const std::string& message) {
        std::string prefix;
        int color_pair = 1; // Default red

        switch (level) {
            case ErrorLevel::INFO:    
                prefix = "[INFO] "; 
                color_pair = 2; // Green 
                break;
            case ErrorLevel::WARNING: 
                prefix = "[WARN] "; 
                color_pair = 3; // Yellow
                break;
            case ErrorLevel::ERROR:   
                prefix = "[ERR] "; 
                color_pair = 1; // Red
                break;
            case ErrorLevel::FATAL:   
                prefix = "[FATAL] "; 
                break;
        }

        if (level == ErrorLevel::FATAL) {
            endwin(); 
            fprintf(stderr, "\033[?1049l\033[?1002l\033[0m\r\n");
            std::cerr << prefix << message << std::endl;
            std::_Exit(1);
        }
        else {
            // Send the message to the status bar
            Screen::getScreen().set_status_message(prefix + message, color_pair);
            
            // Force an immediate update so the user sees it even if they aren't typing
            Screen::getScreen().update();
        }
    }

private:
    // Singleton pattern: private constructor
    ErrorHandler() = default;
    ~ErrorHandler() = default;
    
    // Delete copy constructor and assignment operator
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
};