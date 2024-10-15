#include "action.hpp"

class service {
public:
    // Constructor to initialize the modes
    service() {
        // Initialize default services
        modes.emplace_back("visual", true, []() { action::visual::highlight_selected(); });
        modes.emplace_back("find", true, []() { action::find::highlight_searched_word(); });
        modes.emplace_back("highlighting", true, []() { action::visual::highlight_keywords(); }); 
    }

    // Enables a mode and sets the associated action
    void enableService(const std::string& mode, std::function<void()> action) {
        for (auto& m : modes) {
            if (m.name == mode) {
                m.enabled = true;  // Enable the mode
                m.action = action;  // Set the action to execute
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

    // Executes all actions of the active modes
    void run() {
        for (const auto& m : modes) {
            if (m.enabled && m.action) {
                m.action();  // Execute the associated action
            }
        }
    }

private:
    // Struct representing a single service mode
    struct ServiceMode {
        std::string name;                      // Name of the mode
        bool enabled;                          // Status of the mode (active or not)
        std::function<void()> action;         // Action to execute when the mode is active

        // Constructor to initialize the fields
        ServiceMode(const std::string& modeName, bool isEnabled = false, std::function<void()> modeAction = nullptr)
            : name(modeName), enabled(isEnabled), action(modeAction) {}
    };

    std::vector<ServiceMode> modes;  // Vector of modes
};
