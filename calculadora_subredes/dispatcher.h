#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "param_extractor.h"
#include "error_catalog.h"
#include "calculo_subredes.h"

#include <stdbool.h>
#include <stddef.h>

/* Estructura con el resultado del despacho y calculo (T-13) */
typedef struct {
  primitive_type_t primitive;
  bool is_success;
  char result_str[512];
  char error_msg[256];
} dispatch_result_t;

/* Inicializa la estructura dispatch_result_t a estado neutro */
void dispatch_result_init(dispatch_result_t *result);

/* Despacha los parametros extraidos (extracted_params_t) al modulo de calculo correspondiente */
bool dispatch_extracted_params(const extracted_params_t *params, dispatch_result_t *result);

/* Conveniencia: realiza el parsing, extraccion y despacho completo en un unico paso */
bool dispatch_command_line(const char *input, dispatch_result_t *result);

#endif /* DISPATCHER_H */
