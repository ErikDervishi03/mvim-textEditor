cmake_minimum_required(VERSION 3.10)
project(
  project_00819
  VERSION 1.0.0
  DESCRIPTION "IDE vim-like"
  LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

# Try to find ncurses
find_package(Curses)

# If ncurses is not found, give instructions to install it
if(NOT CURSES_FOUND)
  message(STATUS "Ncurses not found.")
  
  # Detect OS and provide installation instructions
  if(UNIX AND NOT APPLE)
    message(STATUS "To install ncurses on a Linux system, run:")
    message(STATUS "  sudo apt-get install libncurses5-dev libncursesw5-dev")
  elseif(APPLE)
    message(STATUS "To install ncurses on macOS, run:")
    message(STATUS "  brew install ncurses")
  else()
    message(FATAL_ERROR "Ncurses not found. Please install it manually.")
  endif()

  message(FATAL_ERROR "Ncurses is required to build this project. Please install it and retry.")
else()
  message(STATUS "Ncurses found!")
endif()

# Include ncurses headers
include_directories(${CURSES_INCLUDE_DIR})

# Collect all source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Add the executable with all source files
add_executable(mvim ${SRC_FILES})

# Link against the ncurses library
target_include_directories(mvim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mvim ${CURSES_LIBRARIES})

# Install the executable globally
install(TARGETS mvim DESTINATION /usr/local/bin)

# Install the languages directory to /usr/local/share/mvim/languages
install(DIRECTORY languages DESTINATION /usr/local/share/mvim)

# Install the configuration file
install(FILES .mvimrc DESTINATION /usr/local/share/mvim)