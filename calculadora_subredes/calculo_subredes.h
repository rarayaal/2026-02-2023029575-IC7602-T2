#ifndef CALCULO_SUBREDES_H
#define CALCULO_SUBREDES_H

#include <stdint.h>
#include <stddef.h>

uint32_t ip_a_entero(const char *cadena_ip);
void entero_a_ip(uint32_t ip, char *buffer, size_t tamano_buffer);

uint32_t cidr_a_mascara(int prefijo);
int mascara_a_cidr(uint32_t mascara);

uint32_t obtener_numero_red(uint32_t ip, uint32_t mascara);
uint32_t obtener_ip_broadcast(uint32_t ip, uint32_t mascara);
uint32_t obtener_cantidad_hosts(int prefijo);
void obtener_rango_hosts(uint32_t ip, uint32_t mascara, char *buffer, size_t tamano_buffer);
const char *obtener_clase_ip(uint32_t ip);
int convertir_mascara_red(const char *entrada, char *salida, size_t tamano_salida);

#endif
