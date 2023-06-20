# If it throws an error try to compile manually

CC = gcc
CFLAGS = -lSDL2 -O3

SRCS = mandelexplorer.c
OBJS = $(SRCS:.c=.o)
TARGET = mandelexplorer

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
