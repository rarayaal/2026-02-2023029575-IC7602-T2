#include "routing.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct route_entry {
  char name[ROUTE_NAME_MAX];
  uint32_t network;
  uint8_t prefix;
  int priority;
  struct route_entry *next;
} route_entry_t;

struct route_table {
  route_entry_t *head;
};

/* Evita desplazamientos invalidos en los casos especiales /0 y /32. */
static uint32_t prefix_mask(int prefix) {
  if (prefix == 0) return 0U;
  if (prefix == 32) return UINT32_MAX;
  return UINT32_MAX << (32 - prefix);
}

static route_entry_t *find_route(route_table_t *table, const char *name) {
  for (route_entry_t *entry = table->head; entry != NULL; entry = entry->next) {
    if (strcmp(entry->name, name) == 0) return entry;
  }
  return NULL;
}

route_table_t *route_table_create(void) {
  return calloc(1, sizeof(route_table_t));
}

void route_table_destroy(route_table_t *table) {
  if (!table) return;

  /* Cada nodo es propiedad de la tabla y debe liberarse antes de ella. */
  route_entry_t *entry = table->head;
  while (entry) {
    route_entry_t *next = entry->next;
    free(entry);
    entry = next;
  }
  free(table);
}

route_status_t route_set(route_table_t *table, const char *name,
                         uint32_t network, int prefix, int priority) {
  if (!table || !name || name[0] == '\0') {
    return ROUTE_STATUS_INVALID_ARGUMENT;
  }
  if (strlen(name) >= ROUTE_NAME_MAX) {
    return ROUTE_STATUS_NAME_TOO_LONG;
  }
  if (prefix < 0 || prefix > 32) {
    return ROUTE_STATUS_INVALID_PREFIX;
  }

  route_entry_t *entry = find_route(table, name);
  if (!entry) {
    entry = calloc(1, sizeof(*entry));
    if (!entry) return ROUTE_STATUS_OUT_OF_MEMORY;

    strcpy(entry->name, name);
    entry->next = table->head;
    table->head = entry;
  }

  /* La red se guarda alineada al inicio del bloque indicado por el prefijo. */
  entry->network = network & prefix_mask(prefix);
  entry->prefix = (uint8_t)prefix;
  entry->priority = priority;
  return ROUTE_STATUS_OK;
}

route_status_t route_delete(route_table_t *table, const char *name) {
  if (!table || !name || name[0] == '\0') {
    return ROUTE_STATUS_INVALID_ARGUMENT;
  }

  route_entry_t **current = &table->head;
  while (*current) {
    if (strcmp((*current)->name, name) == 0) {
      /* Usar el puntero al enlace permite borrar tambien el primer nodo. */
      route_entry_t *removed = *current;
      *current = removed->next;
      free(removed);
      return ROUTE_STATUS_OK;
    }
    current = &(*current)->next;
  }

  return ROUTE_STATUS_NOT_FOUND;
}

static void format_ip(uint32_t ip, char *buffer, size_t buffer_len) {
  snprintf(buffer, buffer_len, "%u.%u.%u.%u", (ip >> 24) & 0xFFU,
           (ip >> 16) & 0xFFU, (ip >> 8) & 0xFFU, ip & 0xFFU);
}

static void append_trace(char *trace, size_t trace_len, const char *format,
                         ...) {
  if (!trace || trace_len == 0) return;

  size_t used = strlen(trace);
  if (used >= trace_len - 1) return;

  va_list args;
  va_start(args, format);
  vsnprintf(trace + used, trace_len - used, format, args);
  va_end(args);
}

