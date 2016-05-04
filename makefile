all: v1.a cleanObject

v1.a: v1.o
	ar crs ./lib/v1.a ./bin/v1.o
v1.o: ./src/v1.c libaux.a
	gcc -c -I./include -L./lib ./src/v1.c -laux -Wall -o ./bin/v1.o	
libaux.a: libaux.o
	ar crs ./lib/libaux.a ./bin/libaux.o
libaux.o: ./src/libaux.c
	gcc -c -I./include ./src/libaux.c -Wall -o ./bin/libaux.o
	
cleanObject:
	rm -f ./bin/*.o
clean:
	rm -f ./lib/libt2fs.a ./bin/*.o ./bin/cpy2t2 ./bin/mkdirt2 ./bin/dirt2 ./bin/rmdirt2
