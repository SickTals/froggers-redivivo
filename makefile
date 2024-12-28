progetto.out: main.o 
	gcc main.o -Wall -o proj.out -lncurses
main.o: main.c main.h
	gcc -c -Wall main.c main.h
clean:
	rm -f *.o *.h.gch
