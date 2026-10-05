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
  assert(table != NULL);

  assert(route_set(table, "oficina", 0x0A080205U, 29, 3) == ROUTE_STATUS_OK);
  assert(route_set(table, "oficina", 0xC0A80199U, 24, 8) == ROUTE_STATUS_OK);

  route_table_destroy(table);
  printf("[OK] Insertar y actualizar ruta por nombre\n");
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
  test_validaciones_de_route_set();
  test_eliminar_ruta();
  return 0;
}