#ifndef SERVER_H
#define SERVER_H

#include <stddef.h>
#include <stdint.h>

#define SERVER_DEFAULT_PORT 9666
#define SERVER_MAX_CLIENTS 64
/* Largo maximo de un comando, contando el terminador nulo. */
#define SERVER_LINE_MAX 1024
/* Tamano del buffer que se le entrega al parser para escribir la respuesta. */
#define SERVER_OUTPUT_MAX 4096

/* Contrato con el parser (P2): recibe una linea ya saneada y terminada en nulo
 * y escribe en output, sin pasarse de max_len, la respuesta para el cliente. */
typedef void (*command_handler_t)(const char *input, char *output,
                                  size_t max_len);

/* Posicion dentro de una secuencia de negociacion Telnet (RFC 854). */
typedef enum {
  TELNET_DATA = 0,
  TELNET_IAC,
  TELNET_OPTION,
  TELNET_SUB,
  TELNET_SUB_IAC
} telnet_state_t;

typedef enum { LINE_PENDING = 0, LINE_READY, LINE_TOO_LONG } line_status_t;

/* Ensambla lineas a partir de los bytes crudos de una sesion. Guarda el estado
 * entre lecturas porque TCP puede partir un comando en varios segmentos. */
typedef struct {
  char line[SERVER_LINE_MAX];
  size_t len;
  int overflow;
  int skip_lf;
  telnet_state_t state;
} line_reader_t;

void line_reader_init(line_reader_t *reader);

/* Procesa un byte. Devuelve LINE_READY cuando reader->line contiene un comando
 * completo terminado en nulo (puede ser vacio), LINE_TOO_LONG si la linea que
 * acaba de terminar excedia SERVER_LINE_MAX y LINE_PENDING en otro caso. */
line_status_t line_reader_feed(line_reader_t *reader, unsigned char byte);

/* Crea el socket de escucha en el puerto dado. Devuelve el descriptor o -1. */
int server_listen(uint16_t port);

/* Atiende clientes hasta que se llame a server_stop. Devuelve 0 si termino de
 * forma ordenada y -1 si fallo poll. No cierra listen_fd. */
int server_run(int listen_fd, command_handler_t handler);

/* Pide la detencion del ciclo principal. Es segura dentro de un manejador de
 * senales. */
void server_stop(void);

#endif
