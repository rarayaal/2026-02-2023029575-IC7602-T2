CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu11

all: prueba_calculo_subredes	

prueba_calculo_subredes: calculadora_subredes/calculo_subredes.o calculadora_subredes/prueba_calculo_subredes.o
	$(CC) $(CFLAGS) -o prueba_calculo_subredes calculadora_subredes/calculo_subredes.o calculadora_subredes/prueba_calculo_subredes.o
	
calculo_subredes.o: calculadora_subredes/calculo_subredes.c calculadora_subredes/calculo_subredes.h
	$(CC) $(CFLAGS) -c calculadora_subredes/calculo_subredes.c

prueba_calculo_subredes.o: calculadora_subredes/prueba_calculo_subredes.c calculadora_subredes/calculo_subredes.h
	$(CC) $(CFLAGS) -c calculadora_subredes/prueba_calculo_subredes.c

test: calculadora_subredes/prueba_calculo_subredes
	./calculadora_subredes/prueba_calculo_subredes

clean:
	rm -f prueba_calculo_subredes calculadora_subredes/*.o

.PHONY: all test clean
