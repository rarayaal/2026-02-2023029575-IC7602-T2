#include "calculo_subredes.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void probar_broadcast(void) {
  char salida[32];

  uint32_t ip1 = ip_a_entero("10.8.2.5");
  uint32_t mascara1 = cidr_a_mascara(29);
  uint32_t broadcast1 = obtener_ip_broadcast(ip1, mascara1);
  entero_a_ip(broadcast1, salida, sizeof(salida));
  assert(strcmp(salida, "10.8.2.7") == 0);

  uint32_t ip2 = ip_a_entero("172.16.0.56");
  uint32_t mascara2 = ip_a_entero("255.255.255.128");
  uint32_t broadcast2 = obtener_ip_broadcast(ip2, mascara2);
  entero_a_ip(broadcast2, salida, sizeof(salida));
  assert(strcmp(salida, "172.16.0.127") == 0);

  printf("[OK] Prueba Primitiva 1: GET BROADCAST\n");
}

void probar_numero_red(void) {
  char salida[32];

  uint32_t ip1 = ip_a_entero("10.8.2.5");
  uint32_t mascara1 = cidr_a_mascara(29);
  uint32_t red1 = obtener_numero_red(ip1, mascara1);
  entero_a_ip(red1, salida, sizeof(salida));
  assert(strcmp(salida, "10.8.2.0") == 0);

  uint32_t ip2 = ip_a_entero("172.16.0.56");
  uint32_t mascara2 = ip_a_entero("255.255.255.128");
  uint32_t red2 = obtener_numero_red(ip2, mascara2);
  entero_a_ip(red2, salida, sizeof(salida));
  assert(strcmp(salida, "172.16.0.0") == 0);

  printf("[OK] Prueba Primitiva 2: GET NETWORK NUMBER\n");
}

void probar_rango_hosts(void) {
  char salida[64];

  uint32_t ip1 = ip_a_entero("10.8.2.5");
  uint32_t mascara1 = cidr_a_mascara(29);
  obtener_rango_hosts(ip1, mascara1, salida, sizeof(salida));
  assert(strcmp(salida, "10.8.2.{1-6}") == 0);

  uint32_t ip2 = ip_a_entero("172.16.0.56");
  uint32_t mascara2 = ip_a_entero("255.255.255.128");
  obtener_rango_hosts(ip2, mascara2, salida, sizeof(salida));
  assert(strcmp(salida, "172.16.0.{1-126}") == 0);

  printf("[OK] Prueba Primitiva 3: GET HOSTS RANGE\n");
}

void probar_mascara_red(void) {
  char salida[32];

  convertir_mascara_red("/27", salida, sizeof(salida));
  assert(strcmp(salida, "255.255.255.224") == 0);

  convertir_mascara_red("255.255.255.0", salida, sizeof(salida));
  assert(strcmp(salida, "/24") == 0);

  printf("[OK] Prueba Primitiva 4: GET NETMASK\n");
}

void probar_cantidad_hosts(void) {
  uint32_t cantidad1 = obtener_cantidad_hosts(29);
  assert(cantidad1 == 6);

  uint32_t mascara2 = ip_a_entero("255.255.255.128");
  int prefijo2 = mascara_a_cidr(mascara2);
  uint32_t cantidad2 = obtener_cantidad_hosts(prefijo2);
  assert(cantidad2 == 126);

  printf("[OK] Prueba Primitiva 5: GET HOSTS COUNT\n");
}

void probar_clase_ip(void) {
  uint32_t ip1 = ip_a_entero("10.8.2.5");
  assert(strcmp(obtener_clase_ip(ip1), "Clase A") == 0);

  uint32_t ip2 = ip_a_entero("172.16.0.56");
  assert(strcmp(obtener_clase_ip(ip2), "Clase B") == 0);

  uint32_t ip3 = ip_a_entero("192.168.1.1");
  assert(strcmp(obtener_clase_ip(ip3), "Clase C") == 0);

  printf("[OK] Prueba Primitiva 6: GET IP CLASS\n");
}

int main(void) {
  printf("---Probando---\n");
  probar_broadcast();
  probar_numero_red();
  probar_rango_hosts();
  probar_mascara_red();
  probar_cantidad_hosts();
  probar_clase_ip();
  printf("---Fin de las pruebas---\n");
  return 0;
}
