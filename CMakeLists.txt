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

# --- MODIFICHE BASATE SUL MANUALE NCURSES(3X) ---

# 1. Abilita le feature Wide Character negli header
# "X/Open Curses, Issue 7 (2009) recommends defining it to 700"
add_definitions(-D_XOPEN_SOURCE=700)
# "Alternatively, you can enable the feature by defining NCURSES_WIDECHAR"
add_definitions(-DNCURSES_WIDECHAR=1)

# 2. Trova e linka ncursesw (Wide)
# Il manuale specifica che i nomi hanno una "w" appesa (es. -lncursesw)
find_library(NCURSESW_LIB NAMES ncursesw)
find_path(NCURSESW_INCLUDE_DIR NAMES ncurses.h PATH_SUFFIXES ncursesw)

if(NCURSESW_LIB AND NCURSESW_INCLUDE_DIR)
  message(STATUS "Found ncursesw: ${NCURSESW_LIB}")
  message(STATUS "Header Path: ${NCURSESW_INCLUDE_DIR}")
  set(CURSES_LIBRARIES ${NCURSESW_LIB})
  set(CURSES_INCLUDE_DIR ${NCURSESW_INCLUDE_DIR})
else()
  message(FATAL_ERROR "ncursesw (Wide) non trovata. Installa libncursesw5-dev o ncurses-devel.")
endif()
# ------------------------------------------------

include_directories(${CURSES_INCLUDE_DIR})

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(mvim ${SRC_FILES})

target_precompile_headers(mvim PRIVATE <string> <vector> <stack> <map> <iostream>)

# Forza l'inclusione della directory corretta PRIMA di quelle di sistema
target_include_directories(mvim BEFORE PUBLIC ${CURSES_INCLUDE_DIR})
target_include_directories(mvim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mvim ${CURSES_LIBRARIES})

install(TARGETS mvim DESTINATION /usr/local/bin)
install(DIRECTORY languages DESTINATION /usr/local/share/mvim)
install(FILES .mvimrc DESTINATION /usr/local/share/mvim)