#ifndef PARAM_EXTRACTOR_H
#define PARAM_EXTRACTOR_H

#include "parser.h"

/* Estructura con la consolidacion de parametros extraidos por comando (T-11) */
typedef struct {
  primitive_type_t primitive;
  bool is_valid;

  /* Parametro IP */
  char ip_str[64];
  uint32_t ip_uint;
  bool has_ip;

  /* Parametro Mascara */
  char mask_str[64];
  uint32_t mask_uint;
  int mask_cidr;
  bool has_mask;

  /* Parametros de Rutas */
  char route_name[64];
  bool has_route_name;

  char dest_net[64];
  uint32_t dest_net_uint;
  bool has_dest_net;

  /* Parametros Numericos y Opciones */
  int priority;
  bool has_priority;

  int size;
  bool has_size;

  bool debug;
} extracted_params_t;

/* Inicializa la estructura extracted_params_t a estado neutro */
void param_extractor_init(extracted_params_t *params);

/* Extrae de forma consolidada todos los parametros del comando parseado */
bool extraer_parametros_comando(const parsed_command_t *cmd, extracted_params_t *params);

/* Accesores individuales de parametros (T-11) */

/* Extrae la IP como cadena de texto. Retorna true si el comando incluye IP */
bool extraer_param_ip(const parsed_command_t *cmd, char *dest_ip, size_t max_len);

/* Extrae la IP como entero uint32_t de 32 bits (T-09). Retorna true si el comando incluye IP */
bool extraer_param_ip_uint32(const parsed_command_t *cmd, uint32_t *ip_uint);

/* Extrae la mascara como cadena (CIDR o decimal). Retorna true si el comando incluye MASK */
bool extraer_param_mascara(const parsed_command_t *cmd, char *dest_mask, size_t max_len);

/* Extrae la mascara convertida a prefijo CIDR (0..32) (T-10). Retorna true si es valida */
bool extraer_param_mascara_cidr(const parsed_command_t *cmd, int *prefijo_cidr);

/* Extrae la mascara convertida a entero uint32_t (T-10). Retorna true si es valida */
bool extraer_param_mascara_uint32(const parsed_command_t *cmd, uint32_t *mask_uint);

/* Extrae el nombre de ruta <NAME> para SET ROUTE o DEL ROUTE */
bool extraer_param_nombre_ruta(const parsed_command_t *cmd, char *dest_name, size_t max_len);

/* Extrae la red destino <DEST> para SET ROUTE como cadena */
bool extraer_param_destino_ruta(const parsed_command_t *cmd, char *dest_net, size_t max_len);

/* Extrae la red destino <DEST> para SET ROUTE como entero uint32_t (T-09) */
bool extraer_param_destino_ruta_uint32(const parsed_command_t *cmd, uint32_t *dest_net_uint);

/* Extrae la prioridad opcional <PRIORITY> de SET ROUTE */
bool extraer_param_prioridad(const parsed_command_t *cmd, int *prioridad);

/* Extrae el tamaño <SIZE> de GET RANDOM SUBNETS */
bool extraer_param_size(const parsed_command_t *cmd, int *size);

/* Extrae la bandera opcional DEBUG de ROUTE IP */
bool extraer_param_debug(const parsed_command_t *cmd, bool *debug);

#endif /* PARAM_EXTRACTOR_H */
