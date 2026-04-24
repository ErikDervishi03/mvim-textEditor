#pragma once
#include "globals/mode.h"
#include "globals/status.h"
#include "windowManager.hpp"
#include "cursor.hpp"
#include "textBuffer.hpp"
#include <array>
#include <vector>
#include <map>
#include <string>

// Forward declarations
struct _win_st;           
typedef struct _win_st WINDOW;

const int MAX_BUFFERS = 4;

class BufferManager {
public:
    struct BufferStructure {
        Cursor cursor;
        textBuffer tBuffer;
        Mode mode;
        Status status;

        size_t max_row, max_col;
        size_t pointed_row, starting_row;
        size_t pointed_col, starting_col;
        int visual_start_row, visual_start_col;
        
        std::string pointed_file;
        std::string command_buffer;
        std::string copy_paste_buffer;

        WINDOW* window;
        std::string name;
        int lock_fd = -1;
    };

    static BufferManager& instance();

    int getBufferCount();
    void create_buffer(const std::string& name);
    std::vector<BufferStructure*> get_all_buffers();
    const std::map<std::string, WINDOW*>& get_bufferWindows() const;
    
    void set_active_buffer(int index);
    BufferStructure& get_active_buffer();
    int get_active_buffer_index();
    WINDOW* get_active_window();
    BufferStructure& get_buffer(int index);
    BufferStructure* get_buffer_by_name(const std::string& name);
    std::string get_current_buffer_name() const;
    
    BufferStructure& next();
    BufferStructure& previous();
    void delete_buffer(int index = -1);
    
    void syncSystemVarsFromBuffer();
    void syncBufferFromSystemVars();
    void update_all_buffers_dimensions();
    void switchToWindowBuffer(WINDOW* window);
    WindowManager& getWindowManager();

private:
    WindowManager& windowManager = WindowManager::getInstance();
    std::array<BufferStructure, MAX_BUFFERS> buffers;
    int active_buffer_index;
    int buffer_count;

    BufferManager() = default;
    BufferManager(const BufferManager&) = delete;
    BufferManager& operator=(const BufferManager&) = delete;
};