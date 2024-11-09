CC = cc
CFLAGS = -Wall
LDFLAGS = -lX11 -lXtst

TARGET = nouse

SRC = nouse.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

install: $(TARGET)
	mkdir -p ~/.local/bin
	cp $(TARGET) ~/.local/bin/

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean install
