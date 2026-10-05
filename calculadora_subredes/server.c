#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/* Comandos Telnet que interesan para saltarse la negociacion (RFC 854). */
#define TELNET_CMD_SE 240
#define TELNET_CMD_SB 250
#define TELNET_CMD_WILL 251
#define TELNET_CMD_DONT 254
#define TELNET_CMD_IAC 255

#define READ_CHUNK 4096
/* Respuestas pendientes de envio por cliente. Si un cliente deja de leer y
 * llena este buffer se le desconecta para que no afecte a los demas. */
#define CLIENT_OUT_MAX (4 * SERVER_OUTPUT_MAX)

#define MSG_LINE_TOO_LONG "ERROR: comando demasiado largo\r\n"
#define MSG_SERVER_FULL "ERROR: servidor lleno, intente mas tarde\r\n"

typedef struct {
  int fd;
  char address[INET_ADDRSTRLEN + 8];
  line_reader_t reader;
  char out[CLIENT_OUT_MAX];
  size_t out_len;
} client_t;

/* fds[0] es el socket de escucha y fds[i + 1] corresponde a clients[i]. */
static struct pollfd fds[SERVER_MAX_CLIENTS + 1];
static client_t clients[SERVER_MAX_CLIENTS];
static size_t client_count = 0;

static volatile sig_atomic_t stop_requested = 0;

void server_stop(void) { stop_requested = 1; }

void line_reader_init(line_reader_t *reader) {
  memset(reader, 0, sizeof(*reader));
  reader->state = TELNET_DATA;
}

static line_status_t line_reader_finish(line_reader_t *reader) {
  line_status_t status = reader->overflow ? LINE_TOO_LONG : LINE_READY;
  reader->line[reader->len] = '\0';
  reader->len = 0;
  reader->overflow = 0;
  return status;
}

line_status_t line_reader_feed(line_reader_t *reader, unsigned char byte) {
  switch (reader->state) {
  case TELNET_IAC:
    if (byte == TELNET_CMD_SB) {
      reader->state = TELNET_SUB;
    } else if (byte >= TELNET_CMD_WILL && byte <= TELNET_CMD_DONT) {
      reader->state = TELNET_OPTION;
    } else {
      reader->state = TELNET_DATA;
    }
    return LINE_PENDING;
  case TELNET_OPTION:
    reader->state = TELNET_DATA;
    return LINE_PENDING;
  case TELNET_SUB:
    if (byte == TELNET_CMD_IAC) {
      reader->state = TELNET_SUB_IAC;
    }
    return LINE_PENDING;
  case TELNET_SUB_IAC:
    reader->state = (byte == TELNET_CMD_SE) ? TELNET_DATA : TELNET_SUB;
    return LINE_PENDING;
  case TELNET_DATA:
    break;
  }

  if (byte == TELNET_CMD_IAC) {
    reader->state = TELNET_IAC;
    return LINE_PENDING;
  }

  /* Telnet termina las lineas con \r\n o \r\0; otros clientes mandan solo \n.
   * El \r ya cierra la linea, asi que el byte que lo acompana se descarta. */
  if (reader->skip_lf) {
    reader->skip_lf = 0;
    if (byte == '\n' || byte == '\0') {
      return LINE_PENDING;
    }
  }
  if (byte == '\r') {
    reader->skip_lf = 1;
    return line_reader_finish(reader);
  }
  if (byte == '\n') {
    return line_reader_finish(reader);
  }

  if (byte == '\t') {
    byte = ' ';
  }
  /* Solo pasa ASCII imprimible; el resto son caracteres de control. */
  if (byte < 0x20 || byte > 0x7E) {
    return LINE_PENDING;
  }

  if (reader->len + 1 >= SERVER_LINE_MAX) {
    reader->overflow = 1;
    return LINE_PENDING;
  }
  reader->line[reader->len++] = (char)byte;
  return LINE_PENDING;
}

static int set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    return -1;
  }
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int server_listen(uint16_t port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    perror("socket");
    return -1;
  }

  /* Permite reiniciar el servidor sin esperar a que expire TIME_WAIT. */
  int enable = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
    perror("setsockopt(SO_REUSEADDR)");
    close(fd);
    return -1;
  }

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(port);

  if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind");
    close(fd);
    return -1;
  }
  if (listen(fd, SOMAXCONN) < 0) {
    perror("listen");
    close(fd);
    return -1;
  }
  if (set_nonblocking(fd) < 0) {
    perror("fcntl(O_NONBLOCK)");
    close(fd);
    return -1;
  }
  return fd;
}

/* Cierra el descriptor y compacta los arreglos moviendo el ultimo cliente al
 * espacio que queda libre. */
static void client_remove(size_t index, const char *reason) {
  printf("[-] %s desconectado (%s)\n", clients[index].address, reason);
  close(clients[index].fd);

  size_t last = client_count - 1;
  if (index != last) {
    clients[index] = clients[last];
    fds[index + 1] = fds[last + 1];
  }
  client_count--;
}

/* Envia lo que el socket acepte sin bloquear. Devuelve 0 si la conexion ya no
 * sirve. */
