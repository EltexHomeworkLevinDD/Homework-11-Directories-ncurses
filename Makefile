CC:=gcc

main : main.c submain.o
	gcc main.c submain.o -o main -lncurses

.PHONY : clean
clean :
	-rm *.o $(objects)  
