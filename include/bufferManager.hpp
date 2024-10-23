#include "globals/mode.h"
#include "globals/status.h"
#include "cursor.hpp"
#include "textBuffer.hpp"
#include "windowManager.hpp"
#include <array>
#include <stdexcept>

// Maximum number of buffers
const int MAX_BUFFERS = 4;

class BufferManager {
public:
    struct BufferStructure {
        Cursor cursor;             ///< Cursor position and state.
        textBuffer tBuffer;        ///< Text buffer associated with this buffer structure.
        Mode mode;                 ///< Mode settings for the buffer.
        Status status;             ///< Status of the buffer (e.g., modified, read-only).

        size_t max_row;           ///< Maximum number of rows in the buffer.
        size_t max_col;           ///< Maximum number of columns in the buffer.
        size_t pointed_row;       ///< Current pointed row in the buffer.
        size_t starting_row;      ///< Starting row for the visual area.
        size_t pointed_col;       ///< Current pointed column in the buffer.
        size_t starting_col;      ///< Starting column for the visual area.

        int visual_start_row;     ///< Start row for visual selection.
        int visual_start_col;     ///< Start column for visual selection.
        int visual_end_row;       ///< End row for visual selection.
        int visual_end_col;       ///< End column for visual selection.

        std::string pointed_file; ///< Currently pointed file name.
        std::string command_buffer; ///< Buffer for command input.
        std::string copy_paste_buffer; ///< Buffer for copy/paste operations.

        WINDOW* window;           ///< Ncurses window associated with this buffer.
        std::string name;         ///< Name of the buffer for identification.
    };

    /// Default constructor initializing the active buffer index and count.
    BufferManager() : active_buffer_index(0), buffer_count(0) {}

    /// Default destructor.
    ~BufferManager() {}

    int getBufferCount() {
        return buffer_count;
    }

    /**
     * @brief Creates a new buffer and assigns a window.
     *
     * Throws an exception if the maximum buffer limit is reached.
     * 
     * @param name The name of the buffer to create, used for window creation.
     */
    void create_buffer(const std::string& name) {
        if (buffer_count >= MAX_BUFFERS) {
            throw std::runtime_error("Max buffer limit reached.");
        }
        // Create the window using WindowManager
        windowManager.create_window(name);
        buffers[buffer_count].window = windowManager.get_window(name);
        buffers[buffer_count].name = name;            // Store the name in the buffer structure
        buffer_count++;
    }

    /**
     * @brief Sets the active buffer by index.
     * 
     * Throws an out_of_range exception if the index is invalid.
     * 
     * @param index The index of the buffer to set as active.
     */
    void set_active_buffer(int index) {
        if (index >= 0 && index < MAX_BUFFERS) {
            active_buffer_index = index;
        } else {
            throw std::out_of_range("Buffer index out of range");
        }
    }

    /**
     * @brief Returns the currently active buffer.
     * 
     * @return Reference to the active BufferStructure.
     */
    BufferStructure& get_active_buffer() {
        return buffers[active_buffer_index];
    }

    /**
     * @brief Returns the window associated with the currently active buffer.
     * 
     * @return Pointer to the active WINDOW.
     */
    WINDOW* get_active_window() {
        return buffers[active_buffer_index].window;
    }

    /**
     * @brief Returns a specific buffer by index.
     * 
     * Throws an out_of_range exception if the index is invalid.
     * 
     * @param index The index of the buffer to return.
     * @return Reference to the requested BufferStructure.
     */
    BufferStructure& get_buffer(int index) {
        if (index >= 0 && index < MAX_BUFFERS) {
            return buffers[index];
        }
        throw std::out_of_range("Buffer index out of range");
    }

    /**
     * @brief Returns a specific buffer name by its associated name.
     * 
     * Throws a runtime_error exception if the name is not found.
     * 
     * @param name The name of the buffer to return.
     * @return The name of the requested buffer.
     */
    BufferManager::BufferStructure* get_buffer_by_name(const std::string& name) {
        for (int i = 0; i < buffer_count; ++i) {
            if (buffers[i].name == name) {
                return &buffers[i]; // Return a pointer to the found buffer
            }
        }
        return nullptr; // Return nullptr if not found
    }


    /**
     * @brief Returns the name of the currently pointed buffer.
     * 
     * @return The name of the currently pointed buffer.
     */
    std::string get_current_buffer_name() const {
        return buffers[active_buffer_index].name;
    }

    /**
     * @brief Moves to the next buffer in a circular manner and returns it.
     * 
     * Increases the active buffer index. If the active buffer index reaches
     * the count of existing buffers, it wraps around to 0.
     * 
     * @return Reference to the new active BufferStructure.
     */
    BufferStructure& next() {
        active_buffer_index = (active_buffer_index + 1) % buffer_count;
        return get_active_buffer();  // Return the new active buffer
    }

    BufferStructure& previous() {
        // Guard against empty buffer list
        if (buffer_count == 0) {
            throw std::runtime_error("No buffers available");
        }

        // If the active buffer index is 0, wrap around to the last buffer
        if (active_buffer_index == 0) {
            active_buffer_index = buffer_count - 1; // Set to the last buffer
        } else {
            active_buffer_index--; // Decrease the index normally
        }
        return get_active_buffer();  // Return the new active buffer
    }


private:
    WindowManager windowManager;                          ///< Window manager for creating and managing windows.
    std::array<BufferStructure, MAX_BUFFERS> buffers;    ///< Array of buffers managed by this BufferManager.
    int active_buffer_index;                              ///< Index of the currently active buffer.
    int buffer_count;                                     ///< Current count of buffers created.
};
