CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu11

all: prueba_calculo_subredes servidor prueba_parser prueba_routing prueba_param_extractor prueba_error_catalog prueba_dispatcher prueba_sintactica

prueba_calculo_subredes: calculadora_subredes/calculo_subredes.o calculadora_subredes/prueba_calculo_subredes.o
	$(CC) $(CFLAGS) -o prueba_calculo_subredes calculadora_subredes/calculo_subredes.o calculadora_subredes/prueba_calculo_subredes.o
	
calculadora_subredes/calculo_subredes.o: calculadora_subredes/calculo_subredes.c calculadora_subredes/calculo_subredes.h
	$(CC) $(CFLAGS) -c calculadora_subredes/calculo_subredes.c -o calculadora_subredes/calculo_subredes.o

calculadora_subredes/prueba_calculo_subredes.o: calculadora_subredes/prueba_calculo_subredes.c calculadora_subredes/calculo_subredes.h
	$(CC) $(CFLAGS) -c calculadora_subredes/prueba_calculo_subredes.c -o calculadora_subredes/prueba_calculo_subredes.o

calculadora_subredes/error_catalog.o: calculadora_subredes/error_catalog.c calculadora_subredes/error_catalog.h
	$(CC) $(CFLAGS) -c calculadora_subredes/error_catalog.c -o calculadora_subredes/error_catalog.o

calculadora_subredes/prueba_error_catalog.o: calculadora_subredes/prueba_error_catalog.c calculadora_subredes/error_catalog.h calculadora_subredes/parser.h
	$(CC) $(CFLAGS) -c calculadora_subredes/prueba_error_catalog.c -o calculadora_subredes/prueba_error_catalog.o

calculadora_subredes/parser.o: calculadora_subredes/parser.c calculadora_subredes/parser.h calculadora_subredes/error_catalog.h calculadora_subredes/calculo_subredes.h calculadora_subredes/routing.h calculadora_subredes/dispatcher.h calculadora_subredes/param_extractor.h
	$(CC) $(CFLAGS) -c calculadora_subredes/parser.c -o calculadora_subredes/parser.o

calculadora_subredes/prueba_parser.o: calculadora_subredes/prueba_parser.c calculadora_subredes/parser.h
	$(CC) $(CFLAGS) -c calculadora_subredes/prueba_parser.c -o calculadora_subredes/prueba_parser.o

calculadora_subredes/prueba_sintactica.o: calculadora_subredes/prueba_sintactica.c calculadora_subredes/parser.h
	$(CC) $(CFLAGS) -c calculadora_subredes/prueba_sintactica.c -o calculadora_subredes/prueba_sintactica.o

calculadora_subredes/param_extractor.o: calculadora_subredes/param_extractor.c calculadora_subredes/param_extractor.h calculadora_subredes/parser.h
	$(CC) $(CFLAGS) -c calculadora_subredes/param_extractor.c -o calculadora_subredes/param_extractor.o

calculadora_subredes/prueba_param_extractor.o: calculadora_subredes/prueba_param_extractor.c calculadora_subredes/param_extractor.h calculadora_subredes/parser.h
	$(CC) $(CFLAGS) -c calculadora_subredes/prueba_param_extractor.c -o calculadora_subredes/prueba_param_extractor.o

calculadora_subredes/dispatcher.o: calculadora_subredes/dispatcher.c calculadora_subredes/dispatcher.h calculadora_subredes/param_extractor.h calculadora_subredes/calculo_subredes.h calculadora_subredes/error_catalog.h
	$(CC) $(CFLAGS) -c calculadora_subredes/dispatcher.c -o calculadora_subredes/dispatcher.o

calculadora_subredes/prueba_dispatcher.o: calculadora_subredes/prueba_dispatcher.c calculadora_subredes/dispatcher.h
	$(CC) $(CFLAGS) -c calculadora_subredes/prueba_dispatcher.c -o calculadora_subredes/prueba_dispatcher.o

calculadora_subredes/routing.o: calculadora_subredes/routing.c calculadora_subredes/routing.h
	$(CC) $(CFLAGS) -c calculadora_subredes/routing.c -o calculadora_subredes/routing.o

calculadora_subredes/prueba_routing.o: calculadora_subredes/prueba_routing.c calculadora_subredes/routing.h
	$(CC) $(CFLAGS) -c calculadora_subredes/prueba_routing.c -o calculadora_subredes/prueba_routing.o

