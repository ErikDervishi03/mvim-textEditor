#include "globals/mode.h"
#include "globals/status.h"
#include "cursor.hpp"
#include "textBuffer.hpp"
#include "windowManager.hpp"
#include <array>
#include <stdexcept>

// Massimo numero di buffer
const int MAX_BUFFERS = 4;

class BufferManager {
public:
    struct BufferStructure {
        Cursor cursor;
        textBuffer tBuffer;
        Mode mode;
        Status status;
        size_t max_row;
        size_t max_col;
        size_t pointed_row;
        size_t starting_row;
        size_t pointed_col;
        size_t starting_col;
        std::string pointed_file;
        std::string command_buffer;
        std::string copy_paste_buffer;
        int visual_start_row;
        int visual_start_col;
        int visual_end_row;
        int visual_end_col;

        WINDOW* window;
    };

    // Il costruttore prende in input un riferimento a WindowManager
    BufferManager() : active_buffer_index(0), buffer_count(0) {}

    ~BufferManager() {}

    // Crea un nuovo buffer e assegna una finestra
    void create_buffer(const std::string& name) {
        if (buffer_count >= MAX_BUFFERS) {
            throw std::runtime_error("Max buffer limit reached.");
        }
        // Crea la finestra utilizzando WindowManager
        windowManager.create_window(name);
        buffers[buffer_count].window = windowManager.get_window(name);
        buffer_count++;
    }

    // Imposta il buffer attivo
    void set_active_buffer(int index) {
        if (index >= 0 && index < MAX_BUFFERS) {
            active_buffer_index = index;
        } else {
            throw std::out_of_range("Buffer index out of range");
        }
    }

    // Restituisce il buffer attivo
    BufferStructure& get_active_buffer() {
        return buffers[active_buffer_index];
    }

    // Restituisce la finestra del buffer attivo
    WINDOW* get_active_window() {
        return buffers[active_buffer_index].window;
    }

    // Restituisce un buffer specifico
    BufferStructure& get_buffer(int index) {
        if (index >= 0 && index < MAX_BUFFERS) {
            return buffers[index];
        }
        throw std::out_of_range("Buffer index out of range");
    }

private:
    WindowManager windowManager; 
    std::array<BufferStructure, MAX_BUFFERS> buffers;  
    int active_buffer_index;  
    int buffer_count;  
};
