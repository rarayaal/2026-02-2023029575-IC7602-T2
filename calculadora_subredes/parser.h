#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @file parser.h
 * @brief Analizador léxico y sintáctico del protocolo de la calculadora de subredes (T-08, T-09, T-10).
 *
 * Define los tipos de primitivas del protocolo (1 a 10), la estructura de comando parseado
 * y las funciones de validación y conversión de direcciones IPv4 y máscaras de red.
 */

/**
 * @brief Tipos de primitivas del protocolo de comunicación (1 a 10).
 */
typedef enum {
  PRIMITIVE_UNKNOWN = 0,        /**< Primitiva desconocida o error sintáctico */
  PRIMITIVE_GET_BROADCAST,      /**< 1. GET BROADCAST IP <IP> MASK <MASK> */
  PRIMITIVE_GET_NETWORK_NUMBER, /**< 2. GET NETWORK NUMBER IP <IP> MASK <MASK> */
  PRIMITIVE_GET_HOSTS_RANGE,    /**< 3. GET HOSTS RANGE IP <IP> MASK <MASK> */
  PRIMITIVE_GET_NETMASK,        /**< 4. GET NETMASK MASK <MASK> */
  PRIMITIVE_GET_HOSTS_COUNT,    /**< 5. GET HOSTS COUNT MASK <MASK> */
  PRIMITIVE_GET_IP_CLASS,       /**< 6. GET IP CLASS <IP> */
  PRIMITIVE_SET_ROUTE,          /**< 7. SET ROUTE <NAME> DEST <NET> MASK <MASK> [PRIORITY <NUM>] */
  PRIMITIVE_DEL_ROUTE,          /**< 8. DEL ROUTE <NAME> */
  PRIMITIVE_GET_RANDOM_SUBNETS, /**< 9. GET RANDOM SUBNETS IP <IP> MASK <MASK> SIZE <NUM> */
  PRIMITIVE_ROUTE_IP            /**< 10. ROUTE IP <IP> [DEBUG] */
} primitive_type_t;

/**
 * @brief Estructura con el resultado del análisis léxico y sintáctico de un comando (T-08).
 */
typedef struct {
  primitive_type_t primitive;   /**< Tipo de primitiva identificada */
  bool is_valid;                 /**< Indica si la sintaxis y los parámetros son válidos */
  char ip[64];                   /**< Dirección IPv4 extraída en texto */
  char mask[64];                 /**< Máscara extraída en texto (CIDR /N o decimal punteada) */
  char name[64];                 /**< Nombre de la ruta (SET/DEL ROUTE) */
  char dest[64];                 /**< Dirección IP o red de destino (SET ROUTE) */
  int priority;                  /**< Prioridad numérica de la ruta */
  bool has_priority;             /**< Indica si el parámetro opcional PRIORITY está presente */
  int size;                      /**< Cantidad de subredes aleatorias solicitadas (GET RANDOM SUBNETS) */
  bool has_size;                 /**< Indica si el parámetro SIZE está presente */
  char subnet_mask[64];          /**< Máscara de las subredes hijas de P4 */
  bool has_subnet_mask;          /**< Indica si se recibió la máscara hija */
  bool debug;                    /**< Flag de depuración para ROUTE IP */
  char error_msg[256];           /**< Mensaje descriptivo de error centralizado (T-12) si is_valid es false */
} parsed_command_t;

/**
 * @brief Inicializa la estructura parsed_command_t a un estado neutro por defecto.
 * @param cmd Puntero a la estructura parsed_command_t a inicializar.
 */
void parser_init_command(parsed_command_t *cmd);

/* ========================================================================= */
/* FUNCIONES DE T-09: CONVERSIÓN Y VALIDACIÓN DE DIRECCIONES IPv4            */
/* ========================================================================= */

/**
 * @brief Valida si una cadena cumple con el formato estándar IPv4 decimal punteado (A.B.C.D).
 *
 * Verifica que existan exactamente 4 octetos con valores numéricos en el rango [0, 255],
 * separados por puntos, sin caracteres alfabéticos ni símbolos no permitidos.
 *
 * @param cadena_ip Cadena de texto con la dirección IP a validar.
 * @return true si la dirección es IPv4 válida, false en caso contrario.
 */
bool validar_ip(const char *cadena_ip);

/**
 * @brief Convierte una dirección IPv4 en notación decimal punteada a un entero de 32 bits (T-09).
 *
 * @param cadena_ip Cadena de texto con la dirección IP decimal punteada.
 * @param ip_entero Puntero donde se almacenará el resultado entero de 32 bits en orden de red.
 * @return true si la conversión fue exitosa, false si la IP es inválida.
 */
