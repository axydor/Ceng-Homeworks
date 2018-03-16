all: server.c hunter.c prey.c
	gcc -o server -g server.c
	gcc -o hunter hunter.c 
	gcc -o prey prey.c
