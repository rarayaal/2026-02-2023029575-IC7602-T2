#include "param_extractor.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_t11_primitiva_1_get_broadcast(void) {
  parsed_command_t cmd;
  extracted_params_t params;
  char ip[64], mask[64];
  uint32_t ip_u, mask_u;
  int cidr;

  assert(parse_line("GET BROADCAST IP 192.168.1.10 MASK /24", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.primitive == PRIMITIVE_GET_BROADCAST);
  assert(params.has_ip == true && strcmp(params.ip_str, "192.168.1.10") == 0);
  assert(params.ip_uint == 3232235786U);
  assert(params.has_mask == true && strcmp(params.mask_str, "/24") == 0);
  assert(params.mask_cidr == 24);
  assert(params.mask_uint == 0xFFFFFF00U);

  assert(extraer_param_ip(&cmd, ip, sizeof(ip)) == true && strcmp(ip, "192.168.1.10") == 0);
  assert(extraer_param_ip_uint32(&cmd, &ip_u) == true && ip_u == 3232235786U);
  assert(extraer_param_mascara(&cmd, mask, sizeof(mask)) == true && strcmp(mask, "/24") == 0);
  assert(extraer_param_mascara_cidr(&cmd, &cidr) == true && cidr == 24);
  assert(extraer_param_mascara_uint32(&cmd, &mask_u) == true && mask_u == 0xFFFFFF00U);

  printf("[OK] T11 Primitiva 1: GET BROADCAST extrae IP y MASK\n");
}

