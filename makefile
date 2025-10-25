# Minesweeper Makefile
CC = gcc
CFLAGS = -lncursesw -DNCURSES_WIDECHAR=1
TARGET = ms
INSTALL_DIR = /usr/bin
SRC = minesweeper.c

# Colors
GREEN = \033[0;32m
MAGENTA = \033[0;35m
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
	@echo "$(MAGENTA)Compiling minesweeper...$(RESET)"
	@$(CC) $(SRC) $(CFLAGS) -o $(TARGET)
	@echo "$(CYAN)$(BOLD)Installing to $(INSTALL_DIR)...$(RESET)"
	# Move the binary to a temp file
	@$(SUDO) mv $(TARGET) $(INSTALL_DIR)/$(TARGET).bin
	# Create a wrapper script that sets LANG=C.utf8
	@echo '#!/bin/sh' | $(SUDO) tee $(INSTALL_DIR)/$(TARGET) > /dev/null
	@echo 'LANG=C.utf8 exec $(INSTALL_DIR)/$(TARGET).bin "$$@"' | $(SUDO) tee -a $(INSTALL_DIR)/$(TARGET) > /dev/null
	@$(SUDO) chmod +x $(INSTALL_DIR)/$(TARGET)
	@echo ""
	@echo "$(GREEN)$(BOLD)✓ Successfully installed!$(RESET)"
	@echo "$(GREEN)  → Run '$(BOLD)ms$(RESET)$(GREEN)' to play$(RESET)"
	@echo ""

clean:
	@$(SUDO) rm -f $(INSTALL_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET).bin
	@echo ""
	@echo "$(GREEN)✓ Removed $(INSTALL_DIR)/$(TARGET) and backup binary$(RESET)"
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

