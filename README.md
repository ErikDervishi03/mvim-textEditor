# mvim
mvim is a VIM inspired modal-based text editor written in Cpp using the [ncurses](https://opensource.apple.com/source/old_ncurses/old_ncurses-1/ncurses/test/ncurses.c.auto.html) library. 

# Demo

## Quick Start
Cano has the following dependencies:
- [GCC](https://gcc.gnu.org/)
- [Make](https://www.gnu.org/software/make/)
- [ncurses](https://opensource.apple.com/source/old_ncurses/old_ncurses-1/ncurses/test/ncurses.c.auto.html)

## install mvim
```sh
  git clone https://github.com/ErikDervishi03/mvim-textEditor.git
  cd mvim-textEditor
  mkdir build
  cd build
  cmake ..
  sudo make install
```
## how to run
1. create a new file
```sh
  mvim
```

2. open an existing file
```sh
  mvim filename
```

## Modes
Normal - For motions and deletion \
Insert - For inserting text \

## Keybinds
|Mode  | Keybind        | Action                                          |
|------|----------------|-------------------------------------------------|
|Insert| Esc            | change to Normal mode                           |
|Normal| q              | quit                                            |
|Normal| s              | save                                            |
|Normal| h              | Move cursor left                                |
|Normal| j              | Move cursor down                                |
|Normal| k              | Move cursor up                                  |
|Normal| l              | Move cursor right                               |
|Normal| x              | Delete character                                |
|Normal| g              | Go to first line                                |
|Normal| G              | Go to last line                                 |
|Normal| o              | Create line below current                       |
|Normal| O              | Create line above current                       |
|Normal| i              | Enter insert mode                               |
|Normal| m              | open a menu that explain keys                   |
|Normal| a              | go to the end of the line                       |
|Normal| d              | remove the line                                 |

## inspiration
- [vim](https://github.com/vim/vim)
- [Cano](https://github.com/CobbCoding1/Cano?tab=readme-ov-file) for this README
