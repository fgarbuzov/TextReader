# Compiler and flags
CC = gcc
CFLAGS = -Wall -O2

# Resource compiler for Windows
RC = windres

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Resource file
RC_SRC = $(SRC_DIR)/lister.rc
RC_OBJ = $(OBJ_DIR)/lister_res.o  

# Output executable
EXEC = $(BIN_DIR)/TextReader

# Default target
all: $(EXEC)

# Link the object files and resource file to create the executable
$(EXEC): $(OBJ) $(RC_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) $(RC_OBJ) -o $@ -lgdi32 -luser32 -lkernel32 -lcomctl32 -mwindows

# Compile the .c files into .o object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile the .rc file into a .o object file (for resource linking)
$(RC_OBJ): $(RC_SRC)
	$(RC) $(RC_SRC) -o $(RC_OBJ)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
