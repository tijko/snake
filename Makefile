CC = gcc
FLAGS = -g -Wall -Wextra -pedantic -lncursesw

TARGET = snake

default:
	$(CC) $(TARGET).c $(TARGET).h -o $(TARGET) $(FLAGS)

install:
	cp $(TARGET) /usr/bin

uninstall:
	rm /usr/bin/$(TARGET)

clean:
	rm $(TARGET)
