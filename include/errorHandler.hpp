#pragma once
#include <iostream>
#include <ncurses.h>
#include <string>
#include <cstdlib> // for exit()

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
        switch (level) {
            case ErrorLevel::INFO:    prefix = "[INFO] "; break;
            case ErrorLevel::WARNING: prefix = "[WARN] "; break;
            case ErrorLevel::ERROR:   prefix = "[ERR] "; break;
            case ErrorLevel::FATAL:   prefix = "[FATAL] "; break;
        }

        // Print to standard error
        endwin(); 
        std::cerr << prefix << message << std::endl;
        refresh();

        // If it's a fatal error, exit the program immediately
        if (level == ErrorLevel::FATAL) {
            std::exit(1);
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