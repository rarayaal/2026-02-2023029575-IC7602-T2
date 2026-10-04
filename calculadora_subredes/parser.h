#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Tipos de primitivas del protocolo (1 a 10) */
typedef enum {
  PRIMITIVE_UNKNOWN = 0,
  PRIMITIVE_GET_BROADCAST,      /* 1. GET BROADCAST IP <IP> MASK <MASK> */
  PRIMITIVE_GET_NETWORK_NUMBER, /* 2. GET NETWORK NUMBER IP <IP> MASK <MASK> */
  PRIMITIVE_GET_HOSTS_RANGE,    /* 3. GET HOSTS RANGE IP <IP> MASK <MASK> */
  PRIMITIVE_GET_NETMASK,        /* 4. GET NETMASK MASK <MASK> */
  PRIMITIVE_GET_HOSTS_COUNT,    /* 5. GET HOSTS COUNT MASK <MASK> */
  PRIMITIVE_GET_IP_CLASS,       /* 6. GET IP CLASS <IP> */
  PRIMITIVE_SET_ROUTE,          /* 7. SET ROUTE <NAME> DEST <NET> MASK <MASK> [PRIORITY <NUM>] */
  PRIMITIVE_DEL_ROUTE,          /* 8. DEL ROUTE <NAME> */
  PRIMITIVE_GET_RANDOM_SUBNETS, /* 9. GET RANDOM SUBNETS IP <IP> MASK <MASK> SIZE <NUM> */
  PRIMITIVE_ROUTE_IP            /* 10. ROUTE IP <IP> [DEBUG] */
} primitive_type_t;

/* Estructura con el resultado del análisis léxico y sintáctico (T-08) */
typedef struct {
  primitive_type_t primitive;
  bool is_valid;
  char ip[64];
  char mask[64];
  char name[64];
  char dest[64];
  int priority;
  bool has_priority;
  int size;
  bool has_size;
  bool debug;
  char error_msg[256];
} parsed_command_t;

/* Inicializa la estructura parsed_command_t a un estado por defecto neutro */
void parser_init_command(parsed_command_t *cmd);

/* Valida si una cadena cumple con el formato IPv4 (A.B.C.D, 0-255 por octeto) */
bool validar_ip(const char *cadena_ip);

/* Convierte una direccion IPv4 en notacion decimal punteada a entero de 32 bits.
   Retorna true si la conversion fue exitosa, almacenando el resultado en *ip_entero. (T-09) */
bool convertir_ip_a_entero(const char *cadena_ip, uint32_t *ip_entero);

/* Retorna la representacion entera de 32 bits de una IPv4. Retorna 0 si es invalida. (T-09) */
uint32_t ip_a_entero(const char *cadena_ip);

/* Valida si una cadena cumple con el formato de mascara (/N entre 0-32 o decimal contigua) */
bool validar_mascara(const char *cadena_mascara);

/* Función principal de T-08: realiza el análisis léxico, sintáctico y extracción de parámetros */
bool parse_line(const char *input, parsed_command_t *cmd);

/* Punto de entrada para el servidor TCP (mantiene la firma para server.h/main.c) */
void handle_command(const char *input, char *output, size_t max_len);

#endif /* PARSER_H */
