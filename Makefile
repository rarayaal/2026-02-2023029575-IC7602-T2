CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu11

all: prueba_calculo_subredes servidor

prueba_calculo_subredes: calculadora_subredes/calculo_subredes.o calculadora_subredes/prueba_calculo_subredes.o
	$(CC) $(CFLAGS) -o prueba_calculo_subredes calculadora_subredes/calculo_subredes.o calculadora_subredes/prueba_calculo_subredes.o
	
calculo_subredes.o: calculadora_subredes/calculo_subredes.c calculadora_subredes/calculo_subredes.h
	$(CC) $(CFLAGS) -c calculadora_subredes/calculo_subredes.c

prueba_calculo_subredes.o: calculadora_subredes/prueba_calculo_subredes.c calculadora_subredes/calculo_subredes.h
	$(CC) $(CFLAGS) -c calculadora_subredes/prueba_calculo_subredes.c

test: calculadora_subredes/prueba_calculo_subredes
	./calculadora_subredes/prueba_calculo_subredes

# Servidor TCP. Enlaza el handle_command provisional hasta que exista el parser.
servidor: calculadora_subredes/main.o calculadora_subredes/server.o calculadora_subredes/parser_stub.o
	$(CC) $(CFLAGS) -o servidor calculadora_subredes/main.o calculadora_subredes/server.o calculadora_subredes/parser_stub.o

prueba_server: calculadora_subredes/prueba_server.o calculadora_subredes/server.o
	$(CC) $(CFLAGS) -o prueba_server calculadora_subredes/prueba_server.o calculadora_subredes/server.o

calculadora_subredes/main.o calculadora_subredes/server.o calculadora_subredes/prueba_server.o: calculadora_subredes/server.h

test_server: prueba_server
	./prueba_server

run: servidor
	./servidor

clean:
	rm -f prueba_calculo_subredes servidor prueba_server calculadora_subredes/*.o

.PHONY: all test test_server run clean