static int client_flush(client_t *client) {
  while (client->out_len > 0) {
    ssize_t sent =
        send(client->fd, client->out, client->out_len, MSG_NOSIGNAL);
    if (sent < 0) {
      if (errno == EINTR) {
        continue;
      }
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return 1;
      }
      return 0;
    }
    client->out_len -= (size_t)sent;
    memmove(client->out, client->out + sent, client->out_len);
  }
  return 1;
}

/* Encola una respuesta convirtiendo cada fin de linea a \r\n, que es lo que
 * espera Telnet, y garantizando que termine en uno. Devuelve 0 si no cabe. */
static int client_queue(client_t *client, const char *text) {
  size_t len = client->out_len;
  char previous = '\n';

  for (const char *p = text; *p != '\0'; p++) {
    if (*p == '\r') {
      continue;
    }
    if (len + 2 > CLIENT_OUT_MAX) {
      return 0;
    }
    if (*p == '\n') {
      client->out[len++] = '\r';
    }
    client->out[len++] = *p;
    previous = *p;
  }
  if (previous != '\n') {
    if (len + 2 > CLIENT_OUT_MAX) {
      return 0;
    }
    client->out[len++] = '\r';
    client->out[len++] = '\n';
  }
  client->out_len = len;
  return 1;
}

/* Lee lo disponible y despacha cada comando completo. Devuelve 0 si hay que
 * cerrar la conexion y deja el motivo en reason. */
static int client_read(client_t *client, command_handler_t handler,
                       const char **reason) {
  unsigned char buffer[READ_CHUNK];
  char output[SERVER_OUTPUT_MAX];

  ssize_t received = recv(client->fd, buffer, sizeof(buffer), 0);
  if (received == 0) {
    *reason = "cierre del cliente";
    return 0;
  }
  if (received < 0) {
    if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
      return 1;
    }
    *reason = strerror(errno);
    return 0;
  }

  for (ssize_t i = 0; i < received; i++) {
    line_status_t status = line_reader_feed(&client->reader, buffer[i]);
    int queued = 1;

    if (status == LINE_TOO_LONG) {
      queued = client_queue(client, MSG_LINE_TOO_LONG);
    } else if (status == LINE_READY && client->reader.line[0] != '\0') {
      output[0] = '\0';
      handler(client->reader.line, output, sizeof(output));
      output[sizeof(output) - 1] = '\0';
      queued = client_queue(client, output);
    }
    if (!queued) {
      *reason = "buffer de salida lleno";
      return 0;
    }
  }

  if (!client_flush(client)) {
    *reason = strerror(errno);
    return 0;
  }
  return 1;
}

static void accept_clients(int listen_fd) {
  for (;;) {
    struct sockaddr_in address;
    socklen_t address_len = sizeof(address);
    int fd = accept(listen_fd, (struct sockaddr *)&address, &address_len);
    if (fd < 0) {
      if (errno == EINTR || errno == ECONNABORTED) {
        continue;
      }
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("accept");
      }
      return;
    }

    if (client_count == SERVER_MAX_CLIENTS) {
      send(fd, MSG_SERVER_FULL, strlen(MSG_SERVER_FULL), MSG_NOSIGNAL);
      close(fd);
      continue;
    }
    if (set_nonblocking(fd) < 0) {
      perror("fcntl(O_NONBLOCK)");
      close(fd);
      continue;
    }

    client_t *client = &clients[client_count];
    char ip[INET_ADDRSTRLEN] = "?";
    inet_ntop(AF_INET, &address.sin_addr, ip, sizeof(ip));
    snprintf(client->address, sizeof(client->address), "%s:%u", ip,
             (unsigned)ntohs(address.sin_port));
    client->fd = fd;
    client->out_len = 0;
    line_reader_init(&client->reader);

    fds[client_count + 1].fd = fd;
    fds[client_count + 1].revents = 0;
    client_count++;
    printf("[+] %s conectado (%zu activos)\n", client->address, client_count);
  }
}

int server_run(int listen_fd, command_handler_t handler) {
  int result = 0;

  fds[0].fd = listen_fd;
  fds[0].events = POLLIN;

  while (!stop_requested) {
    for (size_t i = 0; i < client_count; i++) {
      fds[i + 1].events = POLLIN;
      if (clients[i].out_len > 0) {
        fds[i + 1].events |= POLLOUT;
      }
    }

    if (poll(fds, client_count + 1, -1) < 0) {
      if (errno == EINTR) {
        continue;
      }
      perror("poll");
      result = -1;
      break;
    }

    /* Se recorre de atras hacia adelante porque client_remove mueve el ultimo
     * cliente, ya atendido, al espacio del que se elimina. */
    for (size_t i = client_count; i-- > 0;) {
      short revents = fds[i + 1].revents;
      const char *reason = "error en el socket";
      int alive = 1;

      if (revents & POLLNVAL) {
        alive = 0;
      } else if (revents & (POLLIN | POLLHUP | POLLERR)) {
        alive = client_read(&clients[i], handler, &reason);
      } else if (revents & POLLOUT) {
        alive = client_flush(&clients[i]);
      }
      if (!alive) {
        client_remove(i, reason);
      }
    }

    /* Los clientes nuevos se aceptan al final para no evaluar sus revents
     * antes de que pasen por poll. */
    if (fds[0].revents & POLLIN) {
      accept_clients(listen_fd);
    }
  }

  while (client_count > 0) {
    client_remove(client_count - 1, "servidor detenido");
  }
  return result;
}
