#include "error_catalog.h"
#include <stdio.h>
#include <string.h>

const char *error_catalog_get_code_name(error_code_t code) {
  switch (code) {
    case ERR_CATALOG_SUCCESS:            return "ERR_SUCCESS";
    case ERR_CATALOG_NULL_INPUT:         return "ERR_NULL_INPUT";
    case ERR_CATALOG_EMPTY_LINE:        return "ERR_EMPTY_LINE";
    case ERR_CATALOG_UNKNOWN_PRIMITIVE:  return "ERR_UNKNOWN_PRIMITIVE";
    case ERR_CATALOG_INVALID_SYNTAX:     return "ERR_INVALID_SYNTAX";
    case ERR_CATALOG_MISSING_PARAM:      return "ERR_MISSING_PARAM";
    case ERR_CATALOG_EXTRA_PARAMS:       return "ERR_EXTRA_PARAMS";
    case ERR_CATALOG_INVALID_IP:         return "ERR_INVALID_IP";
    case ERR_CATALOG_INVALID_MASK:       return "ERR_INVALID_MASK";
    case ERR_CATALOG_EXPECTED_KEYWORD:   return "ERR_EXPECTED_KEYWORD";
    case ERR_CATALOG_INVALID_NUMERIC:    return "ERR_INVALID_NUMERIC";
    case ERR_CATALOG_OUT_OF_RANGE:       return "ERR_OUT_OF_RANGE";
    case ERR_CATALOG_UNRECOGNIZED_PARAM:  return "ERR_UNRECOGNIZED_PARAM";
    default:                             return "ERR_UNKNOWN";
  }
}

void err_cat_null_input(char *dest, size_t max_len) {
  if (!dest || max_len == 0) return;
  snprintf(dest, max_len, "ERROR: entrada nula");
}

void err_cat_empty_line(char *dest, size_t max_len) {
  if (!dest || max_len == 0) return;
  snprintf(dest, max_len, "ERROR: linea vacia");
}

void err_cat_unknown_primitive(char *dest, size_t max_len, const char *token) {
  if (!dest || max_len == 0) return;
  snprintf(dest, max_len, "ERROR: primitiva desconocida: %s", token ? token : "");
}

void err_cat_invalid_syntax(char *dest, size_t max_len, const char *context) {
  if (!dest || max_len == 0) return;
  snprintf(dest, max_len, "ERROR: sintaxis invalida en %s", context ? context : "comando");
}

void err_cat_missing_param(char *dest, size_t max_len, const char *context, const char *req_spec) {
  if (!dest || max_len == 0) return;
  if (req_spec && strncmp(req_spec, "requiere ", 9) == 0) {
    bool plural = (strstr(req_spec, "SIZE") != NULL || strstr(req_spec, "<NAME>") != NULL);
    snprintf(dest, max_len, "ERROR: parametro%s en %s (%s)",
             plural ? "s faltantes" : " faltante",
             context ? context : "comando", req_spec);
  } else if (req_spec && *req_spec != '\0') {
    snprintf(dest, max_len, "ERROR: parametro %s faltante en %s", req_spec, context ? context : "comando");
  } else {
    snprintf(dest, max_len, "ERROR: parametro faltante en %s", context ? context : "comando");
  }
}

void err_cat_extra_params(char *dest, size_t max_len, const char *context) {
  if (!dest || max_len == 0) return;
  snprintf(dest, max_len, "ERROR: parametros adicionales no permitidos en %s", context ? context : "comando");
}

void err_cat_invalid_ip(char *dest, size_t max_len, const char *ip_str) {
  if (!dest || max_len == 0) return;
  snprintf(dest, max_len, "ERROR: direccion IP invalida: %s", ip_str ? ip_str : "");
}

void err_cat_invalid_mask(char *dest, size_t max_len, const char *mask_str) {
  if (!dest || max_len == 0) return;
  snprintf(dest, max_len, "ERROR: mascara de red invalida: %s", mask_str ? mask_str : "");
}

void err_cat_expected_keyword(char *dest, size_t max_len, const char *context, const char *expected) {
  if (!dest || max_len == 0) return;
  snprintf(dest, max_len, "ERROR: parametro invalido en %s, se esperaba %s", context ? context : "comando", expected ? expected : "");
}

void err_cat_invalid_numeric(char *dest, size_t max_len, const char *param_name, const char *val_str) {
  if (!dest || max_len == 0) return;
  snprintf(dest, max_len, "ERROR: valor de %s debe ser numerico: %s", param_name ? param_name : "parametro", val_str ? val_str : "");
}

void err_cat_out_of_range(char *dest, size_t max_len, const char *description, const char *val_str) {
  if (!dest || max_len == 0) return;
  snprintf(dest, max_len, "ERROR: %s: %s", description ? description : "valor fuera de rango", val_str ? val_str : "");
}

void err_cat_unrecognized_param(char *dest, size_t max_len, const char *context, const char *param_str) {
  if (!dest || max_len == 0) return;
  snprintf(dest, max_len, "ERROR: parametro no reconocido en %s: %s", context ? context : "comando", param_str ? param_str : "");
}
