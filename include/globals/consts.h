#pragma once

constexpr int KEY_ENTER_ = 10;

constexpr int KEY_TAB = 9;

constexpr int ESC = 27;

constexpr int SCROLL_START_THRESHOLD = 5;

constexpr int span = 4;

constexpr int tab_size = 2;

constexpr int MAX_FILENAME_LENGTH = 30;

constexpr int KEY_BACKSPACE_LEGACY = 8;

// Define common xterm key codes for Ctrl+Arrows
// These are not standard in ncurses, so we define them manually.
constexpr int CTRL_KEY_UP  =  571;
constexpr int CTRL_KEY_DOWN = 530;
constexpr int CTRL_KEY_LEFT = 550;
constexpr int CTRL_KEY_RIGHT = 565;