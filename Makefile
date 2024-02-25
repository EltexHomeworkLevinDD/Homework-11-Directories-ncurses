C:=gcc

new: new.c subnew.c
	gcc new.c subnew.c -o new -lncurses
