#ifndef PARAM_EXTRACTOR_H
#define PARAM_EXTRACTOR_H

#include "parser.h"

/**
 * @file param_extractor.h
 * @brief Extractor consolidado de parámetros por comando (T-11).
 *
 * Proporciona estructuras y funciones accesores para extraer y convertir
 * de forma limpia los parámetros IP, MASK, NETWORK NUMBER, NUMBER, SIZE, PRIORITY y DEBUG.
 */

/**
 * @brief Estructura con la consolidación de parámetros extraídos por comando (T-11).
 */
typedef struct {
  primitive_type_t primitive;   /**< Tipo de primitiva procesada */
  bool is_valid;                 /**< Indica si la extracción fue exitosa */

  /* Parámetro IP */
  char ip_str[64];               /**< IP en formato texto decimal punteado */
  uint32_t ip_uint;              /**< IP convertida a entero de 32 bits (T-09) */
  bool has_ip;                   /**< Indica si la primitiva incluye parámetro IP */

  /* Parámetro Máscara */
  char mask_str[64];             /**< Máscara en formato texto (CIDR o decimal) */
  uint32_t mask_uint;            /**< Máscara convertida a binario de 32 bits (T-10) */
  int mask_cidr;                 /**< Prefijo CIDR equivalente [0..32] (T-10) */
  bool has_mask;                 /**< Indica si la primitiva incluye parámetro de máscara */

  /* Parámetros de Rutas (SET/DEL ROUTE) */
  char route_name[64];           /**< Nombre de la ruta <NAME> */
  bool has_route_name;           /**< Indica si existe nombre de ruta */

  char dest_net[64];             /**< Red de destino <DEST> */
  uint32_t dest_net_uint;        /**< Red de destino convertida a entero uint32_t */
  bool has_dest_net;             /**< Indica si existe red de destino */

  /* Parámetros Numéricos y Opciones */
  int priority;                  /**< Valor numérico de prioridad de la ruta */
  bool has_priority;             /**< Flag de presencia de parámetro PRIORITY */

  int size;                      /**< Tamaño/cantidad de subredes aleatorias solicitadas */
  bool has_size;                 /**< Flag de presencia de parámetro SIZE */

  bool debug;                    /**< Flag de depuración para ROUTE IP */
} extracted_params_t;

/**
 * @brief Inicializa la estructura extracted_params_t a estado neutro.
 * @param params Puntero a la estructura a inicializar.
 */
void param_extractor_init(extracted_params_t *params);

/**
 * @brief Extrae de forma consolidada todos los parámetros del comando parseado.
 * @param cmd Puntero al comando parseado resultante de parse_line (T-08).
 * @param params Puntero a la estructura extracted_params_t donde se almacenarán los resultados.
 * @return true si los parámetros fueron extraídos exitosamente, false en caso contrario.
 */
bool extraer_parametros_comando(const parsed_command_t *cmd, extracted_params_t *params);

/* ACCESORES INDIVIDUALES DE PARÁMETROS (T-11) */

/**
 * @brief Extrae la IP como cadena de texto decimal punteada.
 * @param cmd Comando parseado.
 * @param dest_ip Búfer donde se copiará la IP.
 * @param max_len Tamaño del búfer dest_ip.
 * @return true si la IP fue extraída correctamente.
 */
bool extraer_param_ip(const parsed_command_t *cmd, char *dest_ip, size_t max_len);

/**
 * @brief Extrae la IP como entero uint32_t de 32 bits (T-09).
 * @param cmd Comando parseado.
 * @param ip_uint Puntero donde se almacenará el entero uint32_t.
 * @return true si la IP se convirtió exitosamente a entero.
 */
bool extraer_param_ip_uint32(const parsed_command_t *cmd, uint32_t *ip_uint);

/**
 * @brief Extrae la máscara de red como cadena de texto (CIDR o decimal).
 * @param cmd Comando parseado.
 * @param dest_mask Búfer donde se copiará la máscara.
 * @param max_len Tamaño del búfer dest_mask.
 * @return true si la máscara fue extraída.
 */
bool extraer_param_mascara(const parsed_command_t *cmd, char *dest_mask, size_t max_len);

/**
 * @brief Extrae la máscara convertida a prefijo CIDR entero [0..32] (T-10).
 * @param cmd Comando parseado.
 * @param prefijo_cidr Puntero donde se almacenará el prefijo CIDR.
 * @return true si la máscara es válida y se obtuvo el prefijo.
 */
bool extraer_param_mascara_cidr(const parsed_command_t *cmd, int *prefijo_cidr);

/**
 * @brief Extrae la máscara convertida a entero binario uint32_t de 32 bits (T-10).
 * @param cmd Comando parseado.
 * @param mask_uint Puntero donde se almacenará el entero uint32_t de la máscara.
 * @return true si la máscara se convirtió exitosamente.
 */
bool extraer_param_mascara_uint32(const parsed_command_t *cmd, uint32_t *mask_uint);

/**
 * @brief Extrae el nombre de la ruta <NAME> para SET ROUTE o DEL ROUTE.
 * @param cmd Comando parseado.
 * @param dest_name Búfer de destino para el nombre.
 * @param max_len Tamaño máximo del búfer.
 * @return true si el nombre de ruta está presente.
 */
bool extraer_param_nombre_ruta(const parsed_command_t *cmd, char *dest_name, size_t max_len);

/**
 * @brief Extrae la red de destino <DEST> para SET ROUTE como cadena de texto.
 * @param cmd Comando parseado.
 * @param dest_net Búfer de destino para la red.
 * @param max_len Tamaño máximo del búfer.
 * @return true si el destino está presente.
 */
bool extraer_param_destino_ruta(const parsed_command_t *cmd, char *dest_net, size_t max_len);

/**
 * @brief Extrae la red de destino <DEST> para SET ROUTE como entero uint32_t (T-09).
 * @param cmd Comando parseado.
 * @param dest_net_uint Puntero donde se almacenará el entero uint32_t de destino.
 * @return true si la red de destino fue convertida a entero.
 */
bool extraer_param_destino_ruta_uint32(const parsed_command_t *cmd, uint32_t *dest_net_uint);

/**
 * @brief Extrae la prioridad opcional <PRIORITY> de SET ROUTE.
 * @param cmd Comando parseado.
 * @param prioridad Puntero donde se almacenará el valor de prioridad.
 * @return true si la prioridad estaba presente en el comando.
 */
bool extraer_param_prioridad(const parsed_command_t *cmd, int *prioridad);

/**
 * @brief Extrae el tamaño <SIZE> para la generación de subredes aleatorias GET RANDOM SUBNETS.
 * @param cmd Comando parseado.
 * @param size Puntero donde se almacenará la cantidad solicitada.
 * @return true si el parámetro SIZE está presente.
 */
bool extraer_param_size(const parsed_command_t *cmd, int *size);

/**
 * @brief Extrae la bandera opcional DEBUG de ROUTE IP.
 * @param cmd Comando parseado.
 * @param debug Puntero donde se almacenará el estado booleano de la bandera DEBUG.
 * @return true si el comando es válido.
 */
bool extraer_param_debug(const parsed_command_t *cmd, bool *debug);

#endif /* PARAM_EXTRACTOR_H */

