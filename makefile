final: main.o menu.o frog.o
	gcc main.o menu.o frog.o -Wall -o proj.out -lncurses
	rm -f *.o *.h.gch
main.o: main.c 
	gcc -c -Wall main.c
menu.o: menu.c
	gcc -c -Wall menu.c
frog.o: frog.c
	gcc -c -Wall frog.c
clean:
	rm -f *.o *.h.gch *.out
