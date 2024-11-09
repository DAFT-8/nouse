# Compiler and flags
CC = cc
CFLAGS = -Wall
LDFLAGS = -lX11 -lXtst

# Target executable
TARGET = nouse

# Source file
SRC = nouse.c

# Object file
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

# Rule to link the object file and create the executable
$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Rule to compile the source file into an object file
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

# Install the binary to /usr/local/bin and set permissions
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	chmod u+x /usr/local/bin/$(TARGET)

# Clean target to remove compiled files
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean install
