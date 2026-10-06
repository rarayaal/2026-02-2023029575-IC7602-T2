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

  assert(parse_line("GET RANDOM SUBNETS NETWORK NUMBER 10.0.0.0 MASK /8 NUMBER 3 SIZE /24", &cmd) == true);
  assert(cmd.has_subnet_mask == true);
  assert(strcmp(cmd.subnet_mask, "/24") == 0);
  assert(cmd.size == 3);

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

static void test_t09_ipv4_validation_and_conversion(void) {
  uint32_t val = 0;

  /* 1. Conversiones validas y verificacion de uint32_t exacto */
  assert(convertir_ip_a_entero("0.0.0.0", &val) == true && val == 0U);
  assert(ip_a_entero("0.0.0.0") == 0U);

  assert(convertir_ip_a_entero("0.0.0.1", &val) == true && val == 1U);
  assert(ip_a_entero("0.0.0.1") == 1U);

  assert(convertir_ip_a_entero("0.0.1.0", &val) == true && val == 256U);
  assert(ip_a_entero("0.0.1.0") == 256U);

  assert(convertir_ip_a_entero("1.0.0.0", &val) == true && val == 16777216U);
  assert(ip_a_entero("1.0.0.0") == 16777216U);

  assert(convertir_ip_a_entero("10.0.0.1", &val) == true && val == 167772161U);
  assert(convertir_ip_a_entero("127.0.0.1", &val) == true && val == 2130706433U);
  assert(convertir_ip_a_entero("172.16.0.1", &val) == true && val == 2886729729U);

  assert(convertir_ip_a_entero("192.168.1.1", &val) == true && val == 3232235777U);
  assert(ip_a_entero("192.168.1.1") == 3232235777U);

  assert(convertir_ip_a_entero("255.255.255.254", &val) == true && val == 4294967294U);
  assert(convertir_ip_a_entero("255.255.255.255", &val) == true && val == 4294967295U);
  assert(ip_a_entero("255.255.255.255") == 4294967295U);

  /* Ceros a la izquierda */
  assert(convertir_ip_a_entero("001.002.003.004", &val) == true && val == 16909060U);

  /* 2. Casos invalidos por rango */
  assert(validar_ip("256.0.0.1") == false);
  assert(validar_ip("192.256.1.1") == false);
  assert(validar_ip("192.168.256.1") == false);
  assert(validar_ip("192.168.1.256") == false);
  assert(validar_ip("999.999.999.999") == false);
  assert(validar_ip("-1.0.0.0") == false);
  assert(convertir_ip_a_entero("256.0.0.1", &val) == false && val == 0U);

  /* 3. Casos invalidos por caracteres no numericos */
  assert(validar_ip("abc.168.1.1") == false);
  assert(validar_ip("192.abc.1.1") == false);
  assert(validar_ip("192.168.abc.1") == false);
  assert(validar_ip("192.168.1.abc") == false);
  assert(validar_ip("192.168.a.1") == false);
  assert(validar_ip("192.168.1x.1") == false);
  assert(validar_ip("192_168_1_1") == false);
  assert(validar_ip("192-168-1-1") == false);
  assert(validar_ip("192.168.1.1abc") == false);
  assert(validar_ip("abc192.168.1.1") == false);
  assert(convertir_ip_a_entero("192.168.1x.1", &val) == false);

  /* 4. Casos invalidos por formato malformado o espacios */
  assert(validar_ip("192.168.1") == false);
  assert(validar_ip("192.168") == false);
  assert(validar_ip("192") == false);
  assert(validar_ip("192.168.1.1.1") == false);
  assert(validar_ip("192..168.1.1") == false);
  assert(validar_ip(".192.168.1.1") == false);
  assert(validar_ip("192.168.1.1.") == false);
  assert(validar_ip("192.168..1") == false);
  assert(validar_ip("..192.168.1") == false);
  assert(validar_ip("") == false);
  assert(validar_ip(" ") == false);
  assert(validar_ip("   ") == false);
  assert(validar_ip(".") == false);
  assert(validar_ip("...") == false);
  assert(validar_ip(" 192.168.1.1") == false);
  assert(validar_ip("192.168.1.1 ") == false);

  printf("[OK] T09 - IPv4 validation and conversion\n");
}

