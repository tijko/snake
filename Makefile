CC = gcc
FLAGS = -g -Wall -Wextra -pedantic -lcursesw

TARGET = snake

default:
	$(CC) $(TARGET).c $(TARGET).h -o $(TARGET) $(FLAGS)

install:
	cp $(TARGET) /usr/bin

uninstall:
	rm /usr/bin/$(TARGET)

clean:
	rm $(TARGET)
