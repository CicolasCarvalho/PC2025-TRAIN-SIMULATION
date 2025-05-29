# Compiler
CC := gcc

# Directories
SRC_DIR := src
BUILD_DIR := build

# Source files
# Find all .c files recursively in SRC_DIR
SRC := $(shell find $(SRC_DIR) -name '*.c')

# Object files
# This will correctly map e.g. src/subdir/foo.c to build/subdir/foo.o
OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

# Compiler flags
CFLAGS := -Wall -Wextra # -lubsan -fsanitize=undefined -fsanitize=address -Wno-unused-parameter

# Linker flags
LDFLAGS := -lraylib

# Build target
TARGET := $(BUILD_DIR)/out

# Default target
all: $(TARGET) 

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D) # Create directory for .o file if it doesn't exist
	$(CC) $(CFLAGS) -c $< -o $@ -g 

# Link object files
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Clean build directory
clean:
	rm -rf $(BUILD_DIR) # Remove the entire build directory

.PHONY: all clean
