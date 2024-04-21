CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99
SRC = cache_simulator.c
TARGET = cache_simulator

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)