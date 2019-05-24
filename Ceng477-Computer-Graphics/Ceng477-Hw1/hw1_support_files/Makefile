CXX = g++
CXXFLAGS = -std=c++11 -O3 -pthread -lpthread #-Wall -pg -g 

.PHONY: all

all: raytracer

raytracer: raytracer.o parser.o ppm.o tinyxml2.o main.o
	$(CXX) $(CXXFLAGS) -o raytracer main.o raytracer.o parser.o ppm.o tinyxml2.o

main.o: main.cpp parser.h ppm.h raytracer.h
	$(CXX) $(CXXFLAGS) -c main.cpp

raytracer.o: raytracer.cpp raytracer.h parser.h
	$(CXX) $(CXXFLAGS) -Wno-narrowing -c raytracer.cpp

parser.o: parser.cpp parser.h tinyxml2.h
	$(CXX) $(CXXFLAGS) -c parser.cpp

tinyxml2.o: tinyxml2.cpp tinyxml2.h
	$(CXX) $(CXXFLAGS) -c tinyxml2.cpp

ppm.o: ppm.cpp ppm.h
	$(CXX) $(CXXFLAGS) -c ppm.cpp

clean:
	rm raytracer *.o *.ppm
