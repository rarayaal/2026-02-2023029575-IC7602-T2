#ifndef ROUTING_H
#define ROUTING_H

#include <stddef.h>
#include <stdint.h>

#define ROUTE_NAME_MAX 64

/* La implementacion interna de la tabla permanece oculta para sus usuarios. */
typedef struct route_table route_table_t;

typedef enum {
  ROUTE_STATUS_OK = 0,
  ROUTE_STATUS_INVALID_ARGUMENT,
  ROUTE_STATUS_INVALID_PREFIX,
  ROUTE_STATUS_NAME_TOO_LONG,
  ROUTE_STATUS_OUT_OF_MEMORY,
  ROUTE_STATUS_NOT_FOUND,
  ROUTE_STATUS_NO_MATCH,
  ROUTE_STATUS_INVALID_COUNT,
  ROUTE_STATUS_SUBNET_TOO_LARGE,
  ROUTE_STATUS_INSUFFICIENT_SPACE
} route_status_t;

typedef struct {
  char name[ROUTE_NAME_MAX];
  uint32_t network;
  uint8_t prefix;
  int priority;
} route_match_t;

typedef struct {
  uint32_t network;
  uint8_t prefix;
} route_subnet_t;

/* Crea una tabla vacia o devuelve NULL si no hay memoria. */
route_table_t *route_table_create(void);

/* Libera todas las rutas y acepta NULL para facilitar la limpieza. */
void route_table_destroy(route_table_t *table);

/* Inserta una ruta o actualiza la ruta que tenga el mismo nombre. */
route_status_t route_set(route_table_t *table, const char *name,
                         uint32_t network, int prefix, int priority);

/* Elimina una ruta por nombre y libera su memoria. */
route_status_t route_delete(route_table_t *table, const char *name);

/* Devuelve la mejor coincidencia segun prefijo y prioridad. */
route_status_t route_lookup(const route_table_t *table, uint32_t ip,
                            route_match_t *match);

/* Igual que route_lookup, pero agrega una traza explicativa si se solicita. */
route_status_t route_lookup_debug(const route_table_t *table, uint32_t ip,
                                  route_match_t *match, char *trace,
                                  size_t trace_len);

/* Llena un arreglo con subredes aleatorias no repetidas dentro del padre. */
route_status_t route_generate_random_subnets(uint32_t parent_network,
                                             int parent_prefix,
                                             int subnet_prefix, size_t count,
                                             route_subnet_t *subnets);

#endif