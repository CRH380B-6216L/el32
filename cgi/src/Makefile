CC = gcc
CFLAGS = -Wall
LDFLAGS = -lusb-1.0
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
TARGET = xpadD

.PHONY: all clean

all: $(TARGET)
 
$(TARGET) : $(OBJECTS)
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

dwnld: $(SOURCES)
	arm-linux-gcc $(CFLAGS) -o xpadD2 xpadD.c $(LDFLAGS)
	arm-linux-gcc $(CFLAGS) -o xpadCGI xpadCGI.c $(LDFLAGS)
	sshpass -p '233' scp xpadD2 root@10.0.0.42:/bin
	sshpass -p '233' scp index.html root@10.0.0.42:/var/www
	sshpass -p '233' scp xpadCGI root@10.0.0.42:/var/www/cgi-bin

clean:
	@rm -rf $(TARGETS) $(OBJECTS)
