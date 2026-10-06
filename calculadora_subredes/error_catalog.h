#ifndef ERROR_CATALOG_H
#define ERROR_CATALOG_H

#include <stddef.h>
#include <stdbool.h>

/* ========================================================================= */
/* T-12: CATÁLOGO CENTRALIZADO DE MENSAJES DE ERROR                           */
/* ========================================================================= */

/* Codigos enumerados para categorias de error centralizadas */
typedef enum {
  ERR_CATALOG_SUCCESS = 0,
  ERR_CATALOG_NULL_INPUT,
  ERR_CATALOG_EMPTY_LINE,
  ERR_CATALOG_UNKNOWN_PRIMITIVE,
  ERR_CATALOG_INVALID_SYNTAX,
  ERR_CATALOG_MISSING_PARAM,
  ERR_CATALOG_EXTRA_PARAMS,
  ERR_CATALOG_INVALID_IP,
  ERR_CATALOG_INVALID_MASK,
  ERR_CATALOG_EXPECTED_KEYWORD,
  ERR_CATALOG_INVALID_NUMERIC,
  ERR_CATALOG_OUT_OF_RANGE,
  ERR_CATALOG_UNRECOGNIZED_PARAM
} error_code_t;

/* Retorna la representacion en texto del codigo de error */
const char *error_catalog_get_code_name(error_code_t code);

/* Funciones centralizadas para construccion y formateo de mensajes de error */

void err_cat_null_input(char *dest, size_t max_len);
void err_cat_empty_line(char *dest, size_t max_len);
void err_cat_unknown_primitive(char *dest, size_t max_len, const char *token);
void err_cat_invalid_syntax(char *dest, size_t max_len, const char *context);
void err_cat_missing_param(char *dest, size_t max_len, const char *context, const char *req_spec);
void err_cat_extra_params(char *dest, size_t max_len, const char *context);
void err_cat_invalid_ip(char *dest, size_t max_len, const char *ip_str);
void err_cat_invalid_mask(char *dest, size_t max_len, const char *mask_str);
void err_cat_expected_keyword(char *dest, size_t max_len, const char *context, const char *expected);
void err_cat_invalid_numeric(char *dest, size_t max_len, const char *param_name, const char *val_str);
void err_cat_out_of_range(char *dest, size_t max_len, const char *description, const char *val_str);
void err_cat_unrecognized_param(char *dest, size_t max_len, const char *context, const char *param_str);

#endif /* ERROR_CATALOG_H */
