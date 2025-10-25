# minesweeper-terminal
Fully functional minesweeper TUI with colors, timer, auto-flag, auto-chord and custom board sizes.

# Screenshot
<img width="1919" height="1001" alt="image" src="https://github.com/user-attachments/assets/51404280-aa36-4dcf-8fb0-dd2c9dc87508" />

## Install prerequisites for Debian/Ubuntu-based systems
```
sudo apt install libxml2
sudo apt install gcc
```

# How to install (recommended for easy access via 'ms' command)
```
git clone https://github.com/veudal/minesweeper-terminal.git
cd minesweeper-terminal
sudo make install
```

# How to build the file yourself
```
git clone https://github.com/veudal/minesweeper-terminal.git
cd minesweeper-terminal
gcc minesweeper.c -lncursesw -DNCURSES_WIDECHAR=1 -o ms
./ms
```

