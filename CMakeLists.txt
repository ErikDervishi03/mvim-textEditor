cmake_minimum_required(VERSION 3.10)
project(
  project_00819
  VERSION 1.0.0
  DESCRIPTION "IDE vim-like"
  LANGUAGES C CXX)

# Find and include the ncurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Collect all source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Add the executable with all source files
add_executable(mvim ${SRC_FILES})

# Link against the ncurses library
target_include_directories(mvim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mvim ${CURSES_LIBRARIES})
