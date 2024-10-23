#include "../include/editor.hpp"

namespace fs = std::filesystem;


/**
 * Displays a form for inputting a filename.
 * The form consists of a simple window where the user can type a filename, with support for backspace and exiting via the ESC key.
 * Once the filename is entered and confirmed, it is stored in the global variable `pointed_file`.
 */



// Save function
void editor::file::save()
{

  if (pointed_file.empty())
  {
    pointed_file = editor::system::text_form("Insert file name:");        // Prompt for filename if not set
  }

  // Only save if a filename was entered
  if (!pointed_file.empty())
  {
    status = Status::saved;
    std::ofstream myfile(pointed_file);
    if (!myfile.is_open())
    {
      std::cerr << "Failed to open file for writing: " << pointed_file << "\n";
      return;
    }

    editor::system::restore();

    for (int i = 0; i < buffer.getSize(); i++)
    {
      myfile << buffer.get_buffer()[i] << "\n";
    }
    myfile.close();
  }
  else
  {
    std::cerr << "No file name provided. Save aborted.\n";
  }
}

bool is_readable(std::string file_name)
{

  fs::perms file_perms = fs::status(file_name).permissions();

  if (fs::exists(file_name) &&
      fs::is_regular_file(file_name) &&
      fs::file_size(file_name) < 1000000 &&
      fs::file_size(file_name) > 0 &&
      (file_perms& fs::perms::owner_read) != fs::perms::none &&
      (file_perms& fs::perms::group_read) != fs::perms::none &&
      (file_perms& fs::perms::others_read) != fs::perms::none)
  {

    return true;

  }
  else
  {
    std::cerr << "File is not readable or does not exist.\n";
    return false;
  }
}

// Read function
void editor::file::read(std::string file_name)
{

  // If the status is unsaved, prompt for confirmation
  if (status == Status::unsaved)
  {
    bool confirmed = editor::system::confirm_exit();
    if (!confirmed)
    {
      // If the user selects "No", return to the editor
      return;
    }
  }

  if (is_readable(file_name))
  {
    std::ifstream myfile(file_name);
    if (!myfile.is_open())
    {
      std::cerr << "Can't open: " << file_name << "\n";
      return;
    }

    status = Status::saved;

    pointed_file = file_name;
    pointed_row = 0;
    starting_row = 0;
    cursor.set(0, 0);

    buffer.clear();
    std::string line;

    while (std::getline(myfile, line))
    {
      buffer.push_back(line);
    }

    myfile.close();
  }
  else if (file_name.empty())
  {
    buffer.restore();
  }
}


void editor::file::file_selection_menu()
{
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);    // Hide the cursor

  fs::path current_path = fs::current_path();    // Start from the current directory
  std::vector<std::string> files;
  int selected = 0;
  bool exit_menu = false;

  // Helper function to load directory contents
  auto load_directory = [&]()
    {
      files.clear();
      files.push_back("..");   // Option to go to the parent directory

      for (const auto& entry : fs::directory_iterator(current_path))
      {
        files.push_back(entry.path().filename().string());
      }
      std::sort(files.begin(), files.end());   // Sort files alphabetically
    };

  // Load initial directory
  load_directory();

  int height, width;
  getmaxyx(stdscr, height, width);

  while (!exit_menu)
  {
    clear();      // Clear the screen

    // Display the full path of the current directory
    std::string full_path = current_path.string();
    mvprintw(0, 0, "Current Path: %s", full_path.c_str());

    // Display the list of files
    for (size_t i = 0; i < files.size(); ++i)
    {
      if (i == selected)
      {
        attron(A_REVERSE);          // Highlight selected item
      }
      mvprintw(2 + i, 2, "%s", files[i].c_str());
      if (i == selected)
      {
        attroff(A_REVERSE);         // Remove highlight
      }
    }

    // Refresh the screen
    refresh();

    // Handle user input for navigation
    int ch = getch();

    switch (ch)
    {
    case KEY_UP:
    {
      selected = (selected > 0) ? selected - 1 : files.size() - 1;
      break;
    }
    case KEY_DOWN:
    {
      selected = (selected + 1) % files.size();
      break;
    }
    case '\n':          // Enter key to select
    {
      if (files[selected] == "..")
      {
        // Go back to the parent directory
        current_path = current_path.parent_path();
        load_directory();
        selected = 0;              // Reset selection to the top
      }
      else
      {
        // Check if it's a directory or file
        fs::path chosen_path = current_path / files[selected];
        if (fs::is_directory(chosen_path) && access(chosen_path.c_str(), R_OK | W_OK) == 0)
        {
          current_path = fs::absolute(chosen_path);
          load_directory();
          selected = 0;                // Reset selection to the top
        }
        else
        {
          // It's a file, call the `read` function of `editor::file`
          editor::file::read(chosen_path.c_str());
          exit_menu = true;                // Exit menu after opening the file
        }
      }
      break;
    }
    case 27:          // ESC key to exit the menu
    {
      exit_menu = true;
      break;
    }
    }
  }
  curs_set(1);
  erase();
  endwin();
}