bool convertir_ip_a_entero(const char *cadena_ip, uint32_t *ip_entero);

/**
 * @brief Retorna la representación entera de 32 bits de una dirección IPv4 (T-09).
 *
 * @param cadena_ip Cadena de texto con la dirección IP decimal punteada.
 * @return uint32_t Entero de 32 bits, o 0U si la cadena es inválida.
 */
uint32_t ip_a_entero(const char *cadena_ip);

/**
 * @brief Valida si una cadena cumple con el formato de máscara (CIDR /N o decimal contigua).
 *
 * @param cadena_mascara Cadena de texto con la máscara a validar.
 * @return true si la máscara es válida y contigua, false en caso contrario.
 */
bool validar_mascara(const char *cadena_mascara);

/* ========================================================================= */
/* FUNCIONES DE T-10: CONVERSIÓN Y VALIDACIÓN DE MÁSCARAS DE RED             */
/* ========================================================================= */

/**
 * @brief Valida si un prefijo CIDR se encuentra dentro del rango válido [0, 32].
 * @param prefijo Valor entero del prefijo CIDR.
 * @return true si 0 <= prefijo <= 32, false en caso contrario.
 */
bool validar_prefijo_cidr(int prefijo);

/**
 * @brief Convierte un prefijo CIDR (0..32) a su equivalente en máscara binaria de 32 bits.
 * @param prefijo Valor del prefijo CIDR (0 a 32).
 * @param mascara Puntero donde se almacenará la máscara binaria uint32_t resultante.
 * @return true si el prefijo es válido y la conversión se realiza correctamente, false si es inválido.
 */
bool convertir_cidr_a_mascara(int prefijo, uint32_t *mascara);

/**
 * @brief Valida si una máscara binaria uint32_t posee bits 1 contiguos desde el MSB.
 * @param mascara Máscara binaria uint32_t a verificar.
 * @return true si los bits 1 son contiguos, false si es discontinua o no contigua.
 */
bool es_mascara_contigua(uint32_t mascara);

/**
 * @brief Convierte una máscara binaria uint32_t contigua a su correspondiente prefijo CIDR (0..32).
 * @param mascara Máscara binaria uint32_t.
 * @param prefijo Puntero donde se almacenará el prefijo CIDR entero resultante (0..32).
 * @return true si la máscara es contigua y la conversión es exitosa, false si no es contigua.
 */
bool convertir_mascara_a_cidr(uint32_t mascara, int *prefijo);

/**
 * @brief Convierte un prefijo CIDR (0..32) a su notación decimal punteada "A.B.C.D".
 * @param prefijo Prefijo CIDR (0..32).
 * @param output Búfer de salida donde se escribirá la cadena decimal punteada.
 * @param max_len Tamaño máximo del búfer de salida.
 * @return true si la conversión fue exitosa, false en caso de error de parámetros o prefijo fuera de rango.
 */
bool convertir_cidr_a_cadena_decimal(int prefijo, char *output, size_t max_len);

/**
 * @brief Convierte cualquier notación de máscara (CIDR "/N", "N" o decimal "A.B.C.D") a un prefijo CIDR (0..32).
 * @param mascara_str Cadena de texto con la máscara.
 * @param prefijo Puntero donde se almacenará el prefijo CIDR resultante.
 * @return true si la máscara pudo ser parseada y es válida, false en caso contrario.
 */
bool parsear_mascara_a_cidr(const char *mascara_str, int *prefijo);

/**
 * @brief Convierte cualquier notación de máscara (CIDR "/N" o decimal "A.B.C.D") a su representación binaria uint32_t.
 * @param mascara_str Cadena de texto con la máscara.
 * @param mascara_uint Puntero donde se almacenará el valor binario uint32_t.
 * @return true si la conversión fue exitosa, false si la máscara es inválida.
 */
bool parsear_mascara_a_entero(const char *mascara_str, uint32_t *mascara_uint);

/**
 * @brief Función principal de T-08: realiza el análisis léxico, sintáctico y extracción inicial del comando.
 * @param input Cadena de caracteres recibida con la línea de comando.
 * @param cmd Puntero a la estructura parsed_command_t donde se almacenarán los resultados.
 * @return true si el comando es sintácticamente válido, false si contiene errores de sintaxis o parámetros.
 */
bool parse_line(const char *input, parsed_command_t *cmd);

/**
 * @brief Punto de entrada de compatibilidad para el servidor TCP de la aplicación.
 * @param input Cadena con la solicitud recibida.
 * @param output Búfer de salida para enviar la respuesta.
 * @param max_len Tamaño máximo del búfer de salida.
 */
void handle_command(const char *input, char *output, size_t max_len);

#endif /* PARSER_H */

