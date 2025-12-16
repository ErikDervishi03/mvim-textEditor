#include "../include/mvimStarter.hpp"
#include <ncurses.h>
#include <ostream>
#include "../include/bufferManager.hpp"

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
  BufferManager::instance().create_buffer("main");
  BufferManager::instance().syncSystemVarsFromBuffer();
  is_undoing = false;
  
  initialize_ncurses();    // Initialize ncurses first (CRITICAL)
  
  pointed_file = "";
  status = Status::saved;
  setDefaults();

  // Load config NOW, after the screen is ready to display errors
  _command.loadConfig(".mvimrc"); 

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

  BufferManager::instance().create_buffer("main");
  BufferManager::instance().syncSystemVarsFromBuffer();
  
  initialize_ncurses();    // Initialize ncurses first (CRITICAL)
  setDefaults();

  // Load config NOW, after the screen is ready to display errors
  _command.loadConfig(".mvimrc");

  is_undoing = false;
  status = Status::saved;
  pointed_file = filename;

  cursor.restore(span);
  editor::file::read(filename);
  screen.update();
  mvimService.run();
  wrefresh(pointed_window);
}

void mvimStarter::updateVar()
{
  getmaxyx(pointed_window, max_row, max_col);
  max_col = max_col- span - 1;
}

// Run the mvimStarter main loop
void mvimStarter::run()
{
  wbkgd(pointed_window, COLOR_PAIR(get_pair(bgColor, cursorColor)));
  homeScreen();    // Show initial screen if no file is loaded

  cursor.restore(span);
  
  while (true)
  {
    // Set a timeout (e.g., 200ms) so wgetch doesn't block forever.
    // This allows us to refresh the status bar to clear expired messages.
    wtimeout(pointed_window, 200);

    int input = wgetch(pointed_window);
    
    if (input != ERR)
    {
      // Cancella il contenuto della finestra attualmente puntata
      werase(pointed_window);  

      // Esegue il comando dell'utente
      _command.execute(input);

      // Aggiorna il contenuto dello schermo
      screen.update();

      // Reimposta il colore di sfondo della finestra principale
      wbkgd(pointed_window, COLOR_PAIR(get_pair(bgColor, cursorColor)));

      // Aggiorna le variabili dello stato attuale
      updateVar();

      // Esegue i servizi necessari
      mvimService.run();

      // Ripristina la posizione del cursore
      cursor.restore(span);

      // Pulisce e aggiorna tutte le finestre gestite da BufferManager
      const auto& buffers = BufferManager::instance().get_all_buffers();
      for (const auto& buffer : buffers) {
          if (buffer->window == pointed_window) continue;

          // Cancella ogni finestra per evitare residui di testo
          werase(buffer->window);

          // Stampa il contenuto del buffer nella finestra
          print_bufferStructure(buffer);

          // Aggiorna la finestra per mostrare i nuovi contenuti
          wrefresh(buffer->window);
      }

      // Aggiorna la finestra attualmente puntata
      wrefresh(pointed_window);
    }
    else 
    {
      // If no key was pressed (timeout), we still check the status bar.
      // This ensures that error messages disappear automatically after 3 seconds.
      screen.draw_status_bar();
      wrefresh(pointed_window);
    }
  }
}

void mvimStarter::print_bufferStructure(BufferManager::BufferStructure* buffer){
  if (buffer != nullptr) {
    screen.print_buffer(
        buffer->tBuffer.get_buffer(),  
        buffer->window,                        
        buffer->starting_row,          
        buffer->starting_col,          
        buffer->max_col                
    );
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

    // Make sure this wait is blocking (or handles ERR, but blocking is fine for intro)
    wtimeout(pointed_window, -1); 
    int input = wgetch(pointed_window);      // Wait for user input
    
    _command.execute(input);
    werase(pointed_window);
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



