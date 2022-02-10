CC_EXEC = g++
CC_FLAGS = -std=c++11
CC = $(CC_EXEC) $(CC_FLAGS)

all: ImageFilters.out
 
ImageFilters.out: main.o
	$(CC) main.o -o ImageFilters.out

main.o: main.cpp
	$(CC) -c main.cpp

.PHONY: cleans
clean:
	rm -rf *.o *.out