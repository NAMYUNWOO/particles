# Makefile for Raylib project (macOS & Windows cross-platform)

# Disable implicit rules to prevent tangle errors
.SUFFIXES:
MAKEFLAGS += --no-builtin-rules

# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -std=c99 -D_DEFAULT_SOURCE

# Directories
SRC_DIR      := src
CORE_DIR     := $(SRC_DIR)/core
ENTITIES_DIR := $(SRC_DIR)/entities
MANAGERS_DIR := $(ENTITIES_DIR)/managers
STAGES_DIR   := $(MANAGERS_DIR)/stages
ITEMS_DIR    := $(ENTITIES_DIR)/items
BIN_DIR      := bin

# Source and object files
SRC_FILES := \
	$(SRC_DIR)/main.c \
	$(CORE_DIR)/game.c \
	$(CORE_DIR)/physics.c \
	$(CORE_DIR)/input_handler.c \
	$(CORE_DIR)/memory_pool.c \
	$(CORE_DIR)/dev_test_mode.c \
	$(CORE_DIR)/event/event_system.c \
	$(ENTITIES_DIR)/player.c \
	$(ENTITIES_DIR)/particle.c \
	$(ENTITIES_DIR)/enemy.c \
	$(ENTITIES_DIR)/enemy_state.c \
	$(ENTITIES_DIR)/explosion.c \
	$(ITEMS_DIR)/hp_potion.c \
	$(MANAGERS_DIR)/enemy_manager.c \
	$(MANAGERS_DIR)/particle_manager.c \
	$(MANAGERS_DIR)/stage_manager.c \
	$(MANAGERS_DIR)/item_manager.c \
	$(STAGES_DIR)/stage_1.c \
	$(STAGES_DIR)/stage_2.c \
	$(STAGES_DIR)/stage_3.c \
	$(STAGES_DIR)/stage_4.c \
	$(STAGES_DIR)/stage_5.c \
	$(STAGES_DIR)/stage_6.c \
	$(STAGES_DIR)/stage_7.c \
	$(STAGES_DIR)/stage_8.c \
	$(STAGES_DIR)/stage_9.c \
	$(STAGES_DIR)/stage_10.c \
	$(STAGES_DIR)/stage_test.c
OBJ_FILES := $(SRC_FILES:.c=.o)

# Platform detection: Windows_NT for Windows, otherwise assume macOS
ifeq ($(OS),Windows_NT)

# -- Windows (MinGW/MSYS2) --
RAYLIB_PATH    := C:/Users/namyunwoo/W64Devkit/w64devkit
INCLUDE_PATHS  := -I"$(RAYLIB_PATH)/include" -I./src
LDFLAGS        := -L"$(RAYLIB_PATH)/lib"
LDLIBS         := -lraylib -lopengl32 -lgdi32 -lwinmm -lm

else

# -- macOS --
RAYLIB_PATH    := /opt/homebrew/Cellar/raylib/5.5
INCLUDE_PATHS  := -I"$(RAYLIB_PATH)/include" -I./src
LDFLAGS        := -L"$(RAYLIB_PATH)/lib"
LDLIBS         := -lraylib \
                   -framework OpenGL \
                   -framework Cocoa \
                   -framework IOKit \
                   -framework CoreVideo \
                   -lm

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

# Stage-specific test targets
# These targets compile and run the game, jumping directly to a specific stage
test-stage-1: all
	@echo "Testing Stage 1: Gravitational Anomaly"
	@./$(BIN_DIR)/game --start-stage 1

test-stage-2: all
	@echo "Testing Stage 2: The Hunt Begins"
	@./$(BIN_DIR)/game --start-stage 2

test-stage-3: all
	@echo "Testing Stage 3: Speed Demons"
	@./$(BIN_DIR)/game --start-stage 3

test-stage-4: all
	@echo "Testing Stage 4: Divide and Conquer"
	@./$(BIN_DIR)/game --start-stage 4

test-stage-5: all
	@echo "Testing Stage 5: Orbital Mechanics"
	@./$(BIN_DIR)/game --start-stage 5

test-stage-6: all
	@echo "Testing Stage 6: Guardian of the Void (BOSS)"
	@./$(BIN_DIR)/game --start-stage 6

test-stage-7: all
	@echo "Testing Stage 7: Quantum Flux"
	@./$(BIN_DIR)/game --start-stage 7

test-stage-8: all
	@echo "Testing Stage 8: Magnetic Storm"
	@./$(BIN_DIR)/game --start-stage 8

test-stage-9: all
	@echo "Testing Stage 9: Chain Reaction"
	@./$(BIN_DIR)/game --start-stage 9

test-stage-10: all
	@echo "Testing Stage 10: The Particle Overlord (FINAL BOSS)"
	@./$(BIN_DIR)/game --start-stage 10

# Developer test mode - interactive enemy spawning and testing
test-enemy: all
	@echo "Launching developer test mode..."
	@./$(BIN_DIR)/game --test-mode

# Compile individual stage files for validation
compile-stage-%: $(STAGES_DIR)/stage_%.c
	@echo "Compiling stage $*..."
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $(STAGES_DIR)/stage_$*.o
	@echo "Stage $* compiled successfully"

.PHONY: all clean run test-stage-1 test-stage-2 test-stage-3 test-stage-4 test-stage-5 \
        test-stage-6 test-stage-7 test-stage-8 test-stage-9 test-stage-10
