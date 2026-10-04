#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_primitiva_1_get_broadcast(void) {
  parsed_command_t cmd;

  /* Valida CIDR */
  assert(parse_line("GET BROADCAST IP 192.168.1.10 MASK /24", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(cmd.primitive == PRIMITIVE_GET_BROADCAST);
  assert(strcmp(cmd.ip, "192.168.1.10") == 0);
  assert(strcmp(cmd.mask, "/24") == 0);

  /* Valida Decimal */
  assert(parse_line("GET BROADCAST IP 10.0.0.5 MASK 255.255.255.0", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(cmd.primitive == PRIMITIVE_GET_BROADCAST);
  assert(strcmp(cmd.ip, "10.0.0.5") == 0);
  assert(strcmp(cmd.mask, "255.255.255.0") == 0);

  /* IP invalida */
  assert(parse_line("GET BROADCAST IP 999.168.1.10 MASK /24", &cmd) == false);
  assert(cmd.is_valid == false);
  assert(strstr(cmd.error_msg, "ERROR") != NULL);

  /* Mascara invalida */
  assert(parse_line("GET BROADCAST IP 192.168.1.10 MASK /33", &cmd) == false);
  assert(cmd.is_valid == false);

  /* Parametro faltante */
  assert(parse_line("GET BROADCAST IP 192.168.1.10", &cmd) == false);
  assert(cmd.is_valid == false);

  /* Parametros adicionales */
  assert(parse_line("GET BROADCAST IP 192.168.1.10 MASK /24 EXTRA", &cmd) == false);
  assert(cmd.is_valid == false);

  printf("[OK] Primitiva 1: GET BROADCAST\n");
}

static void test_primitiva_2_get_network_number(void) {
  parsed_command_t cmd;

  assert(parse_line("GET NETWORK NUMBER IP 192.168.1.50 MASK /24", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(cmd.primitive == PRIMITIVE_GET_NETWORK_NUMBER);
  assert(strcmp(cmd.ip, "192.168.1.50") == 0);
  assert(strcmp(cmd.mask, "/24") == 0);

  /* Error sintaxis */
  assert(parse_line("GET NETWORK NUMBER IP 192.168.1.50 MASK", &cmd) == false);
  assert(cmd.is_valid == false);

  printf("[OK] Primitiva 2: GET NETWORK NUMBER\n");
}

static void test_primitiva_3_get_hosts_range(void) {
  parsed_command_t cmd;

  assert(parse_line("GET HOSTS RANGE IP 192.168.1.10 MASK /24", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(cmd.primitive == PRIMITIVE_GET_HOSTS_RANGE);
  assert(strcmp(cmd.ip, "192.168.1.10") == 0);
  assert(strcmp(cmd.mask, "/24") == 0);

  /* Faltante IP */
  assert(parse_line("GET HOSTS RANGE MASK /24", &cmd) == false);
  assert(cmd.is_valid == false);

  printf("[OK] Primitiva 3: GET HOSTS RANGE\n");
}

static void test_primitiva_4_get_netmask(void) {
  parsed_command_t cmd;

  assert(parse_line("GET NETMASK MASK /24", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(cmd.primitive == PRIMITIVE_GET_NETMASK);
  assert(strcmp(cmd.mask, "/24") == 0);

  assert(parse_line("GET NETMASK MASK 255.255.255.0", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(strcmp(cmd.mask, "255.255.255.0") == 0);

  assert(parse_line("GET NETMASK /16", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(strcmp(cmd.mask, "/16") == 0);

  /* Mascara invalida */
  assert(parse_line("GET NETMASK MASK /99", &cmd) == false);
  assert(cmd.is_valid == false);

  printf("[OK] Primitiva 4: GET NETMASK\n");
}

static void test_primitiva_5_get_hosts_count(void) {
  parsed_command_t cmd;

  assert(parse_line("GET HOSTS COUNT MASK /24", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(cmd.primitive == PRIMITIVE_GET_HOSTS_COUNT);
  assert(strcmp(cmd.mask, "/24") == 0);

  assert(parse_line("GET HOSTS COUNT /29", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(strcmp(cmd.mask, "/29") == 0);

  /* Mascara invalida */
  assert(parse_line("GET HOSTS COUNT MASK /35", &cmd) == false);
  assert(cmd.is_valid == false);

  printf("[OK] Primitiva 5: GET HOSTS COUNT\n");
}

static void test_primitiva_6_get_ip_class(void) {
  parsed_command_t cmd;

  assert(parse_line("GET IP CLASS 10.8.2.5", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(cmd.primitive == PRIMITIVE_GET_IP_CLASS);
  assert(strcmp(cmd.ip, "10.8.2.5") == 0);

  assert(parse_line("GET IP CLASS IP 172.16.0.1", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(strcmp(cmd.ip, "172.16.0.1") == 0);

  /* IP invalida */
  assert(parse_line("GET IP CLASS 300.0.0.1", &cmd) == false);
  assert(cmd.is_valid == false);

  printf("[OK] Primitiva 6: GET IP CLASS\n");
}

static void test_primitiva_7_set_route(void) {
  parsed_command_t cmd;

  assert(parse_line("SET ROUTE casa DEST 192.168.1.0 MASK /24", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(cmd.primitive == PRIMITIVE_SET_ROUTE);
  assert(strcmp(cmd.name, "casa") == 0);
  assert(strcmp(cmd.dest, "192.168.1.0") == 0);
  assert(strcmp(cmd.mask, "/24") == 0);
  assert(cmd.has_priority == false);

  assert(parse_line("SET ROUTE oficina DEST 10.0.0.0 MASK 255.0.0.0 PRIORITY 10", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(strcmp(cmd.name, "oficina") == 0);
  assert(strcmp(cmd.dest, "10.0.0.0") == 0);
  assert(strcmp(cmd.mask, "255.0.0.0") == 0);
  assert(cmd.has_priority == true);
  assert(cmd.priority == 10);

  /* Prioridad no numerica */
  assert(parse_line("SET ROUTE oficina DEST 10.0.0.0 MASK /8 PRIORITY diez", &cmd) == false);

  /* Parametro faltante */
  assert(parse_line("SET ROUTE casa DEST 192.168.1.0", &cmd) == false);

  printf("[OK] Primitiva 7: SET ROUTE\n");
}

static void test_primitiva_8_del_route(void) {
  parsed_command_t cmd;

  assert(parse_line("DEL ROUTE casa", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(cmd.primitive == PRIMITIVE_DEL_ROUTE);
  assert(strcmp(cmd.name, "casa") == 0);

  /* Nombre faltante */
  assert(parse_line("DEL ROUTE", &cmd) == false);

  /* Parametros extras */
  assert(parse_line("DEL ROUTE casa extra", &cmd) == false);

  printf("[OK] Primitiva 8: DEL ROUTE\n");
}

static void test_primitiva_9_get_random_subnets(void) {
  parsed_command_t cmd;

  assert(parse_line("GET RANDOM SUBNETS IP 10.0.0.0 MASK /16 SIZE 4", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(cmd.primitive == PRIMITIVE_GET_RANDOM_SUBNETS);
  assert(strcmp(cmd.ip, "10.0.0.0") == 0);
  assert(strcmp(cmd.mask, "/16") == 0);
  assert(cmd.has_size == true);
  assert(cmd.size == 4);

  /* SIZE <= 0 */
  assert(parse_line("GET RANDOM SUBNETS IP 10.0.0.0 MASK /16 SIZE 0", &cmd) == false);

  /* SIZE no numerico */
  assert(parse_line("GET RANDOM SUBNETS IP 10.0.0.0 MASK /16 SIZE cuatro", &cmd) == false);

  printf("[OK] Primitiva 9: GET RANDOM SUBNETS\n");
}

static void test_primitiva_10_route_ip(void) {
  parsed_command_t cmd;

  assert(parse_line("ROUTE IP 8.8.8.8", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(cmd.primitive == PRIMITIVE_ROUTE_IP);
  assert(strcmp(cmd.ip, "8.8.8.8") == 0);
  assert(cmd.debug == false);

  assert(parse_line("ROUTE IP 8.8.8.8 DEBUG", &cmd) == true);
  assert(cmd.is_valid == true);
  assert(strcmp(cmd.ip, "8.8.8.8") == 0);
  assert(cmd.debug == true);

  /* IP invalida */
  assert(parse_line("ROUTE IP abc.def.ghi.jkl", &cmd) == false);

  printf("[OK] Primitiva 10: ROUTE IP\n");
}

static void test_casos_invalidos_y_desconocidos(void) {
  parsed_command_t cmd;

  /* Primitiva desconocida */
  assert(parse_line("INVALID COMMAND HERE", &cmd) == false);
  assert(cmd.primitive == PRIMITIVE_UNKNOWN);
  assert(strstr(cmd.error_msg, "ERROR") != NULL);

  /* Linea vacia */
  assert(parse_line("", &cmd) == false);

  /* Espacios multiples y tabulaciones */
  assert(parse_line("   GET \t  NETMASK   \t MASK   /24   ", &cmd) == true);
  assert(cmd.primitive == PRIMITIVE_GET_NETMASK);
  assert(strcmp(cmd.mask, "/24") == 0);

  /* Mascara decimal no contigua */
  assert(parse_line("GET NETMASK MASK 255.0.255.0", &cmd) == false);

  printf("[OK] Casos invalidos y desconocidos\n");
}

int main(void) {
  printf("=== INICIANDO PRUEBAS UNITARIAS DIRECTAS DE PARSER T-08 ===\n");
  test_primitiva_1_get_broadcast();
  test_primitiva_2_get_network_number();
  test_primitiva_3_get_hosts_range();
  test_primitiva_4_get_netmask();
  test_primitiva_5_get_hosts_count();
  test_primitiva_6_get_ip_class();
  test_primitiva_7_set_route();
  test_primitiva_8_del_route();
  test_primitiva_9_get_random_subnets();
  test_primitiva_10_route_ip();
  test_casos_invalidos_y_desconocidos();
  printf("=== TODAS LAS PRUEBAS DIRECTAS DE PARSER PASARON CORRECTAMENTE ===\n");
  return 0;
}
