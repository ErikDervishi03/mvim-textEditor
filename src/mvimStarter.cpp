#include "../include/mvimStarter.hpp"

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
  action::file::read(filename);    // Load file content
  screen.update();    // Update screen
  action::visual::highlight_keywords();    // Highlight keywords

  refresh();
  status = Status::saved;
}

// Utility function to print a message directly to the terminal (outsmvimStarter of ncurses mode)
static void print_to_terminal(int message)
{
  endwin();    // End ncurses mode
  std::cout << message << std::endl;    // Print message to terminal
  refresh();    // Re-enter ncurses mode
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

      // Highlight keywords after printing the buffer
      action::visual::highlight_keywords();

      // If in visual mode, highlight the selection
      if (mode == visual)
      {
        visual_end_row = pointed_row;
        visual_end_col = cursor.getX() + span + 1;
        action::visual::highlight_selected();          // Highlight selection in visual mode
      }
      else
      {
        visual_start_row = pointed_row;
        visual_start_col = cursor.getX() + span + 1;
      }

      // If in find mode, highlight found occurrences
      if (mode == find)
      {
        action::find::highlight_searched_word();          // Highlight found occurrences
      }

      // Restore cursor position after all operations
      cursor.restore(span);

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
    int colors[] = {COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE};

    // Definire le coppie di colori: ogni foreground con ogni background
    int pair_id = 0;
    for (int bg : colors) {
        for (int fg : colors) {
            init_pair(pair_id, fg, bg);
            pair_id++;
        }
    }
}

void mvimStarter::setKeyWordColor(color pColor)
{
  keyWordColor = pColor;
}

void mvimStarter::setCommentColor(color pColor)
{
  commentsColor = pColor;
}

void mvimStarter::setNumberRowsColor(color pColor)
{
  numberRowsColor = pColor;
}

void mvimStarter::setHighlightedTextColor(color pColor)
{
  highlightedTextColor = pColor;
}

void mvimStarter::setBracketsColor(color pColor)
{
  bracketsColor = pColor;
}

void mvimStarter::setPreprocessorColor(color pColor)
{
  preprocessorColor = pColor;
}

void mvimStarter::setHighlightedBgColor(color pColor)
{
  highlightedBgColor = pColor;
}

void mvimStarter::setBgColor(color pColor){
  bgColor = pColor;
}

void mvimStarter::setTextColor(color pColor){
  textColor = pColor;
}

void mvimStarter::setCursorColor(color pColor){
  cursorColor = pColor;
}

void mvimStarter::setColorSchema(struct colorSchema pColorSchema)
{
  setBgColor(pColorSchema.background);

  setKeyWordColor(get_pair_default(pColorSchema.keyWord));
  setCommentColor(get_pair_default(pColorSchema.comments));
  setNumberRowsColor(get_pair_default(pColorSchema.numberRows));
  setBracketsColor(get_pair_default(pColorSchema.brackets));
  setHighlightedBgColor(pColorSchema.highlightedBg);
  setHighlightedTextColor(get_pair(highlightedBgColor, pColorSchema.highlightedText));
  setPreprocessorColor(get_pair_default(pColorSchema.preprocessor));
  setTextColor(get_pair_default(pColorSchema.text));
  setCursorColor(pColorSchema.cursor);
}

void mvimStarter::setColorSchemaByName(const std::string& schemaName)
{
    if (schemaName == "default")
    {
        setColorSchema(
        {
            COLOR_BLUE,              // keyWord
            COLOR_YELLOW,         // numberRows
            COLOR_CYAN,             // comments
            COLOR_WHITE,     // highlightedText
            COLOR_RED,         // highlightedBg
            COLOR_YELLOW,           // brackets
            COLOR_MAGENTA, // preprocessorColor
            COLOR_WHITE,    // backgroundColor
            COLOR_BLACK,            // textColor
            COLOR_GREEN,        //cursor
        });
    }
    else if (schemaName == "dark")
    {
        setColorSchema(
        {
            COLOR_CYAN,         // keyWord
            COLOR_GREEN,     // numberRows
            COLOR_YELLOW,      // comments
            COLOR_BLACK,// highlightedText
            COLOR_RED,  // highlightedBg
            COLOR_MAGENTA,     // brackets
            COLOR_RED // preprocessorColor
        });
    }
    else if (schemaName == "light")
    {
        setColorSchema(
        {
            COLOR_BLACK,         // keyWord
            COLOR_WHITE,      // numberRows
            COLOR_BLUE,         // comments
            COLOR_YELLOW,// highlightedText
            COLOR_CYAN,    // highlightedBg
            COLOR_GREEN,        // brackets
            COLOR_RED  // preprocessorColor
        });
    }
    else if (schemaName == "pastel")
    {
        setColorSchema(
        {
            COLOR_MAGENTA,   // keyWord
            COLOR_CYAN,      // numberRows
            COLOR_YELLOW,    // comments
            COLOR_WHITE,     // highlightedText
            COLOR_GREEN,     // highlightedBg
            COLOR_BLUE,      // brackets
            COLOR_RED        // preprocessorColor
        });
    }
    else
    {
        // Optional: Handle unknown color schema
        std::cerr << "Unknown color scheme: " << schemaName << std::endl;
    }
}



void mvimStarter::setDefaults()
{
  setColorSchemaByName("default");
}


void mvimStarter::startBenchmark(std::string filename)
{
  auto start_time = std::chrono::high_resolution_clock::now();    // Start timing

  action::file::read(filename);    // Load file content
  auto end_time = std::chrono::high_resolution_clock::now();    // End timing
  std::chrono::duration<double, std::milli> load_time = end_time - start_time;    // Get load time in milliseconds

  std::cout << "Time taken to load the file: " << load_time.count() << " ms" << std::endl;
  std::cout << "Benchmarking mode: Exiting mvimStarter after loading." << std::endl;
  exit(0);    // Exit the program after showing benchmark results
}
