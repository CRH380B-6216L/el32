CC = gcc
CFLAGS = -Wall
LDFLAGS = -lusb-1.0
SOURCES = xpadD.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = xpadD

.PHONY: all clean

all: $(TARGET)
 
$(TARGET) : $(OBJECTS)
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

download: $(SOURCES)
	arm-linux-gcc $(CFLAGS) -o xpadD2 $^ $(LDFLAGS)
	sshpass -p 'root' scp xpadD2 root@10.0.0.42:/bin

clean:
	@rm -rf $(TARGETS) $(OBJECTS)
