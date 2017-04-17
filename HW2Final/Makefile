all: utils.h myShell.o
	gcc utils.h myShell.o -o myShell.out

utils.o: utils.c
	gcc -c utils.c

myShell.o: myShell.c
	gcc -c myShell.c

clean:
	rm -rf *.o
	rm -f *.out
	rm -f *~*.h.gch *#[