static void test_t10_netmask_validation_and_conversion(void) {
  /* 1. Validacion de prefijo CIDR (0 <= prefijo <= 32) */
  assert(validar_prefijo_cidr(0) == true);
  assert(validar_prefijo_cidr(8) == true);
  assert(validar_prefijo_cidr(16) == true);
  assert(validar_prefijo_cidr(24) == true);
  assert(validar_prefijo_cidr(32) == true);
  assert(validar_prefijo_cidr(-1) == false);
  assert(validar_prefijo_cidr(33) == false);
  assert(validar_prefijo_cidr(100) == false);

  /* 2. Conversion CIDR -> mascara binaria uint32_t */
  uint32_t mask_val = 0;
  assert(convertir_cidr_a_mascara(0, &mask_val) == true && mask_val == 0x00000000U);
  assert(convertir_cidr_a_mascara(1, &mask_val) == true && mask_val == 0x80000000U);
  assert(convertir_cidr_a_mascara(8, &mask_val) == true && mask_val == 0xFF000000U);
  assert(convertir_cidr_a_mascara(16, &mask_val) == true && mask_val == 0xFFFF0000U);
  assert(convertir_cidr_a_mascara(24, &mask_val) == true && mask_val == 0xFFFFFF00U);
  assert(convertir_cidr_a_mascara(30, &mask_val) == true && mask_val == 0xFFFFFFFCU);
  assert(convertir_cidr_a_mascara(31, &mask_val) == true && mask_val == 0xFFFFFFFEU);
  assert(convertir_cidr_a_mascara(32, &mask_val) == true && mask_val == 0xFFFFFFFFU);
  assert(convertir_cidr_a_mascara(33, &mask_val) == false);
  assert(convertir_cidr_a_mascara(-1, &mask_val) == false);

  /* 3. Conversion CIDR -> cadena decimal */
  char str_buf[64];
  assert(convertir_cidr_a_cadena_decimal(0, str_buf, sizeof(str_buf)) == true && strcmp(str_buf, "0.0.0.0") == 0);
  assert(convertir_cidr_a_cadena_decimal(8, str_buf, sizeof(str_buf)) == true && strcmp(str_buf, "255.0.0.0") == 0);
  assert(convertir_cidr_a_cadena_decimal(16, str_buf, sizeof(str_buf)) == true && strcmp(str_buf, "255.255.0.0") == 0);
  assert(convertir_cidr_a_cadena_decimal(24, str_buf, sizeof(str_buf)) == true && strcmp(str_buf, "255.255.255.0") == 0);
  assert(convertir_cidr_a_cadena_decimal(25, str_buf, sizeof(str_buf)) == true && strcmp(str_buf, "255.255.255.128") == 0);
  assert(convertir_cidr_a_cadena_decimal(30, str_buf, sizeof(str_buf)) == true && strcmp(str_buf, "255.255.255.252") == 0);
  assert(convertir_cidr_a_cadena_decimal(32, str_buf, sizeof(str_buf)) == true && strcmp(str_buf, "255.255.255.255") == 0);

  /* 4. Conversion mascara binaria uint32_t -> CIDR */
  int prefijo_out = -1;
  assert(convertir_mascara_a_cidr(0x00000000U, &prefijo_out) == true && prefijo_out == 0);
  assert(convertir_mascara_a_cidr(0x80000000U, &prefijo_out) == true && prefijo_out == 1);
  assert(convertir_mascara_a_cidr(0xFF000000U, &prefijo_out) == true && prefijo_out == 8);
  assert(convertir_mascara_a_cidr(0xFFFF0000U, &prefijo_out) == true && prefijo_out == 16);
  assert(convertir_mascara_a_cidr(0xFFFFFF00U, &prefijo_out) == true && prefijo_out == 24);
  assert(convertir_mascara_a_cidr(0xFFFFFFFFU, &prefijo_out) == true && prefijo_out == 32);

  /* Mascaras no contiguas rechazadas */
  assert(convertir_mascara_a_cidr(0xFF00FF00U, &prefijo_out) == false && prefijo_out == -1);
  assert(convertir_mascara_a_cidr(0x0000FFFFU, &prefijo_out) == false && prefijo_out == -1);

  /* 5. Prueba de Reversibilidad (0..32): CIDR -> mascara -> CIDR y mascara -> CIDR -> mascara */
  for (int p = 0; p <= 32; p++) {
    uint32_t m = 0;
    int p_back = -1;
    assert(convertir_cidr_a_mascara(p, &m) == true);
    assert(convertir_mascara_a_cidr(m, &p_back) == true);
    assert(p == p_back);

    uint32_t m_back = 0;
    assert(convertir_cidr_a_mascara(p_back, &m_back) == true);
    assert(m == m_back);
  }

  /* 6. Parsing de cadenas de mascara (CIDR o Decimal) */
  assert(parsear_mascara_a_cidr("/24", &prefijo_out) == true && prefijo_out == 24);
  assert(parsear_mascara_a_cidr("24", &prefijo_out) == true && prefijo_out == 24);
  assert(parsear_mascara_a_cidr("/0", &prefijo_out) == true && prefijo_out == 0);
  assert(parsear_mascara_a_cidr("/32", &prefijo_out) == true && prefijo_out == 32);
  assert(parsear_mascara_a_cidr("255.255.255.0", &prefijo_out) == true && prefijo_out == 24);
  assert(parsear_mascara_a_cidr("255.255.0.0", &prefijo_out) == true && prefijo_out == 16);
  assert(parsear_mascara_a_cidr("255.0.0.0", &prefijo_out) == true && prefijo_out == 8);
  assert(parsear_mascara_a_cidr("0.0.0.0", &prefijo_out) == true && prefijo_out == 0);
  assert(parsear_mascara_a_cidr("255.255.255.255", &prefijo_out) == true && prefijo_out == 32);

  /* Casos invalidos de parseo de mascara */
  assert(parsear_mascara_a_cidr("/33", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("/-1", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("/24abc", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("24abc", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255.0.255.0", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255.255.0.255", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255.0.255.255", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255.255.128.255", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("192.168.0.0", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("10.0.0.0", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("256.0.0.0", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255.256.0.0", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255.255.a.0", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255.255.255.x", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("abc", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr(".", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255.255", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255.255.255", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255.255.255.255.0", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255..255.0", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr(".255.255.0", &prefijo_out) == false);
  assert(parsear_mascara_a_cidr("255.255.255.", &prefijo_out) == false);

  printf("[OK] T10 - Netmask validation and conversion\n");
}

int main(void) {
  printf("=== INICIANDO PRUEBAS UNITARIAS DIRECTAS DE PARSER T-08, T-09 Y T-10 ===\n");
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
  test_t09_ipv4_validation_and_conversion();
  test_t10_netmask_validation_and_conversion();
  printf("=== TODAS LAS PRUEBAS DIRECTAS (T-08, T-09 Y T-10) PASARON CORRECTAMENTE ===\n");
  return 0;
}
