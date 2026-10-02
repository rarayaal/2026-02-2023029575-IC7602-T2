#include "server.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Punto de entrada del parser (P2). Se enlaza con parser.c o, mientras este no
 * exista, con parser_stub.c. */
void handle_command(const char *input, char *output, size_t max_len);

static void on_signal(int signal_number) {
  (void)signal_number;
  server_stop();
}

/* Devuelve 0 si el texto no es un puerto valido (1-65535). */
static uint16_t parse_port(const char *text) {
  char *end = NULL;
  long value = strtol(text, &end, 10);
  if (*text == '\0' || *end != '\0' || value < 1 || value > 65535) {
    return 0;
  }
  return (uint16_t)value;
}

int main(int argc, char *argv[]) {
  uint16_t port = SERVER_DEFAULT_PORT;

  if (argc > 2) {
    fprintf(stderr, "Uso: %s [puerto]\n", argv[0]);
    return EXIT_FAILURE;
  }
  if (argc == 2) {
    port = parse_port(argv[1]);
    if (port == 0) {
      fprintf(stderr, "Puerto invalido: %s (debe estar entre 1 y 65535)\n",
              argv[1]);
      return EXIT_FAILURE;
    }
  }

  /* Los logs deben verse de inmediato aunque stdout no sea una terminal, como
   * pasa dentro de un contenedor. */
  setvbuf(stdout, NULL, _IOLBF, 0);

  /* Sin SA_RESTART para que poll se interrumpa y el ciclo vea la detencion.
   * SIGTERM es la senal que envia Kubernetes al terminar el pod. */
  struct sigaction action;
  memset(&action, 0, sizeof(action));
  action.sa_handler = on_signal;
  sigemptyset(&action.sa_mask);
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTERM, &action, NULL);
  signal(SIGPIPE, SIG_IGN);

  int listen_fd = server_listen(port);
  if (listen_fd < 0) {
    fprintf(stderr, "No se pudo iniciar el servidor en el puerto %u\n",
            (unsigned)port);
    return EXIT_FAILURE;
  }
  printf("Servidor escuchando en el puerto %u\n", (unsigned)port);

  int result = server_run(listen_fd, handle_command);

  close(listen_fd);
  printf("Servidor detenido\n");
  return result == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
