#include "param_extractor.h"
#include <stdio.h>
#include <string.h>

void param_extractor_init(extracted_params_t *params) {
  if (!params) return;
  memset(params, 0, sizeof(*params));
  params->primitive = PRIMITIVE_UNKNOWN;
  params->is_valid = false;
  params->ip_str[0] = '\0';
  params->ip_uint = 0U;
  params->has_ip = false;

  params->mask_str[0] = '\0';
  params->mask_uint = 0U;
  params->mask_cidr = -1;
  params->has_mask = false;

  params->route_name[0] = '\0';
  params->has_route_name = false;

  params->dest_net[0] = '\0';
  params->dest_net_uint = 0U;
  params->has_dest_net = false;

  params->priority = 0;
  params->has_priority = false;

  params->size = 0;
  params->has_size = false;

  params->debug = false;
}

bool extraer_param_ip(const parsed_command_t *cmd, char *dest_ip, size_t max_len) {
  if (!cmd || !cmd->is_valid || cmd->ip[0] == '\0') {
    if (dest_ip && max_len > 0) dest_ip[0] = '\0';
    return false;
  }
  if (dest_ip && max_len > 0) {
    snprintf(dest_ip, max_len, "%.63s", cmd->ip);
  }
  return true;
}

bool extraer_param_ip_uint32(const parsed_command_t *cmd, uint32_t *ip_uint) {
  if (!cmd || !cmd->is_valid || cmd->ip[0] == '\0') {
    if (ip_uint) *ip_uint = 0U;
    return false;
  }
  return convertir_ip_a_entero(cmd->ip, ip_uint);
}

bool extraer_param_mascara(const parsed_command_t *cmd, char *dest_mask, size_t max_len) {
  if (!cmd || !cmd->is_valid || cmd->mask[0] == '\0') {
    if (dest_mask && max_len > 0) dest_mask[0] = '\0';
    return false;
  }
  if (dest_mask && max_len > 0) {
    snprintf(dest_mask, max_len, "%.63s", cmd->mask);
  }
  return true;
}

bool extraer_param_mascara_cidr(const parsed_command_t *cmd, int *prefijo_cidr) {
  if (!cmd || !cmd->is_valid || cmd->mask[0] == '\0') {
    if (prefijo_cidr) *prefijo_cidr = -1;
    return false;
  }
  return parsear_mascara_a_cidr(cmd->mask, prefijo_cidr);
}

bool extraer_param_mascara_uint32(const parsed_command_t *cmd, uint32_t *mask_uint) {
  if (!cmd || !cmd->is_valid || cmd->mask[0] == '\0') {
    if (mask_uint) *mask_uint = 0U;
    return false;
  }
  return parsear_mascara_a_entero(cmd->mask, mask_uint);
}

bool extraer_param_nombre_ruta(const parsed_command_t *cmd, char *dest_name, size_t max_len) {
  if (!cmd || !cmd->is_valid || cmd->name[0] == '\0') {
    if (dest_name && max_len > 0) dest_name[0] = '\0';
    return false;
  }
  if (dest_name && max_len > 0) {
    snprintf(dest_name, max_len, "%.63s", cmd->name);
  }
  return true;
}

bool extraer_param_destino_ruta(const parsed_command_t *cmd, char *dest_net, size_t max_len) {
  if (!cmd || !cmd->is_valid || cmd->dest[0] == '\0') {
    if (dest_net && max_len > 0) dest_net[0] = '\0';
    return false;
  }
  if (dest_net && max_len > 0) {
    snprintf(dest_net, max_len, "%.63s", cmd->dest);
  }
  return true;
}

bool extraer_param_destino_ruta_uint32(const parsed_command_t *cmd, uint32_t *dest_net_uint) {
  if (!cmd || !cmd->is_valid || cmd->dest[0] == '\0') {
    if (dest_net_uint) *dest_net_uint = 0U;
    return false;
  }
  return convertir_ip_a_entero(cmd->dest, dest_net_uint);
}

bool extraer_param_prioridad(const parsed_command_t *cmd, int *prioridad) {
  if (!cmd || !cmd->is_valid || !cmd->has_priority) {
    if (prioridad) *prioridad = 0;
    return false;
  }
  if (prioridad) {
    *prioridad = cmd->priority;
  }
  return true;
}

bool extraer_param_size(const parsed_command_t *cmd, int *size) {
  if (!cmd || !cmd->is_valid || !cmd->has_size) {
    if (size) *size = 0;
    return false;
  }
  if (size) {
    *size = cmd->size;
  }
  return true;
}

bool extraer_param_debug(const parsed_command_t *cmd, bool *debug) {
  if (!cmd || !cmd->is_valid) {
    if (debug) *debug = false;
    return false;
  }
  if (debug) {
    *debug = cmd->debug;
  }
  return true;
}

bool extraer_parametros_comando(const parsed_command_t *cmd, extracted_params_t *params) {
  param_extractor_init(params);

  if (!cmd || !cmd->is_valid) {
    return false;
  }

  params->primitive = cmd->primitive;
  params->is_valid = cmd->is_valid;

  if (cmd->ip[0] != '\0') {
    params->has_ip = extraer_param_ip(cmd, params->ip_str, sizeof(params->ip_str));
    extraer_param_ip_uint32(cmd, &params->ip_uint);
  }

  if (cmd->mask[0] != '\0') {
    params->has_mask = extraer_param_mascara(cmd, params->mask_str, sizeof(params->mask_str));
    extraer_param_mascara_cidr(cmd, &params->mask_cidr);
    extraer_param_mascara_uint32(cmd, &params->mask_uint);
  }

  if (cmd->name[0] != '\0') {
    params->has_route_name = extraer_param_nombre_ruta(cmd, params->route_name, sizeof(params->route_name));
  }

  if (cmd->dest[0] != '\0') {
    params->has_dest_net = extraer_param_destino_ruta(cmd, params->dest_net, sizeof(params->dest_net));
    extraer_param_destino_ruta_uint32(cmd, &params->dest_net_uint);
  }

  if (cmd->has_priority) {
    params->has_priority = extraer_param_prioridad(cmd, &params->priority);
  }

  if (cmd->has_size) {
    params->has_size = extraer_param_size(cmd, &params->size);
  }

  params->debug = cmd->debug;

  return true;
}
