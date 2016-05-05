all: v1.a cleanObject

v1.a: v1.o
	ar crs ./lib/v1.a ./bin/v1.o
v1.o: ./src/v1.c libaux.a
	gcc -c -I./include -L./lib ./src/v1.c -laux -Wall -o ./bin/v1.o	
libaux.a: libaux.o
	ar crs ./lib/libaux.a ./bin/libaux.o
libaux.o: ./src/libaux.c
	gcc -c -I./include -L./lib ./src/libaux.c -lfila -Wall -o ./bin/libaux.o
libfila.a: ./bin/fila.o
	ar crs ./lib/libfila.a ./bin/fila.o
	
cleanObject:
	rm -f ./bin/libaux.o ./bin/v1.o
clean:
	rm -f ./lib/libt2fs.a ./bin/*.o
