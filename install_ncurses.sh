#!/bin/bash

# Check if ncurses is installed, if not, install it
if ! ncurses6-config --version &> /dev/null
then
    echo "Installing ncurses..."
    
    # For Ubuntu/Debian systems
    if [ -x "$(command -v apt)" ]; then
        sudo apt update
        sudo apt install -y libncurses5-dev libncursesw5-dev
    
    # For RedHat/CentOS systems
    elif [ -x "$(command -v yum)" ]; then
        sudo yum install -y ncurses-devel
    
    # For macOS (Homebrew)
    elif [ -x "$(command -v brew)" ]; then
        brew install ncurses
    
    else
        echo "Package manager not found. Please install ncurses manually."
        exit 1
    fi
else
    echo "Ncurses is already installed."
fi