static route_status_t lookup_internal(const route_table_t *table, uint32_t ip,
                                      route_match_t *match, char *trace,
                                      size_t trace_len) {
  if (!table || !match) return ROUTE_STATUS_INVALID_ARGUMENT;
  if (trace && trace_len == 0) return ROUTE_STATUS_INVALID_ARGUMENT;

  if (trace) trace[0] = '\0';
  bool found = false;
  route_entry_t best = {0};

  for (route_entry_t *entry = table->head; entry != NULL; entry = entry->next) {
    uint32_t mask = prefix_mask(entry->prefix);
    uint32_t masked_ip = ip & mask;
    bool matches = masked_ip == entry->network;

    if (trace) {
      char network_text[16];
      char masked_text[16];
      format_ip(entry->network, network_text, sizeof(network_text));
      format_ip(masked_ip, masked_text, sizeof(masked_text));
      append_trace(trace, trace_len,
                   "Evaluada %s/%u: IP AND mascara = %s, red = %s, %s.\n",
                   entry->name, entry->prefix, masked_text, network_text,
                   matches ? "coincide" : "no coincide");
    }

    /* Primero gana el prefijo mas especifico; la prioridad solo desempata. */
    if (matches &&
        (!found || entry->prefix > best.prefix ||
         (entry->prefix == best.prefix && entry->priority > best.priority))) {
      best = *entry;
      found = true;
    }
  }

  if (!found) {
    if (trace) append_trace(trace, trace_len, "Resultado: no existe una ruta.\n");
    return ROUTE_STATUS_NO_MATCH;
  }

  snprintf(match->name, sizeof(match->name), "%s", best.name);
  match->network = best.network;
  match->prefix = best.prefix;
  match->priority = best.priority;

  if (trace) {
    append_trace(trace, trace_len,
                 "Seleccionada %s/%u con prioridad %d por Longest Prefix Match.\n",
                 best.name, best.prefix, best.priority);
  }
  return ROUTE_STATUS_OK;
}

route_status_t route_lookup(const route_table_t *table, uint32_t ip,
                            route_match_t *match) {
  return lookup_internal(table, ip, match, NULL, 0);
}

route_status_t route_lookup_debug(const route_table_t *table, uint32_t ip,
                                  route_match_t *match, char *trace,
                                  size_t trace_len) {
  if (!trace || trace_len == 0) return ROUTE_STATUS_INVALID_ARGUMENT;
  return lookup_internal(table, ip, match, trace, trace_len);
}

route_status_t route_generate_random_subnets(uint32_t parent_network,
                                             int parent_prefix,
                                             int subnet_prefix, size_t count,
                                             route_subnet_t *subnets) {
  if (!subnets && count > 0) return ROUTE_STATUS_INVALID_ARGUMENT;
  if (parent_prefix < 0 || parent_prefix > 32 || subnet_prefix < 0 ||
      subnet_prefix > 32) {
    return ROUTE_STATUS_INVALID_PREFIX;
  }
  if (subnet_prefix < parent_prefix) return ROUTE_STATUS_SUBNET_TOO_LARGE;
  if (count == 0) return ROUTE_STATUS_INVALID_COUNT;

  unsigned int difference = (unsigned int)(subnet_prefix - parent_prefix);
  uint64_t capacity = 1ULL << difference;
  if ((uint64_t)count > capacity) return ROUTE_STATUS_INSUFFICIENT_SPACE;

  uint32_t parent_mask = prefix_mask(parent_prefix);
  uint32_t block_mask = prefix_mask(subnet_prefix);
  uint32_t normalized_parent = parent_network & parent_mask;
  uint64_t block_size = 1ULL << (32 - subnet_prefix);
  uint64_t state = (uint64_t)time(NULL) ^ normalized_parent ^ count;
  uint64_t start = state & (capacity - 1ULL);
  uint64_t step = ((state >> 16) | 1ULL) & (capacity - 1ULL);
  if (step == 0) step = 1;

  for (size_t index = 0; index < count; index++) {
    /* Cada indice identifica un bloque distinto del mismo tamano. */
    uint64_t subnet_index = (start + (uint64_t)index * step) & (capacity - 1ULL);
    uint64_t offset = subnet_index * block_size;
    subnets[index].network = normalized_parent | (uint32_t)offset;
    subnets[index].network &= block_mask;
    subnets[index].prefix = (uint8_t)subnet_prefix;
  }

  return ROUTE_STATUS_OK;
}