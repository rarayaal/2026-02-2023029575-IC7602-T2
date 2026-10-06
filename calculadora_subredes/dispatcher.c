#include "dispatcher.h"
#include <stdio.h>
#include <string.h>

void dispatch_result_init(dispatch_result_t *result) {
  if (!result) return;
  memset(result, 0, sizeof(*result));
  result->primitive = PRIMITIVE_UNKNOWN;
  result->is_success = false;
  result->result_str[0] = '\0';
  result->error_msg[0] = '\0';
}

bool dispatch_extracted_params(const extracted_params_t *params, dispatch_result_t *result) {
  dispatch_result_init(result);

  if (!result) return false;

  if (!params || !params->is_valid) {
    err_cat_invalid_syntax(result->error_msg, sizeof(result->error_msg), "parametros de entrada");
    return false;
  }

  result->primitive = params->primitive;

  switch (params->primitive) {
    case PRIMITIVE_GET_BROADCAST: {
      uint32_t bcast = obtener_ip_broadcast(params->ip_uint, params->mask_uint);
      entero_a_ip(bcast, result->result_str, sizeof(result->result_str));
      result->is_success = true;
      return true;
    }

    case PRIMITIVE_GET_NETWORK_NUMBER: {
      uint32_t net = obtener_numero_red(params->ip_uint, params->mask_uint);
      entero_a_ip(net, result->result_str, sizeof(result->result_str));
      result->is_success = true;
      return true;
    }

    case PRIMITIVE_GET_HOSTS_RANGE: {
      obtener_rango_hosts(params->ip_uint, params->mask_uint, result->result_str, sizeof(result->result_str));
      result->is_success = true;
      return true;
    }

    case PRIMITIVE_GET_NETMASK: {
      if (convertir_mascara_red(params->mask_str, result->result_str, sizeof(result->result_str)) == 0) {
        result->is_success = true;
        return true;
      } else {
        err_cat_invalid_mask(result->error_msg, sizeof(result->error_msg), params->mask_str);
        return false;
      }
    }

    case PRIMITIVE_GET_HOSTS_COUNT: {
      uint32_t hosts = obtener_cantidad_hosts(params->mask_cidr);
      snprintf(result->result_str, sizeof(result->result_str), "%u", hosts);
      result->is_success = true;
      return true;
    }

    case PRIMITIVE_GET_IP_CLASS: {
      const char *clase = obtener_clase_ip(params->ip_uint);
      snprintf(result->result_str, sizeof(result->result_str), "%s", clase);
      result->is_success = true;
      return true;
    }

    case PRIMITIVE_SET_ROUTE: {
      snprintf(result->result_str, sizeof(result->result_str),
               "OK: ruta %s configurada para red %s mascara %s prioridad %d",
               params->route_name, params->dest_net, params->mask_str,
               params->has_priority ? params->priority : 0);
      result->is_success = true;
      return true;
    }

    case PRIMITIVE_DEL_ROUTE: {
      snprintf(result->result_str, sizeof(result->result_str),
               "OK: ruta %s eliminada", params->route_name);
      result->is_success = true;
      return true;
    }

    case PRIMITIVE_GET_RANDOM_SUBNETS: {
      snprintf(result->result_str, sizeof(result->result_str),
               "OK: generadas %d subredes aleatorias para %s %s",
               params->size, params->ip_str, params->mask_str);
      result->is_success = true;
      return true;
    }

    case PRIMITIVE_ROUTE_IP: {
      snprintf(result->result_str, sizeof(result->result_str),
               "OK: enrutando IP %s (debug: %s)",
               params->ip_str, params->debug ? "activado" : "desactivado");
      result->is_success = true;
      return true;
    }

    default: {
      err_cat_unknown_primitive(result->error_msg, sizeof(result->error_msg), "DESCONOCIDA");
      return false;
    }
  }
}

bool dispatch_command_line(const char *input, dispatch_result_t *result) {
  parsed_command_t cmd;
  extracted_params_t params;

  dispatch_result_init(result);

  if (!parse_line(input, &cmd)) {
    snprintf(result->error_msg, sizeof(result->error_msg), "%s", cmd.error_msg);
    return false;
  }

  if (!extraer_parametros_comando(&cmd, &params)) {
    err_cat_invalid_syntax(result->error_msg, sizeof(result->error_msg), "extruccion de parametros");
    return false;
  }

  return dispatch_extracted_params(&params, result);
}
