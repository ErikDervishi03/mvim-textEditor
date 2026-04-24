#pragma once

namespace editor {
  namespace file
  {
    /**
     * @brief Saves the current buffer content to a file.
     * If the file name has not been provided, it prompts the user to input a file name.
     * The buffer is then written to the specified file if a valid file name is provided.
     * If saving is successful, the status is set to "saved".
     */
    void save();

    /**
     * @brief Reads the contents of a file into the buffer.
     * Checks if the file exists, if it's a regular file, and if the file size is within a certain limit.
     * If the file is valid, its content is read line by line into the buffer.
     * If the file can't be opened, an error is printed.
     * @param file_name The name of the file to read.
     */
    void read(std::string file_name);

    /**
     * @brief Displays a file selection menu to choose a file to open.
     * The user can navigate through the files and directories using arrow keys and select a file to open.
     * If the selected item is a directory, it navigates into the directory. If it's a file, it opens the file.
     * The user can exit the menu by pressing the ESC key.
     */
    void file_selection_menu();
  };
}