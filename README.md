# mvim

mvim is a VIM-inspired modal-based text editor written in C++ using the ncurses library. It combines the efficiency of modal editing with modern features like multiple buffers, syntax highlighting, and customizable keybindings.

If you find a bug or want to discuss the best way to add a new feature, please [open an issue](https://github.com/ErikDervishi03/mvim-textEditor/issues/new/choose).

## Demo
[![asciicast](https://asciinema.org/a/2cxeDudipJMdx0PSHeL8FVybe.svg)](https://asciinema.org/a/2cxeDudipJMdx0PSHeL8FVybe)

## Features
- **Modal Editing**: Normal, Insert, Visual, and Find modes.
- **Multiple Buffers**: Open and edit multiple files simultaneously.
- **Syntax Highlighting**: Customizable syntax coloring via `.mvimlang` definition files.
- **Custom Configuration**: Fully configurable keybindings via `.mvimrc`.
- **System Clipboard**: Seamless copy/paste integration with the system clipboard (requires `xclip`).
- **Mouse Support**: Basic mouse interaction for cursor placement and scrolling.

## Customizable ColorSchemes
You can easily customize the color schemes in mvim to suit your preferences:

![Color Schemes](screenshot/present.jpg)

## Quick Start
mvim has the following dependencies:
- [GCC](https://gcc.gnu.org/)
- [CMake](https://cmake.org/)
- [ncurses](https://invisible-island.net/ncurses/)
- [xclip](https://github.com/astrand/xclip) (required for system clipboard integration)

You can install them with:
```sh
  chmod +x install_dependencies.sh && ./install_dependencies.sh

```

## Installation

### Option 1: Build from source

```sh
  git clone [https://github.com/ErikDervishi03/mvim-textEditor.git](https://github.com/ErikDervishi03/mvim-textEditor.git)
  cd mvim-textEditor
  mkdir build
  cd build
  cmake ..
  sudo make install

```

### Option 2: Use sget

```sh
  sudo sget install [https://github.com/ErikDervishi03/mvim-textEditor.git](https://github.com/ErikDervishi03/mvim-textEditor.git)

```

To uninstall:

```sh
  sudo xargs rm < install_manifest.txt

```

## Configuration

mvim supports a configuration file named `.mvimrc`. The editor looks for this file in the following locations (in order):

1. The current working directory.
2. `/usr/local/share/mvim/.mvimrc`
3. `/usr/share/mvim/.mvimrc`

The configuration file uses a section-based format to bind keys to actions for different modes.

**Example `.mvimrc`:**

```ini
[NORMAL]
Ctrl-s = save
Ctrl-q = quit
Ctrl-z = undo
Ctrl-n = buffer_new
Ctrl-l = buffer_next
Ctrl-h = buffer_prev

[INSERT]
Ctrl-v = paste
Ctrl-s = save

```

## Keybinds (Default Configuration)

mvim uses a hybrid keybinding approach, supporting both traditional Vim motions and common editor shortcuts (e.g., Ctrl+S to save).

### Global / File Operations

| Keybind | Action |
| --- | --- |
| Ctrl-q | Quit |
| Ctrl-s | Save current buffer |
| Ctrl-n | Open new empty buffer |
| Ctrl-l | Switch to next buffer |
| Ctrl-h | Switch to previous buffer |

### Normal Mode

Used for navigation and manipulation.

| Keybind | Action |
| --- | --- |
| i | Enter Insert mode |
| v | Enter Visual mode |
| f | Enter Find mode |
| u / Ctrl-z | Undo |
| Ctrl-v | Paste from system clipboard |
| Ctrl-a | Select all |
| h, j, k, l | Move cursor Left, Down, Up, Right |
| x | Delete character |
| d | Delete line |
| o | Insert line below |
| O | Insert line above |
| Ctrl-Right | Go to next word |
| Ctrl-Left | Go to previous word |

### Insert Mode

Used for typing text.

| Keybind | Action |
| --- | --- |
| Esc | Switch to Normal mode |
| Ctrl-v | Paste |
| Ctrl-z | Undo |
| Ctrl-Right | Move to next word |
| Ctrl-Left | Move to previous word |

### Visual Mode

Used for text selection.

| Keybind | Action |
| --- | --- |
| Esc | Switch to Normal mode |
| > / < | Indent / Unindent selection |
| Ctrl-c | Copy selection to system clipboard |
| Ctrl-x | Cut/Delete selection |

### Find Mode

Used for searching text within the buffer.

| Keybind | Action |
| --- | --- |
| Enter | Confirm search |
| Ctrl-n | Find next occurrence |
| Ctrl-p | Find previous occurrence |
| r | Replace |
| Esc | Return to Normal mode |

## Syntax Highlighting

Syntax highlighting is defined in `.mvimlang` files located in the `languages/` directory. You can add support for new languages by creating a new definition file containing keywords, comment styles, and extensions.

## Technical Details

mvim uses [doxygenmd](https://github.com/d99kris/doxygenmd) to generate its Markdown API documentation:

```
doxygenmd src doc

```

mvim uses Uncrustify to ensure consistent code formatting:

```
uncrustify -c uncrustify.cfg --no-backup src/*
uncrustify -c uncrustify.cfg --no-backup include/*

```

## Inspiration

* [vim](https://github.com/vim/vim)
* [Cano](https://github.com/CobbCoding1/Cano) for this README

