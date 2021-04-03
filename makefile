all: ex2final.c
	gcc ex2final.c -o ex2final
all-GDB: ex2final.c
	gcc -g ex2final.c -o ex2final
