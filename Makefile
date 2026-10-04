CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu11

all: prueba_calculo_subredes servidor prueba_parser

prueba_calculo_subredes: calculadora_subredes/calculo_subredes.o calculadora_subredes/prueba_calculo_subredes.o
	$(CC) $(CFLAGS) -o prueba_calculo_subredes calculadora_subredes/calculo_subredes.o calculadora_subredes/prueba_calculo_subredes.o
	
calculadora_subredes/calculo_subredes.o: calculadora_subredes/calculo_subredes.c calculadora_subredes/calculo_subredes.h
	$(CC) $(CFLAGS) -c calculadora_subredes/calculo_subredes.c -o calculadora_subredes/calculo_subredes.o

calculadora_subredes/prueba_calculo_subredes.o: calculadora_subredes/prueba_calculo_subredes.c calculadora_subredes/calculo_subredes.h
	$(CC) $(CFLAGS) -c calculadora_subredes/prueba_calculo_subredes.c -o calculadora_subredes/prueba_calculo_subredes.o

calculadora_subredes/parser.o: calculadora_subredes/parser.c calculadora_subredes/parser.h
	$(CC) $(CFLAGS) -c calculadora_subredes/parser.c -o calculadora_subredes/parser.o

calculadora_subredes/prueba_parser.o: calculadora_subredes/prueba_parser.c calculadora_subredes/parser.h
	$(CC) $(CFLAGS) -c calculadora_subredes/prueba_parser.c -o calculadora_subredes/prueba_parser.o

test: prueba_calculo_subredes prueba_parser test_server
	./prueba_calculo_subredes
	./prueba_parser

# Servidor TCP. Enlaza el parser real T-08.
servidor: calculadora_subredes/main.o calculadora_subredes/server.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o
	$(CC) $(CFLAGS) -o servidor calculadora_subredes/main.o calculadora_subredes/server.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o

prueba_server: calculadora_subredes/prueba_server.o calculadora_subredes/server.o
	$(CC) $(CFLAGS) -o prueba_server calculadora_subredes/prueba_server.o calculadora_subredes/server.o

prueba_parser: calculadora_subredes/prueba_parser.o calculadora_subredes/parser.o
	$(CC) $(CFLAGS) -o prueba_parser calculadora_subredes/prueba_parser.o calculadora_subredes/parser.o

calculadora_subredes/main.o calculadora_subredes/server.o calculadora_subredes/prueba_server.o: calculadora_subredes/server.h

test_server: prueba_server
	./prueba_server

run: servidor
	./servidor

clean:
	rm -f prueba_calculo_subredes servidor prueba_server prueba_parser calculadora_subredes/*.o

.PHONY: all test test_server run clean
