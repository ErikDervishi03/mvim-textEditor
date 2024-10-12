#include "../include/mvimStarter.hpp"

// Define constants and global variables
const char * mvim_logo = R"(
                      _             
                     (_)            
  _ __ ___   __   __  _   _ __ ____   
 | '_ ` _ \  \ \ / / | | | '_ ` _  |
 | | | | | |  \ V /  | | | | | | | | 
 |_| |_| |_|   \_/   |_| |_| |_| |_| 
                                    
        press a key to start
)";

// Constructor implementations
mvimStarter::mvimStarter() : screen(Screen::getScreen()), benchmark(false) {
    initialize_ncurses();  // Initialize ncurses and colors
    pointed_file = "";  
    status = Status::saved;
    setDefaults();
}

mvimStarter::mvimStarter(std::string filename, bool benchmark) 
    : screen(Screen::getScreen()), benchmark(benchmark) {
    if (benchmark) {
        startBenchmark(filename);
        return;
    }

    initialize_ncurses();  // Initialize ncurses and colors
    setDefaults();

    pointed_file = filename;
    cursor.restore(span);  // Restore cursor position
    action::file::read(filename);  // Load file content
    screen.update();  // Update screen
    action::visual::highlight_keywords();  // Highlight keywords

    refresh();
    status = Status::saved;
}

// Run the mvimStarter main loop
void mvimStarter::run() {
    homeScreen();  // Show initial screen if no file is loaded

    cursor.restore(span);
    while (true) {
        int input = getch();
        if (input != ERR) {
            erase();  // Clear the screen

            // Execute command based on input (modify the buffer if necessary)
            _command.execute(input);

            // Print the updated buffer
            screen.update();

            // Highlight keywords after printing the buffer
            action::visual::highlight_keywords();

            // If in visual mode, highlight the selection
            if (mode == visual) {
                visual_end_row = pointed_row;
                visual_end_col = cursor.getX() + span + 1;
                action::visual::highlight_selected();  // Highlight selection in visual mode
            } else {
                visual_start_row = pointed_row;
                visual_start_col = cursor.getX() + span + 1;
            }

            // If in find mode, highlight found occurrences
            if (mode == find) {
                action::find::highlight_visible_occurrences();  // Highlight found occurrences
            }

            // Restore cursor position after all operations
            cursor.restore(span);

            // Refresh screen to display results
            refresh();
        }
    }
}

// Show the initial welcome screen
void mvimStarter::homeScreen() {
    if (pointed_file.empty()) {
        curs_set(0);

        // Calculate dimensions for centering the text
        int text_width = 40;  // Width of the text block
        int text_height = 9;  // Height of the text block
        int screen_width = screen.get_width();
        int screen_height = screen.get_height();
        int start_x = (screen_width - text_width) / 2;
        int start_y = (screen_height - text_height) / 2;

        // Print the welcome message
        screen.print_multiline_string(start_y, start_x, mvim_logo);

        int input = getch();  // Wait for user input
        _command.execute(input);
        erase();
        screen.update();  // Update screen
        curs_set(1);  // Restore cursor visibility
    }
}

// Utility function to print a message directly to the terminal (outsmvimStarter of ncurses mode)
static void print_to_terminal(int message) {
    endwin();  // End ncurses mode
    std::cout << message << std::endl;  // Print message to terminal
    refresh();  // Re-enter ncurses mode
}

// Helper function to initialize ncurses and color pairs
void mvimStarter::initialize_ncurses() {
    screen.start();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
}

void mvimStarter::setKeyWordColor(color pColor){
    keyWordColor = pColor;
}

void mvimStarter::setCommentColor(color pColor){
    commentsColor = pColor;
}

void mvimStarter::setNumberRowsColor(color pColor){
    numberRowsColor = pColor;
}

void mvimStarter::setHighlightedTextColor(color pColor){
    highlightedTextColor = pColor;
}
void mvimStarter::setBracketsColor(color pColor){
    bracketsColor = pColor;
}

void mvimStarter::setColorSchema(struct colorSchema pColorSchema){
    setKeyWordColor(pColorSchema.keyWord);
    setCommentColor(pColorSchema.comments);
    setNumberRowsColor(pColorSchema.numberRows);
    setBracketsColor(pColorSchema.brackets);
    setHighlightedTextColor(pColorSchema.highlightedText);
}

void mvimStarter::setDefaults(){
    setColorSchema({2, 3, 4, 1,3});
}


void mvimStarter::startBenchmark(std::string filename){
    auto start_time = std::chrono::high_resolution_clock::now();  // Start timing

    action::file::read(filename);  // Load file content
    auto end_time = std::chrono::high_resolution_clock::now();  // End timing
    std::chrono::duration<double, std::milli> load_time = end_time - start_time;  // Get load time in milliseconds

    std::cout << "Time taken to load the file: " << load_time.count() << " ms" << std::endl;
    std::cout << "Benchmarking mode: Exiting mvimStarter after loading." << std::endl;
    exit(0);  // Exit the program after showing benchmark results
}