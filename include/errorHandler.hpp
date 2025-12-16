#pragma once
#include <string>
#include <ncurses.h> 
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
        int color_pair = 1; // Default to standard pair (usually red/error)

        switch (level) {
            case ErrorLevel::INFO:    
                prefix = "[INFO] "; 
                color_pair = 2; // Assuming pair 2 is Green or similar
                break;
            case ErrorLevel::WARNING: 
                prefix = "[WARN] "; 
                color_pair = 3; // Assuming pair 3 is Yellow
                break;
            case ErrorLevel::ERROR:   
                prefix = "[ERR] "; 
                color_pair = 1; // Red
                break;
            case ErrorLevel::FATAL:   
                prefix = "[FATAL] "; 
                break;
        }

        // FATAL errors must still crash the program safely
        if (level == ErrorLevel::FATAL) {
            endwin(); 
            std::cerr << prefix << message << std::endl;
            std::exit(1);
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