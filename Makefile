CC = gcc
FLAGS = -g -Wall -lcurses

TARGET = snake

default:
	$(CC) $(TARGET).c $(TARGET).h -o $(TARGET) $(FLAGS)

install:
	cp $(TARGET) /usr/bin

uninstall:
	rm /usr/bin/$(TARGET)

clean:
	rm $(TARGET)
