#include "editor.hpp"

class service {
public:
    // Constructor to initialize the modes
    service() {
        // Initialize default services
        
        modes.emplace_back("find", true, []() { editor::find::highlight_searched_word(); });
        modes.emplace_back("highlighting", true, []() { editor::visual::highlight_keywords(); }); 
        modes.emplace_back("visual", true, []() { editor::visual::highlight_selected(); });
    }

    // Enables a mode and sets the associated editor
    void enableService(const std::string& mode, std::function<void()> editor) {
        for (auto& m : modes) {
            if (m.name == mode) {
                m.enabled = true;  // Enable the mode
                m.editor = editor;  // Set the editor to execute
                return;
            }
        }
    }

    // Disables a mode
    void disableService(const std::string& mode) {
        for (auto& m : modes) {
            if (m.name == mode) {
                m.enabled = false;  // Disable the mode
                return;
            }
        }
    }

    // Checks if a mode is active
    bool isServiceEnabled(const std::string& mode) const {
        for (const auto& m : modes) {
            if (m.name == mode) {
                return m.enabled;  // Return true if it is active
            }
        }
        return false;
    }

    // Toggles the state of a mode
    void toggleMode(const std::string& mode) {
        for (auto& m : modes) {
            if (m.name == mode) {
                m.enabled = !m.enabled;  // Change the state
                return;
            }
        }
    }

    // Executes all editors of the active modes
    void run() {
        for (const auto& m : modes) {
            if (m.enabled && m.editor) {
                m.editor();  // Execute the associated editor
            }
        }
    }

private:
    // Struct representing a single service mode
    struct ServiceMode {
        std::string name;                      // Name of the mode
        bool enabled;                          // Status of the mode (active or not)
        std::function<void()> editor;         // editor to execute when the mode is active

        // Constructor to initialize the fields
        ServiceMode(const std::string& modeName, bool isEnabled = false, std::function<void()> modeeditor = nullptr)
            : name(modeName), enabled(isEnabled), editor(modeeditor) {}
    };

    std::vector<ServiceMode> modes;  // Vector of modes
};
