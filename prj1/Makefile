testlib: main.o bitmap.o debug.o hash.o hex_dump.o list.o
	gcc -o testlib main.o bitmap.o debug.o hash.o hex_dump.o list.o
main.o : main.c
	gcc -c -o main.o main.c

bitmap.o : bitmap.c
	gcc -c -o bitmap.o bitmap.c

debug.o : debug.c
	gcc -c -o debug.o debug.c

hash.o : hash.c
	gcc -c -o hash.o hash.c

hex_dum.o : hex_dum.c
	gcc -c -o hex_dum.o hex_dum.c

list.o : list.c
	gcc -c -o list.o list.c

clean:
	rm *.o testlib