#!/bin/bash

# Function to install dependencies on Debian-based systems
install_debian() {
    echo "Detected Debian-based system. Installing dependencies..."
    sudo apt update
    sudo apt install -y build-essential cmake libncurses5-dev libncursesw5-dev
}

# Function to install dependencies on Red Hat-based systems
install_redhat() {
    echo "Detected Red Hat-based system. Installing dependencies..."
    sudo dnf install -y gcc-c++ cmake ncurses-devel
}

# Function to install dependencies on macOS
install_macos() {
    echo "Detected macOS system. Installing dependencies..."
    if ! command -v brew &> /dev/null; then
        echo "Homebrew is not installed. Please install Homebrew first."
        exit 1
    fi
    brew install cmake ncurses
}

# Function to install dependencies on Windows using Chocolatey
install_windows_choco() {
    echo "Detected Windows system. Installing dependencies using Chocolatey..."
    choco install -y mingw cmake ncurses
}

# Function to install dependencies on Windows using winget
install_windows_winget() {
    echo "Detected Windows system. Installing dependencies using winget..."
    winget install -e --id Gnu.GCC
    winget install -e --id CMake.CMake
    winget install -e --id Gnu.NCurses
}

# Check the operating system
if [[ -f /etc/os-release ]]; then
    # Load the OS info
    . /etc/os-release

    if [[ "$ID" == "ubuntu" || "$ID" == "debian" ]]; then
        install_debian
    elif [[ "$ID" == "fedora" || "$ID" == "centos" || "$ID" == "rhel" ]]; then
        install_redhat
    else
        echo "Unsupported OS: $ID"
        exit 1
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    install_macos
elif [[ "$OS" == "Windows_NT" ]]; then
    # Check if Chocolatey is installed
    if command -v choco &> /dev/null; then
        install_windows_choco
    # Check if winget is installed
    elif command -v winget &> /dev/null; then
        install_windows_winget
    else
        echo "Neither Chocolatey nor winget is installed. Please install one of them first."
        exit 1
    fi
else
    echo "Unable to detect the operating system."
    exit 1
fi

echo "Dependencies installed successfully."
