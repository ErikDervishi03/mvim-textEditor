cmake_minimum_required(VERSION 3.10)
project(
  project_00819
  VERSION 1.0.0
  DESCRIPTION "IDE vim-like"
  LANGUAGES C CXX
)

# Try to find ncurses
find_package(Curses)

# If ncurses is not found, install it
if(NOT CURSES_FOUND)
  message(STATUS "Ncurses not found, attempting to install...")

  # Run a shell script to install ncurses
  execute_process(
    COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/install_ncurses.sh"
    RESULT_VARIABLE result
  )

  if(result)
    message(FATAL_ERROR "Failed to install ncurses. Please install it manually.")
  endif()

  # Try to find ncurses again after installation
  find_package(Curses REQUIRED)
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

# Copy to /usr/local/bin after build
add_custom_command(TARGET mvim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mvim> /usr/local/bin/mvim
    COMMENT "Copying mvim to /usr/local/bin"
    VERBATIM
)
