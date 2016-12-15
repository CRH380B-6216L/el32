CC = gcc
CFLAGS = -Wall
LDFLAGS = -lusb-1.0 -lncurses
SOURCES = ncurses_demo.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = ncurses

.PHONY: all clean

all: $(TARGET)
 
$(TARGET) : $(OBJECTS)
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

download: $(SOURCES)
	arm-linux-gcc $(CFLAGS) -o $(TARGET)2 $^ $(LDFLAGS)
	sshpass -p '233' scp $(TARGET)2 root@10.0.0.42:/bin

clean:
	@rm -rf $(TARGETS) $(OBJECTS)
