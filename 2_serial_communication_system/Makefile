CC     = gcc
CFLAGS = -Wall -Wextra -g
TARGET = comm_system
SRCS   = src/comm.c src/main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
