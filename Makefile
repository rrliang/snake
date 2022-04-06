CC=gcc
TARGET=snakefork

# all:
# 	gcc mysnake.c -o mysnake -lncurses

all: main.o snake.o
	gcc -o $(TARGET) main.o snake.o -lncurses

snake.o: snake.c snake.h
	gcc -c snake.c

main.o: main.c snake.h
	gcc -c main.c 

clean:
	rm $(TARGET)1
