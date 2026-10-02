#ifndef ROUTING_H
#define ROUTING_H

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
  ROUTE_STATUS_NO_MATCH
} route_status_t;

typedef struct {
  char name[ROUTE_NAME_MAX];
  uint32_t network;
  uint8_t prefix;
  int priority;
} route_match_t;

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

#endif