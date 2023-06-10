CC = gcc
CFLAGS = -lSDL2 -O3

SRCS = mandelexplorer.c
OBJS = $(SRCS:.c=.o)
TARGET = mandelexplorer

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
