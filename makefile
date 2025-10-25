# Minesweeper Makefile
CC = gcc
CFLAGS = -lncursesw -DNCURSES_WIDECHAR=1
TARGET = ms
INSTALL_DIR = /usr/bin
SRC = minesweeper.c

# Colors
GREEN = \033[0;32m
BLUE = \033[0;34m
CYAN = \033[0;36m
BOLD = \033[1m
RESET = \033[0m

# Check if running as root
ifeq ($(shell [ $$(id -u) -eq 0 ] && echo root),root)
SUDO =
else
SUDO = sudo
endif

.PHONY: all install clean help

all: install

install: $(SRC)
	@echo ""
	@echo "$(CYAN)$(BOLD)Compiling minesweeper...$(RESET)"
	@$(CC) $(SRC) $(CFLAGS) -o $(TARGET)
	@echo "$(BLUE)Installing to $(INSTALL_DIR)...$(RESET)"
	@$(SUDO) mv $(TARGET) $(INSTALL_DIR)/$(TARGET)
	@echo ""
	@echo "$(GREEN)$(BOLD)✓ Successfully installed!$(RESET)"
	@echo "$(GREEN)  → Run '$(BOLD)ms$(RESET)$(GREEN)' to play$(RESET)"
	@echo ""

clean:
	@$(SUDO) rm -f $(INSTALL_DIR)/$(TARGET)
	@echo ""
	@echo "$(GREEN)✓ Removed $(INSTALL_DIR)/$(TARGET)$(RESET)"
	@echo ""

help:
	@echo ""
	@echo "$(BOLD)Minesweeper Makefile$(RESET)"
	@echo ""
	@echo "$(CYAN)Usage:$(RESET)"
	@echo "  $(BOLD)make$(RESET)          - Compile and install to /usr/bin (requires sudo)"
	@echo "  $(BOLD)make install$(RESET)  - Same as above"
	@echo "  $(BOLD)make clean$(RESET)    - Remove installed binary"
	@echo "  $(BOLD)make help$(RESET)     - Show this help message"
	@echo ""