static void test_t11_primitiva_2_get_network_number(void) {
  parsed_command_t cmd;
  extracted_params_t params;
  uint32_t ip_u, mask_u;
  int cidr;

  assert(parse_line("GET NETWORK NUMBER IP 10.0.0.50 MASK 255.255.255.0", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.primitive == PRIMITIVE_GET_NETWORK_NUMBER);
  assert(params.has_ip == true && strcmp(params.ip_str, "10.0.0.50") == 0);
  assert(params.ip_uint == 167772210U);
  assert(params.has_mask == true && strcmp(params.mask_str, "255.255.255.0") == 0);
  assert(params.mask_cidr == 24);
  assert(params.mask_uint == 0xFFFFFF00U);

  assert(extraer_param_ip_uint32(&cmd, &ip_u) == true && ip_u == 167772210U);
  assert(extraer_param_mascara_cidr(&cmd, &cidr) == true && cidr == 24);
  assert(extraer_param_mascara_uint32(&cmd, &mask_u) == true && mask_u == 0xFFFFFF00U);

  printf("[OK] T11 Primitiva 2: GET NETWORK NUMBER extrae IP y MASK\n");
}

static void test_t11_primitiva_3_get_hosts_range(void) {
  parsed_command_t cmd;
  extracted_params_t params;

  assert(parse_line("GET HOSTS RANGE IP 172.16.0.1 MASK /16", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.primitive == PRIMITIVE_GET_HOSTS_RANGE);
  assert(params.has_ip == true && strcmp(params.ip_str, "172.16.0.1") == 0);
  assert(params.ip_uint == 2886729729U);
  assert(params.has_mask == true && params.mask_cidr == 16);
  assert(params.mask_uint == 0xFFFF0000U);

  printf("[OK] T11 Primitiva 3: GET HOSTS RANGE extrae IP y MASK\n");
}

static void test_t11_primitiva_4_get_netmask(void) {
  parsed_command_t cmd;
  extracted_params_t params;

  assert(parse_line("GET NETMASK MASK /28", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.primitive == PRIMITIVE_GET_NETMASK);
  assert(params.has_ip == false);
  assert(params.has_mask == true && params.mask_cidr == 28);
  assert(params.mask_uint == 0xFFFFFFF0U);

  printf("[OK] T11 Primitiva 4: GET NETMASK extrae MASK\n");
}

static void test_t11_primitiva_5_get_hosts_count(void) {
  parsed_command_t cmd;
  extracted_params_t params;

  assert(parse_line("GET HOSTS COUNT MASK 255.255.255.240", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.primitive == PRIMITIVE_GET_HOSTS_COUNT);
  assert(params.has_mask == true && params.mask_cidr == 28);
  assert(params.mask_uint == 0xFFFFFFF0U);

  printf("[OK] T11 Primitiva 5: GET HOSTS COUNT extrae MASK\n");
}

static void test_t11_primitiva_6_get_ip_class(void) {
  parsed_command_t cmd;
  extracted_params_t params;

  assert(parse_line("GET IP CLASS 192.168.1.1", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.primitive == PRIMITIVE_GET_IP_CLASS);
  assert(params.has_ip == true && strcmp(params.ip_str, "192.168.1.1") == 0);
  assert(params.ip_uint == 3232235777U);
  assert(params.has_mask == false);

  printf("[OK] T11 Primitiva 6: GET IP CLASS extrae IP\n");
}

static void test_t11_primitiva_7_set_route(void) {
  parsed_command_t cmd;
  extracted_params_t params;
  char name[64], dest[64];
  uint32_t dest_u;
  int prio;

  /* Con prioridad obligatoria */
  assert(parse_line("SET ROUTE casa NETWORK NUMBER 192.168.1.0 MASK /24 PRIORITY 1", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.primitive == PRIMITIVE_SET_ROUTE);
  assert(params.has_route_name == true && strcmp(params.route_name, "casa") == 0);
  assert(params.has_dest_net == true && strcmp(params.dest_net, "192.168.1.0") == 0);
  assert(params.dest_net_uint == 3232235776U);
  assert(params.has_mask == true && params.mask_cidr == 24);
  assert(params.has_priority == true && params.priority == 1);

  /* Con prioridad */
  assert(parse_line("SET ROUTE oficina NETWORK NUMBER 10.0.0.0 MASK /8 PRIORITY 15", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.has_route_name == true && strcmp(params.route_name, "oficina") == 0);
  assert(params.has_dest_net == true && strcmp(params.dest_net, "10.0.0.0") == 0);
  assert(params.dest_net_uint == 167772160U);
  assert(params.has_priority == true && params.priority == 15);

  assert(extraer_param_nombre_ruta(&cmd, name, sizeof(name)) == true && strcmp(name, "oficina") == 0);
  assert(extraer_param_destino_ruta(&cmd, dest, sizeof(dest)) == true && strcmp(dest, "10.0.0.0") == 0);
  assert(extraer_param_destino_ruta_uint32(&cmd, &dest_u) == true && dest_u == 167772160U);
  assert(extraer_param_prioridad(&cmd, &prio) == true && prio == 15);

  printf("[OK] T11 Primitiva 7: SET ROUTE extrae NAME, DEST, MASK y PRIORITY\n");
}

static void test_t11_primitiva_8_del_route(void) {
  parsed_command_t cmd;
  extracted_params_t params;
  char name[64];

  assert(parse_line("DEL ROUTE servidor_principal", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.primitive == PRIMITIVE_DEL_ROUTE);
  assert(params.has_route_name == true && strcmp(params.route_name, "servidor_principal") == 0);

  assert(extraer_param_nombre_ruta(&cmd, name, sizeof(name)) == true && strcmp(name, "servidor_principal") == 0);

  printf("[OK] T11 Primitiva 8: DEL ROUTE extrae NAME\n");
}

static void test_t11_primitiva_9_get_random_subnets(void) {
  parsed_command_t cmd;
  extracted_params_t params;
  int sz;

  assert(parse_line("GET RANDOM SUBNETS IP 10.0.0.0 MASK /16 SIZE 8", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.primitive == PRIMITIVE_GET_RANDOM_SUBNETS);
  assert(params.has_ip == true && strcmp(params.ip_str, "10.0.0.0") == 0);
  assert(params.has_mask == true && params.mask_cidr == 16);
  assert(params.has_size == true && params.size == 8);

  assert(extraer_param_size(&cmd, &sz) == true && sz == 8);

  printf("[OK] T11 Primitiva 9: GET RANDOM SUBNETS extrae IP, MASK y SIZE\n");
}

static void test_t11_primitiva_10_route_ip(void) {
  parsed_command_t cmd;
  extracted_params_t params;
  bool dbg;

  /* Sin DEBUG */
  assert(parse_line("ROUTE IP 8.8.8.8", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.primitive == PRIMITIVE_ROUTE_IP);
  assert(params.has_ip == true && strcmp(params.ip_str, "8.8.8.8") == 0);
  assert(params.ip_uint == 134744072U);
  assert(params.debug == false);

  /* Con DEBUG */
  assert(parse_line("ROUTE IP 8.8.8.8 DEBUG true", &cmd) == true);
  assert(extraer_parametros_comando(&cmd, &params) == true);

  assert(params.debug == true);
  assert(extraer_param_debug(&cmd, &dbg) == true && dbg == true);

  printf("[OK] T11 Primitiva 10: ROUTE IP extrae IP y DEBUG\n");
}

static void test_t11_comando_invalidos(void) {
  parsed_command_t cmd;
  extracted_params_t params;
  char buf[64];
  uint32_t val_u;
  int val_i;

  assert(parse_line("INVALID COMMAND", &cmd) == false);
  assert(extraer_parametros_comando(&cmd, &params) == false);

  assert(extraer_param_ip(&cmd, buf, sizeof(buf)) == false);
  assert(extraer_param_ip_uint32(&cmd, &val_u) == false && val_u == 0U);
  assert(extraer_param_mascara(&cmd, buf, sizeof(buf)) == false);
  assert(extraer_param_mascara_cidr(&cmd, &val_i) == false && val_i == -1);
  assert(extraer_param_prioridad(&cmd, &val_i) == false && val_i == 0);
  assert(extraer_param_size(&cmd, &val_i) == false && val_i == 0);

  printf("[OK] T11 Manejo de comandos invalidos\n");
}

int main(void) {
  printf("=== INICIANDO PRUEBAS UNITARIAS DE EXTRACCIÓN DE PARÁMETROS T-11 ===\n");
  test_t11_primitiva_1_get_broadcast();
  test_t11_primitiva_2_get_network_number();
  test_t11_primitiva_3_get_hosts_range();
  test_t11_primitiva_4_get_netmask();
  test_t11_primitiva_5_get_hosts_count();
  test_t11_primitiva_6_get_ip_class();
  test_t11_primitiva_7_set_route();
  test_t11_primitiva_8_del_route();
  test_t11_primitiva_9_get_random_subnets();
  test_t11_primitiva_10_route_ip();
  test_t11_comando_invalidos();
  printf("=== TODAS LAS PRUEBAS DE T-11 PASARON CORRECTAMENTE ===\n");
  return 0;
}
