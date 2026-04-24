# Bumped to 3.16 to support Precompiled Headers (PCH)
cmake_minimum_required(VERSION 3.16)

project(
  mvim_text_editor
  VERSION 1.0.0
  DESCRIPTION "IDE vim-like"
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- 1. UNITY BUILD (The Ultimate Speed Hack) ---
# This merges all your .cpp files into one giant file before compiling.
# The compiler only parses your headers ONCE for the entire project!
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 4) 

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find ncurses library
find_package(Curses REQUIRED)

# Include directories
include_directories(${CURSES_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Collect source files (Note: If you add new files, remember to re-run cmake ..)
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Add the executable
add_executable(mvim ${SRC_FILES})

# --- 2. PRECOMPILED HEADERS (PCH) ---
# Pre-bakes these massive standard libraries into a binary format
# so they are instantly loaded instead of parsed line-by-line.
target_precompile_headers(mvim PRIVATE 
    <string> 
    <vector> 
    <deque>
    <stack> 
    <map>
    <variant>
    <memory>
    <filesystem>
    <fstream>
)

# Link against the ncurses library
target_link_libraries(mvim ${CURSES_LIBRARIES})

# Install target
install(TARGETS mvim DESTINATION bin)