#include "../include/bufferManager.hpp"
#include "../include/windowManager.hpp" // Now we include it here
#include "../include/errorHandler.hpp"
#include "../include/globals/mvimResources.h" // Needed to sync global vars
#include <ncursesw/ncurses.h>
#include <stdexcept>
#include <unistd.h> // For close()

BufferManager& BufferManager::instance() {
    static BufferManager instance;
    return instance;
}

int BufferManager::getBufferCount() {
    return buffer_count;
}

void BufferManager::create_buffer(const std::string& name) {
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

    buffer.lock_fd = -1;

    buffer_count++;

    update_all_buffers_dimensions();
}

std::vector<BufferManager::BufferStructure*> BufferManager::get_all_buffers() {
    std::vector<BufferStructure*> active_buffers;
    for (int i = 0; i < buffer_count; ++i) {
        active_buffers.push_back(&buffers[i]);
    }
    return active_buffers;
}

const std::map<std::string, WINDOW*>& BufferManager::get_bufferWindows() const {
    return windowManager.get_windows();
}

void BufferManager::set_active_buffer(int index) {
    if (index >= 0 && index < MAX_BUFFERS) {
        active_buffer_index = index;
    } else {
        throw std::out_of_range("Buffer index out of range");
    }
}

BufferManager::BufferStructure& BufferManager::get_active_buffer() {
    return buffers[active_buffer_index];
}

int BufferManager::get_active_buffer_index(){
    return active_buffer_index;
}

WINDOW* BufferManager::get_active_window() {
    return buffers[active_buffer_index].window;
}

BufferManager::BufferStructure& BufferManager::get_buffer(int index) {
    if (index >= 0 && index < MAX_BUFFERS) {
        return buffers[index];
    }
    throw std::out_of_range("Buffer index out of range");
}

BufferManager::BufferStructure* BufferManager::get_buffer_by_name(const std::string& name) {
    for (int i = 0; i < buffer_count; ++i) {
        if (buffers[i].name == name) {
            return &buffers[i];
        }
    }
    return nullptr;
}

std::string BufferManager::get_current_buffer_name() const {
    return buffers[active_buffer_index].name;
}

BufferManager::BufferStructure& BufferManager::next() {
    active_buffer_index = (active_buffer_index + 1) % buffer_count;
    return get_active_buffer();
}

BufferManager::BufferStructure& BufferManager::previous() {
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

void BufferManager::delete_buffer(int index) {
    if (index == -1) {
        index = active_buffer_index;
    }
    
    if (index < 0 || index >= buffer_count) {
        throw std::out_of_range("Buffer index out of range");
    }

    if (buffers[index].lock_fd != -1) {
        close(buffers[index].lock_fd);
        buffers[index].lock_fd = -1;
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

void BufferManager::syncSystemVarsFromBuffer() {
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

void BufferManager::syncBufferFromSystemVars() {
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

void BufferManager::update_all_buffers_dimensions() {
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

void BufferManager::switchToWindowBuffer(WINDOW* window) {
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

WindowManager& BufferManager::getWindowManager(){
    return windowManager;
}