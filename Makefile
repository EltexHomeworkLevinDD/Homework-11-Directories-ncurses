C:=gcc

new: main.c submain.o
	gcc main.c submain.o -o main -lncurses
