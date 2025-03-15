CC = gcc
CFLAGS = -std=c17 -Wextra -Wall -pedantic -g
PYTHON = python3

TARGET = ipk-l4-scan

TESTDIR = test
TEST = test.py

# Source files and header files
SRCS = $(wildcard src/*.c)
LIBS = $(wildcard lib/*.h)

# Object files
OBJS = $(SRCS:.c=.o)


all: $(TARGET)

# Build the target from object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm

# Run tests
test: $(TARGET)
	$(PYTHON) $(TESTDIR)/$(TEST)


# Remove object files and target
clean:
	rm -f $(TARGET)
	cd src && rm -f *.o

.PHONY: clean