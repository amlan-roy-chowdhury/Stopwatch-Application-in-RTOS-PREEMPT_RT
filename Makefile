# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread -lrt

# Target executable
TARGET = stopwatch

# Source files
SRCS = stopwatch.c gpio.c

# Object files
OBJS = $(SRCS:.c=.o)

# Build the target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)
