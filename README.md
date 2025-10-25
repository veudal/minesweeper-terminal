
# minesweeper-terminal

A fully-featured **Minesweeper game for the terminal**. Features include:

- Colorized board
- Timer
- Auto-flag and auto-chord
- Custom board sizes
- Keyboard navigation for easy play

---

## Screenshot

<img width="1919" height="1001" alt="Minesweeper Terminal Screenshot" src="https://github.com/user-attachments/assets/51404280-aa36-4dcf-8fb0-dd2c9dc87508" />

---

## Installation (Recommended)

This method builds the game and installs it system-wide so you can run it with the `ms` command:

```bash
sudo apt update && sudo apt upgrade -y && \
sudo apt install gcc make git libncurses-dev -y && \
git clone https://github.com/veudal/minesweeper-terminal.git && \
cd minesweeper-terminal && \
make install

```

---

## Build from Source

If you prefer to compile manually:


```bash
sudo apt update && sudo apt upgrade -y && \
sudo apt install gcc make git libncurses-dev -y && \
git clone https://github.com/veudal/minesweeper-terminal.git && \
cd minesweeper-terminal && \
gcc minesweeper.c -lncursesw -DNCURSES_WIDECHAR=1 -o ms && \
./ms
```

You can also specify custom board size and mine percentage when running:

```bash
./ms [rows] [cols] [mine_percentage]
```

- `rows` and `cols`: dimensions of the board (default 24×24)  
- `mine_percentage`: percentage of cells that are mines (default 15%)

Examples:

```bash
./ms          # 24x24 board, 15% mines
./ms 20       # 20x20 board, 15% mines
./ms 20 30    # 20 rows × 30 columns, 15% mines
./ms 20 30 20 # 20 rows × 30 columns, 20% mines
```

---

## Usage

**Navigation and controls:**

| Action                       | Key(s)                   |
|-------------------------------|-------------------------|
| Move cursor                   | Arrow keys, WASD         |
| Reveal a cell                 | Space, K                 |
| Flag/unflag a cell            | F, L                     |
| Restart the game              | R                        |
| Quit the game                 | Q                        |

**Notes:**

- Revealing a number cell with all adjacent flags automatically reveals safe cells (**auto-chord**).  
- Flagging all remaining mines around a number cell will automatically mark the rest (**auto-flag**).  
- Timer starts on the first move.  
- Game ends when all non-mine cells are revealed or when a mine is triggered.

---
