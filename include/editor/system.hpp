#pragma once

#include <string>
namespace editor {
  namespace system
  {
    /**
     * @brief Displays a confirmation popup asking the user if they want to exit without saving unsaved changes.
     * The user can select "Yes" or "No" using the arrow keys and confirm with Enter.
     * @return true if the user selects "Yes" (exit without saving), false otherwise.
     */
    bool confirm_exit();

    /**
     * @brief Exits the IDE application.
     * If there are unsaved changes, it prompts the user for confirmation via the confirm_exit() function.
     * If the user confirms, the IDE exits and ncurses mode is terminated.
     */
    void exit_ide();

    /**
     * @brief Displays a help menu with basic instructions for using the IDE.
     * The menu includes key bindings and descriptions for normal mode commands.
     * The user can exit the menu by pressing the ESC key.
     */
    void helpMenu();

    /**
     * @brief Switches the editor mode to "command" mode, positioning the cursor accordingly.
     */
    void change2command();

    /**
     * @brief Displays a form to input a string.
     * The user can input a string and confirm with Enter or cancel with ESC.
     * @param label The label for the input form.
     * @return The input string.
     */
    std::string text_form(const std::string& label);

    /**
     * @brief Switches the editor mode to "normal" mode.
     */
    void change2normal();

    /**
     * @brief Switches the editor mode to "insert" mode.
     */
    void change2insert();

    /**
     * @brief Switches the editor mode to "visual" mode.
     */
    void change2visual();

    /**
     * @brief Switches the editor mode to "find" mode.
     */
    void change2find();

    /**
     * @brief Restores the previous state of the editor.
     */
    void restore();

    // Utility function to print a message directly to the terminal (outsmvimStarter of ncurses mode)
    void print_to_terminal(int message);

    void switch_to_next_buffer();

    void switch_to_previous_buffer();

    void new_buffer();

    void resize();

  };
}