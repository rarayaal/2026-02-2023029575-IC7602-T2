#include "dispatcher.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_caso_1_despacho_calculo_subredes(void) {
  dispatch_result_t res;

  /* 1. GET BROADCAST */
  assert(dispatch_command_line("GET BROADCAST IP 192.168.1.10 MASK /24", &res) == true);
  assert(res.is_success == true);
  assert(res.primitive == PRIMITIVE_GET_BROADCAST);
  assert(strcmp(res.result_str, "192.168.1.255") == 0);

  /* 2. GET NETWORK NUMBER */
  assert(dispatch_command_line("GET NETWORK NUMBER IP 192.168.1.50 MASK 255.255.255.0", &res) == true);
  assert(res.is_success == true);
  assert(res.primitive == PRIMITIVE_GET_NETWORK_NUMBER);
  assert(strcmp(res.result_str, "192.168.1.0") == 0);

  /* 3. GET HOSTS RANGE */
  assert(dispatch_command_line("GET HOSTS RANGE IP 192.168.1.10 MASK /24", &res) == true);
  assert(res.is_success == true);
  assert(res.primitive == PRIMITIVE_GET_HOSTS_RANGE);
  assert(strcmp(res.result_str, "192.168.1.{1-254}") == 0);

  /* 4. GET NETMASK */
  assert(dispatch_command_line("GET NETMASK MASK /24", &res) == true);
  assert(res.is_success == true);
  assert(res.primitive == PRIMITIVE_GET_NETMASK);
  assert(strcmp(res.result_str, "255.255.255.0") == 0);

  /* 5. GET HOSTS COUNT */
  assert(dispatch_command_line("GET HOSTS COUNT MASK /24", &res) == true);
  assert(res.is_success == true);
  assert(res.primitive == PRIMITIVE_GET_HOSTS_COUNT);
  assert(strcmp(res.result_str, "254") == 0);

  /* 6. GET IP CLASS */
  assert(dispatch_command_line("GET IP CLASS 10.0.0.1", &res) == true);
  assert(res.is_success == true);
  assert(res.primitive == PRIMITIVE_GET_IP_CLASS);
  assert(strcmp(res.result_str, "Clase A") == 0);

  printf("[OK] Caso 1: Despacho a calculo de subredes completado exitosamente\n");
}

static void test_caso_2_despacho_enrutamiento(void) {
  dispatch_result_t res;

  /* 7. SET ROUTE */
  assert(dispatch_command_line("SET ROUTE oficina DEST 10.0.0.0 MASK /8 PRIORITY 10", &res) == true);
  assert(res.is_success == true);
  assert(res.primitive == PRIMITIVE_SET_ROUTE);
  assert(strstr(res.result_str, "oficina") != NULL);
  assert(strstr(res.result_str, "10.0.0.0") != NULL);

  /* 8. DEL ROUTE */
  assert(dispatch_command_line("DEL ROUTE oficina", &res) == true);
  assert(res.is_success == true);
  assert(res.primitive == PRIMITIVE_DEL_ROUTE);
  assert(strstr(res.result_str, "oficina") != NULL);

  /* 9. GET RANDOM SUBNETS */
  assert(dispatch_command_line("GET RANDOM SUBNETS IP 10.0.0.0 MASK /16 SIZE 4", &res) == true);
  assert(res.is_success == true);
  assert(res.primitive == PRIMITIVE_GET_RANDOM_SUBNETS);
  assert(strstr(res.result_str, "4") != NULL);

  /* 10. ROUTE IP */
  assert(dispatch_command_line("ROUTE IP 8.8.8.8 DEBUG", &res) == true);
  assert(res.is_success == true);
  assert(res.primitive == PRIMITIVE_ROUTE_IP);
  assert(strstr(res.result_str, "8.8.8.8") != NULL);

  printf("[OK] Caso 2: Despacho a enrutamiento y opciones completado exitosamente\n");
}

static void test_caso_3_parametros_invalidos(void) {
  extracted_params_t invalid_params;
  dispatch_result_t res;

  memset(&invalid_params, 0, sizeof(invalid_params));
  invalid_params.is_valid = false;

  assert(dispatch_extracted_params(&invalid_params, &res) == false);
  assert(res.is_success == false);
  assert(strstr(res.error_msg, "ERROR") != NULL);

  printf("[OK] Caso 3: Rechazo de parametros invalidos antes de despacho\n");
}

static void test_caso_4_primitiva_no_soportada(void) {
  dispatch_result_t res;

  assert(dispatch_command_line("INVALID COMMAND HERE", &res) == false);
  assert(res.is_success == false);
  assert(strstr(res.error_msg, "ERROR: primitiva desconocida") != NULL);

  printf("[OK] Caso 4: Manejo correcto de primitiva no soportada con catalogo de errores\n");
}

int main(void) {
  printf("=== INICIANDO PRUEBAS UNITARIAS DE DESPACHO A CÁLCULO T-13 ===\n");
  test_caso_1_despacho_calculo_subredes();
  test_caso_2_despacho_enrutamiento();
  test_caso_3_parametros_invalidos();
  test_caso_4_primitiva_no_soportada();
  printf("=== TODAS LAS PRUEBAS DE T-13 PASARON CORRECTAMENTE ===\n");
  return 0;
}
