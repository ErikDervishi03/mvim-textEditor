#pragma once
#include "errorHandler.hpp"
#include "globals/mode.h"
#include "globals/status.h"
#include "globals/mvimResources.h"
#include "cursor.hpp"
#include "textBuffer.hpp"
#include "windowManager.hpp"
#include <array>
#include <ncurses.h>
#include <stdexcept>

// Maximum number of buffers
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

        int visual_start_row;
        int visual_start_col;
        
        std::string pointed_file;
        std::string command_buffer;
        std::string copy_paste_buffer;

        WINDOW* window;
        std::string name;
    };

    static BufferManager& instance() {
        static BufferManager instance;
        return instance;
    }

    int getBufferCount() {
        return buffer_count;
    }

    void create_buffer(const std::string& name) {
        if (buffer_count >= MAX_BUFFERS) {
            throw std::runtime_error("Max buffer limit reached.");
        }

        BufferStructure& buffer = buffers[buffer_count];
        windowManager.create_window(name);
        buffer.window = windowManager.get_window(name);
        buffer.name = name;

        buffer.cursor = Cursor();
        cursor.set(0, 0);

        buffer.tBuffer = textBuffer();
        buffer.mode = Mode::insert;
        buffer.status = Status::unsaved;

        getmaxyx(buffer.window, buffer.max_row, buffer.max_col);
        buffer.max_row -= 1; // Reserve 1 row for File Header
        buffer.max_col = buffer.max_col - span - 1;

        buffer.pointed_row = 0;
        buffer.starting_row = 0;
        buffer.pointed_col = 0;
        buffer.starting_col = 0;
        buffer.command_buffer.clear();
        buffer.copy_paste_buffer.clear();

        buffer_count++;

        update_all_buffers_dimensions();
    }

    std::vector<BufferStructure*> get_all_buffers() {
        std::vector<BufferStructure*> active_buffers;
        for (int i = 0; i < buffer_count; ++i) {
            active_buffers.push_back(&buffers[i]);
        }
        return active_buffers;
    }

    const std::map<std::string, WINDOW*>& get_bufferWindows() const {
        return windowManager.get_windows();
    }

    void set_active_buffer(int index) {
        if (index >= 0 && index < MAX_BUFFERS) {
            active_buffer_index = index;
        } else {
            throw std::out_of_range("Buffer index out of range");
        }
    }

    BufferStructure& get_active_buffer() {
        return buffers[active_buffer_index];
    }

    int get_active_buffer_index(){
        return active_buffer_index;
    }

    WINDOW* get_active_window() {
        return buffers[active_buffer_index].window;
    }

    BufferStructure& get_buffer(int index) {
        if (index >= 0 && index < MAX_BUFFERS) {
            return buffers[index];
        }
        throw std::out_of_range("Buffer index out of range");
    }

    BufferManager::BufferStructure* get_buffer_by_name(const std::string& name) {
        for (int i = 0; i < buffer_count; ++i) {
            if (buffers[i].name == name) {
                return &buffers[i];
            }
        }
        return nullptr;
    }

    std::string get_current_buffer_name() const {
        return buffers[active_buffer_index].name;
    }

    BufferStructure& next() {
        active_buffer_index = (active_buffer_index + 1) % buffer_count;
        return get_active_buffer();
    }

    BufferStructure& previous() {
        if (buffer_count == 0) {
            throw std::runtime_error("No buffers available");
        }
        if (active_buffer_index == 0) {
            active_buffer_index = buffer_count - 1;
        } else {
            active_buffer_index--;
        }
        return get_active_buffer();
    }

    void delete_buffer(int index = -1) {
        if (index == -1) {
            index = active_buffer_index;
        }
        
        if (index < 0 || index >= buffer_count) {
            throw std::out_of_range("Buffer index out of range");
        }

        // Kill the window (resizes other windows physically)
        if (windowManager.kill_window(buffers[index].name) == EXIT_FAILURE) {
            throw std::runtime_error("Failed to destroy window for buffer: " + buffers[index].name);
        }

        // Shift buffers
        for (int i = index; i < buffer_count - 1; ++i) {
            buffers[i] = buffers[i + 1];
        }

        buffer_count--;

        if (active_buffer_index >= buffer_count) {
            active_buffer_index = buffer_count - 1;
        }

        // Force recalculation of text dimensions for all remaining buffers
        update_all_buffers_dimensions();
    }

    void syncSystemVarsFromBuffer() {
        auto& activeBuffer = get_active_buffer();

        cursor = activeBuffer.cursor;
        buffer = activeBuffer.tBuffer;
        mode = activeBuffer.mode;
        status = activeBuffer.status;

        max_row = activeBuffer.max_row;
        max_col = activeBuffer.max_col;
        pointed_row = activeBuffer.pointed_row;
        starting_row = activeBuffer.starting_row;
        pointed_col = activeBuffer.pointed_col;
        starting_col = activeBuffer.starting_col;

        visual_start_row = activeBuffer.visual_start_row;
        visual_start_col = activeBuffer.visual_start_col;

        pointed_file = activeBuffer.pointed_file;
        command_buffer = activeBuffer.command_buffer;
        copy_paste_buffer = activeBuffer.copy_paste_buffer;

        pointed_window = activeBuffer.window;

        cursor.pointToWindow(pointed_window);
    }

    void syncBufferFromSystemVars() {
        auto& activeBuffer = get_active_buffer();

        activeBuffer.cursor = cursor;
        activeBuffer.tBuffer = buffer;
        activeBuffer.mode = mode;
        activeBuffer.status = status;

        activeBuffer.max_row = max_row;
        activeBuffer.max_col = max_col;
        activeBuffer.pointed_row = pointed_row;
        activeBuffer.starting_row = starting_row;
        activeBuffer.pointed_col = pointed_col;
        activeBuffer.starting_col = starting_col;

        activeBuffer.visual_start_row = visual_start_row;
        activeBuffer.visual_start_col = visual_start_col;

        activeBuffer.pointed_file = pointed_file;
        activeBuffer.command_buffer = command_buffer;
        activeBuffer.copy_paste_buffer = copy_paste_buffer;

        activeBuffer.window = pointed_window;

        activeBuffer.cursor.pointToWindow(pointed_window);
    }

    void update_all_buffers_dimensions() {
        for (int i = 0; i < buffer_count; ++i) {
            BufferStructure& buffer = buffers[i];
            if (buffer.window) {
                getmaxyx(buffer.window, buffer.max_row, buffer.max_col);
                wresize(buffer.window,  buffer.max_row, buffer.max_col);
                
                buffer.max_row -= 1; // Reserve 1 row for File Header
                buffer.max_col = buffer.max_col - span - 1;

                if (buffer.pointed_col < buffer.starting_col || 
                    buffer.pointed_col > buffer.starting_col + (buffer.max_col - 1)) {
                    if (buffer.pointed_col > buffer.max_col / 2) {
                        buffer.starting_col = buffer.pointed_col - buffer.max_col / 2;
                    } else {
                        buffer.starting_col = 0;
                    }
                }

                buffer.cursor.setX(buffer.pointed_col - buffer.starting_col);
            }
        }
    }

    void switchToWindowBuffer(WINDOW* window) {
        if (window == nullptr) {
            return;
        }

        // 1. Find the index of the buffer that owns this window
        int targetIndex = -1;
        for (int i = 0; i < buffer_count; ++i) {
            if (buffers[i].window == window) {
                targetIndex = i;
                break;
            }
        }

        // 2. If the buffer exists, execute the sync sequence
        if (targetIndex != -1) {
            // A. Save current UI state (sliders/inputs) to the PREVIOUS active buffer
            syncBufferFromSystemVars();

            // B. Switch the active buffer index in the backend
            set_active_buffer(targetIndex);

            // C. Load the NEW buffer's data into the UI system variables
            syncSystemVarsFromBuffer();
        } else {
            ErrorHandler::instance().report(ErrorLevel::ERROR, "switching to not existing buffer");
        }
    }

    WindowManager& getWindowManager(){
        return windowManager;
    }

private:
    WindowManager& windowManager = WindowManager::getInstance();
    std::array<BufferStructure, MAX_BUFFERS> buffers;
    int active_buffer_index;
    int buffer_count;

    BufferManager() = default;
    BufferManager(const BufferManager&) = delete;
    BufferManager& operator=(const BufferManager&) = delete;
};