#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "param_extractor.h"
#include "error_catalog.h"
#include "calculo_subredes.h"

#include <stdbool.h>
#include <stddef.h>

/**
 * @file dispatcher.h
 * @brief Módulo de despacho a las funciones de cálculo de subredes y enrutamiento (T-13).
 *
 * Recibe los argumentos previamente validados y convertidos por el parser (T-08..T-10)
 * y el extractor (T-11), enviándolos a los módulos de cálculo correspondientes.
 */

/**
 * @brief Estructura con el resultado del despacho y ejecución del cálculo (T-13).
 */
typedef struct {
  primitive_type_t primitive;   /**< Primitiva ejecutada */
  bool is_success;               /**< Indica si el despacho y cálculo fueron exitosos */
  char result_str[512];          /**< Respuesta formateada de salida */
  char error_msg[256];           /**< Mensaje de error centralizado si is_success es false */
} dispatch_result_t;

/**
 * @brief Inicializa la estructura dispatch_result_t a estado neutro por defecto.
 * @param result Puntero a la estructura a inicializar.
 */
void dispatch_result_init(dispatch_result_t *result);

/**
 * @brief Despacha los parámetros extraídos (extracted_params_t) al módulo de cálculo correspondiente (T-13).
 *
 * No vuelve a tokenizar ni parsear. Toma las estructuras ya validadas y llama directamente
 * a las funciones de calculo_subredes.h (ej. obtener_ip_broadcast, obtener_numero_red, etc.).
 *
 * @param params Puntero a los parámetros extraídos.
 * @param result Puntero donde se almacenará el resultado del despacho.
 * @return true si el despacho fue exitoso, false en caso de error.
 */
bool dispatch_extracted_params(const extracted_params_t *params, dispatch_result_t *result);

/**
 * @brief Función de conveniencia: realiza parsing, extracción y despacho completo en un solo paso.
 * @param input Cadena de caracteres recibida con el comando.
 * @param result Puntero a la estructura dispatch_result_t de salida.
 * @return true si el comando fue procesado y despachado con éxito, false si ocurrió un error.
 */
bool dispatch_command_line(const char *input, dispatch_result_t *result);

#endif /* DISPATCHER_H */

