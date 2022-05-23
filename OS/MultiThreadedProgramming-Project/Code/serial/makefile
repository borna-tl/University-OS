CC=g++
CFLAGS = -std=c++17 -pthread -Wall -Werror

ImageFilters.out: main.o
	$(CC) $(CFLAGS) -o ImageFilters.out main.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

.PHONY: clean
clean:
	rm -rf *.o *.out