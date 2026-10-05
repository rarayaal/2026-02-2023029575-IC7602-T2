#include "routing.h"

#include <stdlib.h>
#include <string.h>

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
      route_entry_t *removed = *current;
      *current = removed->next;
      free(removed);
      return ROUTE_STATUS_OK;
    }
    current = &(*current)->next;
  }

  return ROUTE_STATUS_NOT_FOUND;
}