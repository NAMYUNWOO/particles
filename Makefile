# Makefile for Raylib project (macOS & Windows cross-platform)

# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -std=c99 -D_DEFAULT_SOURCE

# Directories
SRC_DIR      := src
CORE_DIR     := $(SRC_DIR)/core
ENTITIES_DIR := $(SRC_DIR)/entities
MANAGERS_DIR := $(ENTITIES_DIR)/managers
BIN_DIR      := bin

# Source and object files
SRC_FILES := \
	$(SRC_DIR)/main.c \
	$(CORE_DIR)/game.c \
	$(CORE_DIR)/physics.c \
	$(CORE_DIR)/input_handler.c \
	$(CORE_DIR)/memory_pool.c \
	$(CORE_DIR)/event/event_system.c \
	$(ENTITIES_DIR)/player.c \
	$(ENTITIES_DIR)/particle.c \
	$(ENTITIES_DIR)/enemy.c \
	$(ENTITIES_DIR)/explosion.c \
	$(MANAGERS_DIR)/enemy_manager.c \
	$(MANAGERS_DIR)/particle_manager.c
OBJ_FILES := $(SRC_FILES:.c=.o)

# Platform detection: Windows_NT for Windows, otherwise assume macOS
ifeq ($(OS),Windows_NT)

# -- Windows (MinGW/MSYS2) --
RAYLIB_PATH    := C:/Users/namyunwoo/W64Devkit/w64devkit
INCLUDE_PATHS  := -I"$(RAYLIB_PATH)/include" -I./src
LDFLAGS        := -L"$(RAYLIB_PATH)/lib"
LDLIBS         := -lraylib -lopengl32 -lgdi32 -lwinmm

else

# -- macOS --
RAYLIB_PATH    := /opt/homebrew/Cellar/raylib/5.5
INCLUDE_PATHS  := -I"$(RAYLIB_PATH)/include" -I./src
LDFLAGS        := -L"$(RAYLIB_PATH)/lib"
LDLIBS         := -lraylib \
                   -framework OpenGL \
                   -framework Cocoa \
                   -framework IOKit \
                   -framework CoreVideo

endif

# Default target
all: $(BIN_DIR)/game

# Link step
$(BIN_DIR)/game: $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	@echo "Build complete: $@"

# Compile step
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Clean target
clean:
	rm -f $(BIN_DIR)/game $(OBJ_FILES)
	@echo "Clean complete"

# Run target
run: all
	@./$(BIN_DIR)/game

.PHONY: all clean run
