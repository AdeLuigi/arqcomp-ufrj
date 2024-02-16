###MakeFile#####
all: main
main: main.o
	gcc -o main main.o
main.o: main.c process.h variables.h  queue.h
	gcc -o main.o -c main.c -W -Wall -ansi -pedantic
clean:
	rm -rf *.o
mrproper: clean
	rm - rf all
