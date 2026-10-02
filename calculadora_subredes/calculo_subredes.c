#include "calculo_subredes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t ip_a_entero(const char *cadena_ip) {
  unsigned int octeto1 = 0, octeto2 = 0, octeto3 = 0, octeto4 = 0;
  if (sscanf(cadena_ip, "%u.%u.%u.%u", &octeto1, &octeto2, &octeto3,
             &octeto4) != 4) {
    return 0;
  }
  return ((uint32_t)octeto1 << 24) | ((uint32_t)octeto2 << 16) |
         ((uint32_t)octeto3 << 8) | (uint32_t)octeto4;
}

void entero_a_ip(uint32_t ip, char *buffer, size_t tamano_buffer) {
  unsigned int octeto1 = (ip >> 24) & 0xFF;
  unsigned int octeto2 = (ip >> 16) & 0xFF;
  unsigned int octeto3 = (ip >> 8) & 0xFF;
  unsigned int octeto4 = ip & 0xFF;
  snprintf(buffer, tamano_buffer, "%u.%u.%u.%u", octeto1, octeto2, octeto3,
           octeto4);
}

uint32_t cidr_a_mascara(int prefijo) {
  if (prefijo <= 0)
    return 0;
  if (prefijo >= 32)
    return 0xFFFFFFFFU;
  return (0xFFFFFFFFU << (32 - prefijo));
}

int mascara_a_cidr(uint32_t mascara) {
  int prefijo = 0;
  while (mascara & 0x80000000U) {
    prefijo++;
    mascara <<= 1;
  }
  return prefijo;
}

uint32_t obtener_numero_red(uint32_t ip, uint32_t mascara) {
  return ip & mascara;
}

uint32_t obtener_ip_broadcast(uint32_t ip, uint32_t mascara) {
  return ip | (~mascara);
}

uint32_t obtener_cantidad_hosts(int prefijo) {
  if (prefijo >= 31 || prefijo < 0) {
    return 0;
  }
  return (1U << (32 - prefijo)) - 2;
}

void obtener_rango_hosts(uint32_t ip, uint32_t mascara, char *buffer,
                         size_t tamano_buffer) {
  uint32_t red = obtener_numero_red(ip, mascara);
  uint32_t broadcast = obtener_ip_broadcast(ip, mascara);
  int prefijo = mascara_a_cidr(mascara);

  if (prefijo >= 31) {
    snprintf(buffer, tamano_buffer, "N/A");
    return;
  }

  uint32_t primer_host = red + 1;
  uint32_t ultimo_host = broadcast - 1;

  unsigned int f1 = (primer_host >> 24) & 0xFF;
  unsigned int f2 = (primer_host >> 16) & 0xFF;
  unsigned int f3 = (primer_host >> 8) & 0xFF;
  unsigned int f4 = primer_host & 0xFF;

  unsigned int l1 = (ultimo_host >> 24) & 0xFF;
  unsigned int l2 = (ultimo_host >> 16) & 0xFF;
  unsigned int l3 = (ultimo_host >> 8) & 0xFF;
  unsigned int l4 = ultimo_host & 0xFF;

  if (f1 == l1 && f2 == l2 && f3 == l3) {
    snprintf(buffer, tamano_buffer, "%u.%u.%u.{%u-%u}", f1, f2, f3, f4, l4);
  } else {
    snprintf(buffer, tamano_buffer, "%u.%u.%u.%u - %u.%u.%u.%u", f1, f2, f3, f4,
             l1, l2, l3, l4);
  }
}

const char *obtener_clase_ip(uint32_t ip) {
  unsigned int primer_octeto = (ip >> 24) & 0xFF;
  if ((primer_octeto & 0x80) == 0) {
    return "Clase A";
  }
  if ((primer_octeto & 0xC0) == 0x80) {
    return "Clase B";
  }
  if ((primer_octeto & 0xE0) == 0xC0) {
    return "Clase C";
  }
  if ((primer_octeto & 0xF0) == 0xE0) {
    return "Clase D";
  }
  return "Clase E";
}

int convertir_mascara_red(const char *entrada, char *salida,
                          size_t tamano_salida) {
  if (entrada[0] == '/') {
    int prefijo = atoi(entrada + 1);
    if (prefijo < 0 || prefijo > 32) {
      return -1;
    }
    uint32_t mascara = cidr_a_mascara(prefijo);
    entero_a_ip(mascara, salida, tamano_salida);
    return 0;
  } else {
    uint32_t mascara = ip_a_entero(entrada);
    int prefijo = mascara_a_cidr(mascara);
    snprintf(salida, tamano_salida, "/%d", prefijo);
    return 0;
  }
}
