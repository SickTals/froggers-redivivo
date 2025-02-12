final: main.o menu.o ui.o frog.o river.o dens.o
	gcc main.o menu.o ui.o frog.o river.o dens.o -Wall -o proj.out -lncurses
	rm -f *.o *.h.gch
main.o: main.c 
	gcc -c -Wall main.c
menu.o: menu.c
	gcc -c -Wall menu.c
ui.o: ui.c
	gcc -c -Wall ui.c
frog.o: frog.c
	gcc -c -Wall frog.c
river.o: river.c
	gcc -c -Wall river.c
dens.o: dens.c
	gcc -c -Wall dens.c
clean:
	rm -f *.o *.h.gch *.out
