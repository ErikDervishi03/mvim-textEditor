#include "../include/mvimStarter.hpp"
#include <ostream>


// Define constants and global variables
const char* mvim_logo =
  R"(
                      _             
                     (_)            
  _ __ ___   __   __  _   _ __ ____   
 | '_ ` _ \  \ \ / / | | | '_ ` _  |
 | | | | | |  \ V /  | | | | | | | | 
 |_| |_| |_|   \_/   |_| |_| |_| |_| 
                                    
        press a key to start
)";

// Constructor implementations
mvimStarter::mvimStarter() :
  screen(Screen::getScreen()), benchmark(false)
{
  initialize_ncurses();    // Initialize ncurses and colors
  pointed_file = "";
  status = Status::saved;
  setDefaults();
  mvimService.enableService("highlighting", editor::visual::highlight_keywords);
}

mvimStarter::mvimStarter(std::string filename, bool benchmark)
  : screen(Screen::getScreen()), benchmark(benchmark)
{
  if (benchmark)
  {
    startBenchmark(filename);
    return;
  }

  initialize_ncurses();    // Initialize ncurses and colors
  setDefaults();

  pointed_file = filename;
  cursor.restore(span);    // Restore cursor position
  editor::file::read(filename);    // Load file content
  screen.update();    // Update screen
  mvimService.run();
  refresh();
  status = Status::saved;
}

void mvimStarter::updateVar()
{
  getmaxyx(stdscr, max_row, max_col);
  max_col = max_col- span - 1;
  // If in visual mode, highlight the selection
  if (mode == visual)
  {
    visual_end_row = pointed_row;
    visual_end_col = pointed_col + span + 1;
  }
  else
  {
    visual_start_row = pointed_row;
    visual_start_col = pointed_col + span + 1;
  }
}

// Run the mvimStarter main loop
void mvimStarter::run()
{
  bkgd(COLOR_PAIR(get_pair(bgColor, cursorColor)));
  homeScreen();    // Show initial screen if no file is loaded

  cursor.restore(span);

  while (true)
  {
    int input = getch();
    if (input != ERR)
    {
      erase();        // Clear the screen

      // Execute command based on input (modify the buffer if necessary)
      _command.execute(input);

      // Print the updated buffer
      screen.update();

      bkgd(COLOR_PAIR(get_pair(bgColor, cursorColor)));

      updateVar();

      mvimService.run();

      // Restore cursor position after all operations
      cursor.restore(span);
      
        endwin();    // End ncurses mode
        std::cout << pointed_col << " " << starting_col << " " <<cursor.getX() << " " << max_col << std::endl;
        refresh();    // Re-enter ncurses mode

      // Refresh screen to display results
      refresh();
    }
  }
}

// Show the initial welcome screen
void mvimStarter::homeScreen()
{
  if (pointed_file.empty())
  {
    curs_set(0);

    // Calculate dimensions for centering the text
    int text_width = 40;      // Width of the text block
    int text_height = 9;      // Height of the text block
    int screen_width = screen.get_width();
    int screen_height = screen.get_height();
    int start_x = (screen_width - text_width) / 2;
    int start_y = (screen_height - text_height) / 2;

    // Print the welcome message
    screen.print_multiline_string(start_y, start_x, mvim_logo);

    int input = getch();      // Wait for user input
    _command.execute(input);
    erase();
    screen.update();      // Update screen
    curs_set(1);      // Restore cursor visibility
  }
}


// Helper function to initialize ncurses and color pairs
void mvimStarter::initialize_ncurses()
{
  screen.start();
  start_color();

  // Colori di sfondo da utilizzare
  int colors[] = { COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN,
                   COLOR_WHITE };

  // Definire le coppie di colori: ogni foreground con ogni background
  int pair_id = 0;
  for (int bg : colors)
  {
    for (int fg : colors)
    {
      init_pair(pair_id, fg, bg);
      pair_id++;
    }
  }
}


void mvimStarter::setDefaults()
{
  mvimColorManager.setColorSchemaByName("default");
}


void mvimStarter::startBenchmark(std::string filename)
{
  auto start_time = std::chrono::high_resolution_clock::now();    // Start timing

  editor::file::read(filename);    // Load file content
  auto end_time = std::chrono::high_resolution_clock::now();    // End timing
  std::chrono::duration<double, std::milli> load_time = end_time - start_time;    // Get load time in milliseconds

  std::cout << "Time taken to load the file: " << load_time.count() << " ms" << std::endl;
  std::cout << "Benchmarking mode: Exiting mvimStarter after loading." << std::endl;
  exit(0);    // Exit the program after showing benchmark results
}
