#include "error_catalog.h"
#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_error_catalog_direct_formatting(void) {
  char msg[256];

  /* 1. Entrada nula y linea vacia */
  err_cat_null_input(msg, sizeof(msg));
  assert(strcmp(msg, "ERROR: entrada nula") == 0);

  err_cat_empty_line(msg, sizeof(msg));
  assert(strcmp(msg, "ERROR: linea vacia") == 0);

  /* 2. Primitiva desconocida */
  err_cat_unknown_primitive(msg, sizeof(msg), "FOOBAR");
  assert(strcmp(msg, "ERROR: primitiva desconocida: FOOBAR") == 0);

  /* 3. Sintaxis invalida */
  err_cat_invalid_syntax(msg, sizeof(msg), "comando GET");
  assert(strcmp(msg, "ERROR: sintaxis invalida en comando GET") == 0);

  /* 4. Parametros faltantes (singular y plural) */
  err_cat_missing_param(msg, sizeof(msg), "GET HOSTS COUNT", "MASK");
  assert(strcmp(msg, "ERROR: parametro MASK faltante en GET HOSTS COUNT") == 0);

  err_cat_missing_param(msg, sizeof(msg), "GET BROADCAST", "requiere IP <IP> MASK <MASK>");
  assert(strcmp(msg, "ERROR: parametro faltante en GET BROADCAST (requiere IP <IP> MASK <MASK>)") == 0);

  err_cat_missing_param(msg, sizeof(msg), "GET RANDOM SUBNETS", "requiere IP <IP> MASK <MASK> SIZE <NUM>");
  assert(strcmp(msg, "ERROR: parametros faltantes en GET RANDOM SUBNETS (requiere IP <IP> MASK <MASK> SIZE <NUM>)") == 0);

  /* 5. Parametros adicionales */
  err_cat_extra_params(msg, sizeof(msg), "GET NETMASK");
  assert(strcmp(msg, "ERROR: parametros adicionales no permitidos en GET NETMASK") == 0);

  /* 6. IP e IP invalida */
  err_cat_invalid_ip(msg, sizeof(msg), "999.1.1.1");
  assert(strcmp(msg, "ERROR: direccion IP invalida: 999.1.1.1") == 0);

  /* 7. Mascara invalida */
  err_cat_invalid_mask(msg, sizeof(msg), "/33");
  assert(strcmp(msg, "ERROR: mascara de red invalida: /33") == 0);

  /* 8. Palabra clave esperada */
  err_cat_expected_keyword(msg, sizeof(msg), "SET ROUTE", "PRIORITY");
  assert(strcmp(msg, "ERROR: parametro invalido en SET ROUTE, se esperaba PRIORITY") == 0);

  /* 9. Valor no numerico */
  err_cat_invalid_numeric(msg, sizeof(msg), "SIZE", "abc");
  assert(strcmp(msg, "ERROR: valor de SIZE debe ser numerico: abc") == 0);

  /* 10. Fuera de rango */
  err_cat_out_of_range(msg, sizeof(msg), "el tamaño debe ser mayor a 0", "0");
  assert(strcmp(msg, "ERROR: el tamaño debe ser mayor a 0: 0") == 0);

  /* 11. Parametro no reconocido */
  err_cat_unrecognized_param(msg, sizeof(msg), "ROUTE IP", "FOO");
  assert(strcmp(msg, "ERROR: parametro no reconocido en ROUTE IP: FOO") == 0);

  /* 12. Verificacion de codigos de error */
  assert(strcmp(error_catalog_get_code_name(ERR_CATALOG_MISSING_PARAM), "ERR_MISSING_PARAM") == 0);
  assert(strcmp(error_catalog_get_code_name(ERR_CATALOG_INVALID_MASK), "ERR_INVALID_MASK") == 0);
  assert(strcmp(error_catalog_get_code_name(ERR_CATALOG_UNKNOWN_PRIMITIVE), "ERR_UNKNOWN_PRIMITIVE") == 0);

  printf("[OK] Pruebas directas de formateo del catalogo de errores T-12 pasadas\n");
}

static void test_error_catalog_integration_with_parser(void) {
  parsed_command_t cmd;

  /* 1. Parametros faltantes */
  assert(parse_line("GET BROADCAST IP 192.168.1.1", &cmd) == false);
  assert(strcmp(cmd.error_msg, "ERROR: parametro faltante en GET BROADCAST (requiere IP <IP> MASK <MASK>)") == 0);

  assert(parse_line("GET NETMASK", &cmd) == false);
  assert(strcmp(cmd.error_msg, "ERROR: parametro MASK faltante en GET NETMASK") == 0);

  /* 2. Mascara invalida */
  assert(parse_line("GET NETMASK MASK /99", &cmd) == false);
  assert(strcmp(cmd.error_msg, "ERROR: mascara de red invalida: /99") == 0);

  assert(parse_line("GET BROADCAST IP 10.0.0.1 MASK 255.0.255.0", &cmd) == false);
  assert(strcmp(cmd.error_msg, "ERROR: mascara de red invalida: 255.0.255.0") == 0);

  /* 3. Sintaxis o primitiva desconocida */
  assert(parse_line("INVALID_COMMAND 123", &cmd) == false);
  assert(strcmp(cmd.error_msg, "ERROR: primitiva desconocida: INVALID_COMMAND") == 0);

  assert(parse_line("GET", &cmd) == false);
  assert(strcmp(cmd.error_msg, "ERROR: sintaxis invalida en comando GET") == 0);

  assert(parse_line("GET INVALID_SUBCOMMAND", &cmd) == false);
  assert(strcmp(cmd.error_msg, "ERROR: primitiva desconocida: GET") == 0);

  printf("[OK] Pruebas de integracion del catalogo de errores con parser T-12 pasadas\n");
}

int main(void) {
  printf("=== INICIANDO PRUEBAS UNITARIAS DE CATÁLOGO DE ERRORES T-12 ===\n");
  test_error_catalog_direct_formatting();
  test_error_catalog_integration_with_parser();
  printf("=== TODAS LAS PRUEBAS DE T-12 PASARON CORRECTAMENTE ===\n");
  return 0;
}
