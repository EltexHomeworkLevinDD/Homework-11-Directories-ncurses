CC:=gcc

main : main.c

.PHONY : clean
clean :
	-rm *.o $(objects)  
