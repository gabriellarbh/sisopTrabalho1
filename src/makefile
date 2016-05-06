all: v1.a cleanObject

teste: ./testes/teste.c
	gcc -o ./testes/teste ./testes/teste.c -I./include -L./lib -lv1 -Wall

v1.a: v1.o
	ar crs ./lib/libv1.a ./bin/v1.o ./bin/fila2.o
v1.o: ./src/v1.c
	gcc -c -I./include ./src/v1.c -Wall -o ./bin/v1.o
	
cleanObject:
	rm -f ./bin/v1.o
clean:
	rm -f ./lib/*.a ./bin/v1.o
