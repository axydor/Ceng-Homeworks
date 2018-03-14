all: server.c hunter.c prey.c
	gcc -o serve server.c -I.
	gcc -o hunt hunter.c -I.
	gcc -o prey prey.c -I.

run: input.txt
	./serve < input.txt
