CC_EXEC = g++
CC_FLAGS = -std=c++11
CC = $(CC_EXEC) $(CC_FLAGS)
LINK_FLAGS = -pthread

all: ImageFilters.out 
 
ImageFilters.out: main.o
	$(CC) $(LINK_FLAGS) main.o -o ImageFilters.out

main.o: main.cpp
	$(CC) $(LINK_FLAGS) -c main.cpp

.PHONY: cleans
clean:
	rm -rf *.o *.out