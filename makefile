# Makefile for Minesweeper

CC = gcc
CFLAGS = -DNCURSES_WIDECHAR=1
LIBS = -lncursesw
TARGET = ms
SRC = minesweeper.c
PREFIX = $(HOME)/.local/bin

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)

install: all
	@mkdir -p $(PREFIX)
	@cp $(TARGET) $(PREFIX)
	@echo "Installed $(TARGET) to $(PREFIX)"

	# Add PREFIX to PATH for bash, zsh, fish, and POSIX shells
	@echo "Adding $(PREFIX) to your shell PATH if not already present..."
	@if [ -n "$$BASH_VERSION" ]; then \
	    SHELLRC=~/.bashrc; \
	elif [ -n "$$ZSH_VERSION" ]; then \
	    SHELLRC=~/.zshrc; \
	elif [ -n "$$FISH_VERSION" ]; then \
	    SHELLRC=~/.config/fish/config.fish; \
	else \
	    SHELLRC=~/.profile; \
	fi; \
	if ! grep -qxF 'export PATH="$(PREFIX):$$PATH"' $$SHELLRC 2>/dev/null; then \
	    if [ "$$SHELLRC" = "~/.config/fish/config.fish" ]; then \
	        echo 'set -gx PATH $(PREFIX) $$PATH' >> $$SHELLRC; \
	    else \
	        echo 'export PATH="$(PREFIX):$$PATH"' >> $$SHELLRC; \
	    fi; \
	    echo "PATH updated in $$SHELLRC"; \
	fi

	# Update PATH in current session so 'ms' can be used immediately
	@case "$$SHELL" in \
	    */bash|*/zsh) export PATH="$(PREFIX):$$PATH";; \
	    */fish) set -gx PATH $(PREFIX) $$PATH;; \
	    *) export PATH="$(PREFIX):$$PATH";; \
	esac
	@echo "Done! '$(TARGET)' is now available immediately and after future logins."

clean:
	rm -f $(TARGET)
