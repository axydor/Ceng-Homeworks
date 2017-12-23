CC=g++ -std=c++11 -Wall -Wextra -O2
CFLAGS=-I.

runTH: graph.cpp
	$(CC) -o runTH graph.cpp -I.
