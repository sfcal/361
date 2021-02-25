all: mp3

mp3: main.o add.o print.o freeList.o deletemp3.o
	gcc -g main.o add.o print.o freeList.o deletemp3.o -o mp3

main.o: main.c mp3.h
	gcc -g -c main.c

add.o: add.c mp3.h
	gcc -g -c add.c

print.o: print.c mp3.h
	gcc -g -c print.c

freeList.o: freeList.c mp3.h
	gcc -g -c freeList.c

deletemp3.0: deletemp3.c mp3.h
	gss -g -c deletemp3.c

clean:
	\rm *.o mp3
