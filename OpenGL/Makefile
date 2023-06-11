CC = gcc
CFLAGS = -O3 -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -Isrc/ -Iinclude/

SRCS = main.c src/glad.c
OBJS = $(SRCS:.c=.o)
TARGET = openglvers

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
