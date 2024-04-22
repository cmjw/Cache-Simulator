CC = gcc
CFLAGS = -Wno-error -Wall -Wextra -std=c99
SRC = cache_simulator.c
TARGET = cache_simulator

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)