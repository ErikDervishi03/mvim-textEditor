#pragma once
#include <string>
#include <map>
#include <functional>
#include <vector>

// Forward declaration to avoid circular dependency issues
class Command;

class ConfigParser {
public:
    /**
     * @brief Loads key bindings from a configuration file and applies them to the Command object.
     * @param command Reference to the Command object to bind keys to.
     * @param filename Path to the configuration file (e.g., ".mvimrc").
     */
    static void loadKeyBindings(Command& command, const std::string& filename);

private:
    /**
     * @brief Parses a string representation of a key into its integer key code.
     * Supports single chars ('k'), Control keys ('Ctrl-s'), and special keys ('KEY_UP').
     */
    static int parseKey(const std::string& keyStr);

    /**
     * @brief Returns a map linking string action names to actual editor functions.
     */
    static std::map<std::string, std::function<void()>> getActionMap();
    
    // Helper to trim whitespace
    static std::string trim(const std::string& str);
};