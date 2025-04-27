# Makefile for Raylib project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE
INCLUDE_PATHS = -I/opt/homebrew/Cellar/raylib/5.5/include -I./src

# Libraries and paths for macOS
LDFLAGS = -L/opt/homebrew/Cellar/raylib/5.5/lib
LDLIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# Directories
SRC_DIR = src
CORE_DIR = $(SRC_DIR)/core
ENTITIES_DIR = $(SRC_DIR)/entities
BIN_DIR = bin

# Source files
SRC_FILES = $(SRC_DIR)/main.c \
            $(CORE_DIR)/game.c \
            $(ENTITIES_DIR)/player.c \
            $(ENTITIES_DIR)/particle.c \
            $(ENTITIES_DIR)/enemy.c

# Output
OUTPUT = $(BIN_DIR)/game

# Object files
OBJ_FILES = $(SRC_FILES:.c=.o)

# Default target
all: $(OUTPUT)

# Linking
$(OUTPUT): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	@echo "Build complete: $(OUTPUT)"

# Compilation
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Clean target
clean:
	rm -f $(OUTPUT) $(OBJ_FILES)
	@echo "Clean complete"

# Run target
run: $(OUTPUT)
	./$(OUTPUT)

.PHONY: all clean run 