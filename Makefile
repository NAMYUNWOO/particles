# Makefile for Raylib project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE
INCLUDE_PATHS = -I/opt/homebrew/Cellar/raylib/5.5/include

# Libraries and paths for macOS
LDFLAGS = -L/opt/homebrew/Cellar/raylib/5.5/lib
LDLIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# Source files and output
SRC = main.c game.c player.c particle.c
OUTPUT = game

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(OUTPUT)

# Linking
$(OUTPUT): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# Compilation
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Clean target
clean:
	rm -f $(OUTPUT) $(OBJ)

# Run target
run: $(OUTPUT)
	./$(OUTPUT)

.PHONY: all clean run 