test: prueba_calculo_subredes prueba_parser prueba_routing prueba_param_extractor prueba_error_catalog prueba_dispatcher prueba_sintactica test_server
	./prueba_calculo_subredes
	./prueba_parser
	./prueba_routing
	./prueba_param_extractor
	./prueba_error_catalog
	./prueba_dispatcher
	./prueba_sintactica

# Servidor TCP. Enlaza el parser real (T-08) y el despacho a los modulos de calculo (T-13).
servidor: calculadora_subredes/main.o calculadora_subredes/server.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/error_catalog.o calculadora_subredes/routing.o calculadora_subredes/dispatcher.o calculadora_subredes/param_extractor.o
	$(CC) $(CFLAGS) -o servidor calculadora_subredes/main.o calculadora_subredes/server.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/error_catalog.o calculadora_subredes/routing.o calculadora_subredes/dispatcher.o calculadora_subredes/param_extractor.o

prueba_server: calculadora_subredes/prueba_server.o calculadora_subredes/server.o
	$(CC) $(CFLAGS) -o prueba_server calculadora_subredes/prueba_server.o calculadora_subredes/server.o

prueba_parser: calculadora_subredes/prueba_parser.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/error_catalog.o calculadora_subredes/routing.o calculadora_subredes/dispatcher.o calculadora_subredes/param_extractor.o
	$(CC) $(CFLAGS) -o prueba_parser calculadora_subredes/prueba_parser.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/error_catalog.o calculadora_subredes/routing.o calculadora_subredes/dispatcher.o calculadora_subredes/param_extractor.o

prueba_sintactica: calculadora_subredes/prueba_sintactica.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/error_catalog.o calculadora_subredes/routing.o calculadora_subredes/dispatcher.o calculadora_subredes/param_extractor.o
	$(CC) $(CFLAGS) -o prueba_sintactica calculadora_subredes/prueba_sintactica.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/error_catalog.o calculadora_subredes/routing.o calculadora_subredes/dispatcher.o calculadora_subredes/param_extractor.o

prueba_param_extractor: calculadora_subredes/prueba_param_extractor.o calculadora_subredes/param_extractor.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/error_catalog.o calculadora_subredes/routing.o calculadora_subredes/dispatcher.o
	$(CC) $(CFLAGS) -o prueba_param_extractor calculadora_subredes/prueba_param_extractor.o calculadora_subredes/param_extractor.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/error_catalog.o calculadora_subredes/routing.o calculadora_subredes/dispatcher.o

prueba_error_catalog: calculadora_subredes/prueba_error_catalog.o calculadora_subredes/error_catalog.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/routing.o calculadora_subredes/dispatcher.o calculadora_subredes/param_extractor.o
	$(CC) $(CFLAGS) -o prueba_error_catalog calculadora_subredes/prueba_error_catalog.o calculadora_subredes/error_catalog.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/routing.o calculadora_subredes/dispatcher.o calculadora_subredes/param_extractor.o

prueba_dispatcher: calculadora_subredes/prueba_dispatcher.o calculadora_subredes/dispatcher.o calculadora_subredes/param_extractor.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/error_catalog.o calculadora_subredes/routing.o
	$(CC) $(CFLAGS) -Wl,--allow-multiple-definition -o prueba_dispatcher calculadora_subredes/prueba_dispatcher.o calculadora_subredes/dispatcher.o calculadora_subredes/param_extractor.o calculadora_subredes/parser.o calculadora_subredes/calculo_subredes.o calculadora_subredes/error_catalog.o calculadora_subredes/routing.o

prueba_routing: calculadora_subredes/prueba_routing.o calculadora_subredes/routing.o
	$(CC) $(CFLAGS) -o prueba_routing calculadora_subredes/prueba_routing.o calculadora_subredes/routing.o

calculadora_subredes/main.o calculadora_subredes/server.o calculadora_subredes/prueba_server.o: calculadora_subredes/server.h

test_server: prueba_server
	./prueba_server

run: servidor
	./servidor

clean:
	rm -f prueba_calculo_subredes servidor prueba_server prueba_parser prueba_routing prueba_param_extractor prueba_error_catalog prueba_dispatcher prueba_sintactica calculadora_subredes/*.o

.PHONY: all test test_server run clean prueba_param_extractor prueba_error_catalog prueba_dispatcher prueba_sintactica

