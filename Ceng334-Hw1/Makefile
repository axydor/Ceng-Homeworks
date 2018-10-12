all: server.c hunter.c prey.c
	gcc -o  server -g server.c -Wall
	gcc -o hunter hunter.c -Wall 
	gcc -o prey prey.c -Wall
run: input.txt
	./server < input.txt
