all: v1.a cleanObject

v1.a: v1.o
	ar crs ./lib/v1.a ./bin/v1.o
v1.o: ./src/v1.c libfila.a
	gcc -c -I./include -L./lib ./src/v1.c -lfila -Wall -o ./bin/v1.o
libfila.a: ./bin/fila2.o
	ar crs ./lib/libfila.a ./bin/fila2.o
	
cleanObject:
	rm -f ./bin/v1.o
clean:
	rm -f ./lib/*.a ./bin/v1.o
