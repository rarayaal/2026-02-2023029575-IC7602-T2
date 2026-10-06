#include "routing.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_crear_y_destruir_tabla(void) {
  route_table_t *table = route_table_create();
  assert(table != NULL);
  route_table_destroy(table);
  route_table_destroy(NULL);
  printf("[OK] Crear y destruir tabla\n");
}

static void test_insertar_y_actualizar_ruta(void) {
  route_table_t *table = route_table_create();
  route_match_t match;
  assert(table != NULL);

  assert(route_set(table, "oficina", 0x0A080205U, 29, 3) == ROUTE_STATUS_OK);
  assert(route_set(table, "oficina", 0xC0A80199U, 24, 8) == ROUTE_STATUS_OK);
  assert(route_lookup(table, 0xC0A80105U, &match) == ROUTE_STATUS_OK);
  assert(strcmp(match.name, "oficina") == 0);
  assert(match.network == 0xC0A80100U);
  assert(match.prefix == 24);
  assert(match.priority == 8);

  route_table_destroy(table);
  printf("[OK] Insertar y actualizar ruta por nombre\n");
}

static void test_lpm_y_prioridad(void) {
  route_table_t *table = route_table_create();
  route_match_t match;
  char trace[512];

  assert(table != NULL);
  assert(route_set(table, "general", 0x0A000000U, 8, 100) == ROUTE_STATUS_OK);
  assert(route_set(table, "region", 0x0A080000U, 16, 1) == ROUTE_STATUS_OK);
  assert(route_set(table, "servidores", 0x0A080200U, 24, 1) == ROUTE_STATUS_OK);
  assert(route_lookup(table, 0x0A080205U, &match) == ROUTE_STATUS_OK);
  assert(strcmp(match.name, "servidores") == 0);

  assert(route_set(table, "region-prioritaria", 0x0A080000U, 16, 10) == ROUTE_STATUS_OK);
  assert(route_lookup_debug(table, 0x0A080205U, &match, trace, sizeof(trace)) == ROUTE_STATUS_OK);
  assert(strcmp(match.name, "servidores") == 0);
  assert(strstr(trace, "Seleccionada servidores") != NULL);

  assert(route_lookup(table, 0xAC100001U, &match) == ROUTE_STATUS_NO_MATCH);
  route_table_destroy(table);
  printf("[OK] Longest Prefix Match y prioridad\n");
}

static void test_subredes_aleatorias(void) {
  route_subnet_t subnets[8];
  uint32_t parent_mask = 0xFF000000U;

  assert(route_generate_random_subnets(0x0A000000U, 8, 24, 8, subnets) == ROUTE_STATUS_OK);
  for (size_t i = 0; i < 8; i++) {
    assert((subnets[i].network & parent_mask) == 0x0A000000U);
    assert(subnets[i].prefix == 24);
    for (size_t j = 0; j < i; j++) {
      assert(subnets[i].network != subnets[j].network);
    }
  }
  assert(route_generate_random_subnets(0x0A000000U, 24, 16, 1, subnets) == ROUTE_STATUS_SUBNET_TOO_LARGE);
  assert(route_generate_random_subnets(0x0A000000U, 30, 32, 5, subnets) == ROUTE_STATUS_INSUFFICIENT_SPACE);
  assert(route_generate_random_subnets(0x0A000000U, 24, 24, 0, subnets) == ROUTE_STATUS_INVALID_COUNT);
  printf("[OK] Subredes aleatorias dentro del padre y sin solapamiento\n");
}

static void test_validaciones_de_route_set(void) {
  route_table_t *table = route_table_create();
  char long_name[ROUTE_NAME_MAX + 1];
  memset(long_name, 'x', sizeof(long_name) - 1);
  long_name[sizeof(long_name) - 1] = '\0';

  assert(table != NULL);
  assert(route_set(table, NULL, 0U, 0, 0) == ROUTE_STATUS_INVALID_ARGUMENT);
  assert(route_set(table, "", 0U, 0, 0) == ROUTE_STATUS_INVALID_ARGUMENT);
  assert(route_set(table, long_name, 0U, 0, 0) == ROUTE_STATUS_NAME_TOO_LONG);
  assert(route_set(table, "invalida", 0U, -1, 0) == ROUTE_STATUS_INVALID_PREFIX);
  assert(route_set(table, "invalida", 0U, 33, 0) == ROUTE_STATUS_INVALID_PREFIX);
  assert(route_set(table, "valida", 0U, 0, 0) == ROUTE_STATUS_OK);
  assert(route_set(NULL, "valida", 0U, 0, 0) == ROUTE_STATUS_INVALID_ARGUMENT);

  route_table_destroy(table);
  printf("[OK] Validaciones de route_set\n");
}

static void test_eliminar_ruta(void) {
  route_table_t *table = route_table_create();

  assert(table != NULL);
  assert(route_set(table, "casa", 0xC0A80100U, 24, 1) == ROUTE_STATUS_OK);
  assert(route_delete(table, "casa") == ROUTE_STATUS_OK);
  assert(route_delete(table, "casa") == ROUTE_STATUS_NOT_FOUND);
  assert(route_delete(table, "inexistente") == ROUTE_STATUS_NOT_FOUND);
  assert(route_delete(table, NULL) == ROUTE_STATUS_INVALID_ARGUMENT);

  route_table_destroy(table);
  printf("[OK] Eliminar y liberar ruta\n");
}

int main(void) {
  test_crear_y_destruir_tabla();
  test_insertar_y_actualizar_ruta();
  test_lpm_y_prioridad();
  test_subredes_aleatorias();
  test_validaciones_de_route_set();
  test_eliminar_ruta();
  return 0;
}