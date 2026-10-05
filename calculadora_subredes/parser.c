#include "parser.h"
#include "error_catalog.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 16
#define MAX_TOKEN_LEN 128

void parser_init_command(parsed_command_t *cmd) {
  if (!cmd) return;
  memset(cmd, 0, sizeof(*cmd));
  cmd->primitive = PRIMITIVE_UNKNOWN;
  cmd->priority = 0;
  cmd->has_priority = false;
  cmd->size = 0;
  cmd->has_size = false;
  cmd->debug = false;
  cmd->is_valid = false;
  cmd->error_msg[0] = '\0';
}

static void str_to_upper(char *str) {
  for (size_t i = 0; str[i] != '\0'; i++) {
    str[i] = (char)toupper((unsigned char)str[i]);
  }
}

bool validar_ip(const char *cadena_ip) {
  if (!cadena_ip || *cadena_ip == '\0') return false;

  long long octetos[4];
  char extra[2];
  if (sscanf(cadena_ip, "%lld.%lld.%lld.%lld%1s", &octetos[0], &octetos[1],
             &octetos[2], &octetos[3], extra) != 4) {
    return false;
  }

  for (int i = 0; i < 4; i++) {
    if (octetos[i] < 0 || octetos[i] > 255) {
      return false;
    }
  }

  /* Solo digitos y puntos */
  for (size_t i = 0; cadena_ip[i] != '\0'; i++) {
    if (!isdigit((unsigned char)cadena_ip[i]) && cadena_ip[i] != '.') {
      return false;
    }
  }

  return true;
}

bool convertir_ip_a_entero(const char *cadena_ip, uint32_t *ip_entero) {
  if (!validar_ip(cadena_ip)) {
    if (ip_entero) *ip_entero = 0;
    return false;
  }

  unsigned int oct1 = 0, oct2 = 0, oct3 = 0, oct4 = 0;
  sscanf(cadena_ip, "%u.%u.%u.%u", &oct1, &oct2, &oct3, &oct4);

  uint32_t res = ((uint32_t)oct1 << 24) | ((uint32_t)oct2 << 16) |
                 ((uint32_t)oct3 << 8) | (uint32_t)oct4;

  if (ip_entero) {
    *ip_entero = res;
  }
  return true;
}

uint32_t ip_a_entero(const char *cadena_ip) {
  uint32_t res = 0;
  if (convertir_ip_a_entero(cadena_ip, &res)) {
    return res;
  }
  return 0;
}

bool validar_prefijo_cidr(int prefijo) {
  return (prefijo >= 0 && prefijo <= 32);
}

bool convertir_cidr_a_mascara(int prefijo, uint32_t *mascara) {
  if (!validar_prefijo_cidr(prefijo)) {
    if (mascara) *mascara = 0U;
    return false;
  }
  if (!mascara) return true;

  if (prefijo == 0) {
    *mascara = 0U;
  } else if (prefijo == 32) {
    *mascara = 0xFFFFFFFFU;
  } else {
    *mascara = (0xFFFFFFFFU << (32 - prefijo));
  }
  return true;
}

bool es_mascara_contigua(uint32_t mascara) {
  if (mascara == 0U) return true;
  uint32_t invertida = ~mascara;
  return ((invertida & (invertida + 1U)) == 0U);
}

bool convertir_mascara_a_cidr(uint32_t mascara, int *prefijo) {
  if (!es_mascara_contigua(mascara)) {
    if (prefijo) *prefijo = -1;
    return false;
  }

  int count = 0;
  uint32_t temp = mascara;
  while (temp > 0U) {
    count += (int)(temp & 1U);
    temp >>= 1U;
  }

  if (prefijo) {
    *prefijo = count;
  }
  return true;
}

bool convertir_cidr_a_cadena_decimal(int prefijo, char *output, size_t max_len) {
  uint32_t mascara = 0U;
  if (!convertir_cidr_a_mascara(prefijo, &mascara) || !output || max_len == 0) {
    return false;
  }

  unsigned int oct1 = (mascara >> 24) & 0xFFU;
  unsigned int oct2 = (mascara >> 16) & 0xFFU;
  unsigned int oct3 = (mascara >> 8) & 0xFFU;
  unsigned int oct4 = mascara & 0xFFU;

  snprintf(output, max_len, "%u.%u.%u.%u", oct1, oct2, oct3, oct4);
  return true;
}

bool parsear_mascara_a_cidr(const char *mascara_str, int *prefijo) {
  if (!mascara_str || *mascara_str == '\0') {
    if (prefijo) *prefijo = -1;
    return false;
  }

  const char *p = mascara_str;
  if (*p == '/') {
    p++;
  }

  bool todo_digitos = (*p != '\0');
  for (size_t i = 0; p[i] != '\0'; i++) {
    if (!isdigit((unsigned char)p[i])) {
      todo_digitos = false;
      break;
    }
  }

  if (todo_digitos) {
    char *endptr = NULL;
    long val = strtol(p, &endptr, 10);
    if (*endptr != '\0') {
      if (prefijo) *prefijo = -1;
      return false;
    }
    if (val >= 0 && val <= 32) {
      if (prefijo) *prefijo = (int)val;
      return true;
    }
    if (prefijo) *prefijo = -1;
    return false;
  }

  if (!validar_ip(mascara_str)) {
    if (prefijo) *prefijo = -1;
    return false;
  }

  uint32_t mascara_uint = ip_a_entero(mascara_str);
  return convertir_mascara_a_cidr(mascara_uint, prefijo);
}

bool parsear_mascara_a_entero(const char *mascara_str, uint32_t *mascara_uint) {
  int prefijo = -1;
  if (!parsear_mascara_a_cidr(mascara_str, &prefijo)) {
    if (mascara_uint) *mascara_uint = 0U;
    return false;
  }
  return convertir_cidr_a_mascara(prefijo, mascara_uint);
}

bool validar_mascara(const char *cadena_mascara) {
  int prefijo = -1;
  return parsear_mascara_a_cidr(cadena_mascara, &prefijo);
}

static bool es_numero(const char *cadena) {
  if (!cadena || *cadena == '\0') return false;
  for (size_t i = 0; cadena[i] != '\0'; i++) {
    if (!isdigit((unsigned char)cadena[i])) {
      return false;
    }
  }
  return true;
}

static size_t tokenizar(const char *input,
                         char tokens[MAX_TOKENS][MAX_TOKEN_LEN]) {
  size_t count = 0;
  const char *p = input;

  while (*p != '\0' && count < MAX_TOKENS) {
    while (*p != '\0' && isspace((unsigned char)*p)) {
      p++;
    }
    if (*p == '\0') break;

    size_t len = 0;
    while (*p != '\0' && !isspace((unsigned char)*p) && len < MAX_TOKEN_LEN - 1) {
      tokens[count][len++] = *p++;
    }
    tokens[count][len] = '\0';
    count++;

    while (*p != '\0' && !isspace((unsigned char)*p)) {
      p++;
    }
  }
  return count;
}

bool parse_line(const char *input, parsed_command_t *cmd) {
  parser_init_command(cmd);

  if (!input) {
    err_cat_null_input(cmd->error_msg, sizeof(cmd->error_msg));
    return false;
  }

  char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
  size_t ntokens = tokenizar(input, tokens);

  if (ntokens == 0) {
    err_cat_empty_line(cmd->error_msg, sizeof(cmd->error_msg));
    return false;
  }

  char t0[MAX_TOKEN_LEN];
  snprintf(t0, sizeof(t0), "%s", tokens[0]);
  str_to_upper(t0);

  /* 1-6 y 9: Comandos que inician con GET */
  if (strcmp(t0, "GET") == 0) {
    if (ntokens < 2) {
      err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "comando GET");
      return false;
    }

    char t1[MAX_TOKEN_LEN];
    snprintf(t1, sizeof(t1), "%s", tokens[1]);
    str_to_upper(t1);

    /* 1. GET BROADCAST IP <IP> MASK <MASK> */
    if (strcmp(t1, "BROADCAST") == 0) {
      cmd->primitive = PRIMITIVE_GET_BROADCAST;
      if (ntokens < 6) {
        err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg),
                              "GET BROADCAST", "requiere IP <IP> MASK <MASK>");
        return false;
      }
      if (ntokens > 6) {
        err_cat_extra_params(cmd->error_msg, sizeof(cmd->error_msg), "GET BROADCAST");
        return false;
      }
      char p2[MAX_TOKEN_LEN], p4[MAX_TOKEN_LEN];
      snprintf(p2, sizeof(p2), "%s", tokens[2]); str_to_upper(p2);
      snprintf(p4, sizeof(p4), "%s", tokens[4]); str_to_upper(p4);

      if (strcmp(p2, "IP") != 0 || strcmp(p4, "MASK") != 0) {
        err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET BROADCAST");
        return false;
      }

      if (!validar_ip(tokens[3])) {
        err_cat_invalid_ip(cmd->error_msg, sizeof(cmd->error_msg), tokens[3]);
        return false;
      }
      if (!validar_mascara(tokens[5])) {
        err_cat_invalid_mask(cmd->error_msg, sizeof(cmd->error_msg), tokens[5]);
        return false;
      }

      snprintf(cmd->ip, sizeof(cmd->ip), "%.63s", tokens[3]);
      snprintf(cmd->mask, sizeof(cmd->mask), "%.63s", tokens[5]);
      cmd->is_valid = true;
      return true;
    }

    /* 2. GET NETWORK NUMBER IP <IP> MASK <MASK> */
    if (strcmp(t1, "NETWORK") == 0) {
      if (ntokens < 3) {
        err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET NETWORK");
        return false;
      }
      char t2[MAX_TOKEN_LEN];
      snprintf(t2, sizeof(t2), "%s", tokens[2]); str_to_upper(t2);

      if (strcmp(t2, "NUMBER") == 0) {
        cmd->primitive = PRIMITIVE_GET_NETWORK_NUMBER;
        if (ntokens < 7) {
          err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg),
                                "GET NETWORK NUMBER", "requiere IP <IP> MASK <MASK>");
          return false;
        }
        if (ntokens > 7) {
          err_cat_extra_params(cmd->error_msg, sizeof(cmd->error_msg), "GET NETWORK NUMBER");
          return false;
        }
        char p3[MAX_TOKEN_LEN], p5[MAX_TOKEN_LEN];
        snprintf(p3, sizeof(p3), "%s", tokens[3]); str_to_upper(p3);
        snprintf(p5, sizeof(p5), "%s", tokens[5]); str_to_upper(p5);

        if (strcmp(p3, "IP") != 0 || strcmp(p5, "MASK") != 0) {
          err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET NETWORK NUMBER");
          return false;
        }

        if (!validar_ip(tokens[4])) {
          err_cat_invalid_ip(cmd->error_msg, sizeof(cmd->error_msg), tokens[4]);
          return false;
        }
        if (!validar_mascara(tokens[6])) {
          err_cat_invalid_mask(cmd->error_msg, sizeof(cmd->error_msg), tokens[6]);
          return false;
        }

        snprintf(cmd->ip, sizeof(cmd->ip), "%.63s", tokens[4]);
        snprintf(cmd->mask, sizeof(cmd->mask), "%.63s", tokens[6]);
        cmd->is_valid = true;
        return true;
      }
    }

    /* 3. GET HOSTS RANGE / 5. GET HOSTS COUNT */
    if (strcmp(t1, "HOSTS") == 0) {
      if (ntokens < 3) {
        err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET HOSTS");
        return false;
      }
      char t2[MAX_TOKEN_LEN];
      snprintf(t2, sizeof(t2), "%s", tokens[2]); str_to_upper(t2);

      /* 3. GET HOSTS RANGE IP <IP> MASK <MASK> */
      if (strcmp(t2, "RANGE") == 0) {
        cmd->primitive = PRIMITIVE_GET_HOSTS_RANGE;
        if (ntokens < 7) {
          err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg), "GET HOSTS RANGE", "");
          return false;
        }
        if (ntokens > 7) {
          err_cat_extra_params(cmd->error_msg, sizeof(cmd->error_msg), "GET HOSTS RANGE");
          return false;
        }
        char p3[MAX_TOKEN_LEN], p5[MAX_TOKEN_LEN];
        snprintf(p3, sizeof(p3), "%s", tokens[3]); str_to_upper(p3);
        snprintf(p5, sizeof(p5), "%s", tokens[5]); str_to_upper(p5);

        if (strcmp(p3, "IP") != 0 || strcmp(p5, "MASK") != 0) {
          err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET HOSTS RANGE");
          return false;
        }

        if (!validar_ip(tokens[4])) {
          err_cat_invalid_ip(cmd->error_msg, sizeof(cmd->error_msg), tokens[4]);
          return false;
        }
        if (!validar_mascara(tokens[6])) {
          err_cat_invalid_mask(cmd->error_msg, sizeof(cmd->error_msg), tokens[6]);
          return false;
        }

        snprintf(cmd->ip, sizeof(cmd->ip), "%.63s", tokens[4]);
        snprintf(cmd->mask, sizeof(cmd->mask), "%.63s", tokens[6]);
        cmd->is_valid = true;
        return true;
      }

      /* 5. GET HOSTS COUNT MASK <MASK> */
      if (strcmp(t2, "COUNT") == 0) {
        cmd->primitive = PRIMITIVE_GET_HOSTS_COUNT;
        if (ntokens < 4) {
          err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg), "GET HOSTS COUNT", "MASK");
          return false;
        }
        const char *mask_val = NULL;
        if (ntokens == 5) {
          char p3[MAX_TOKEN_LEN];
          snprintf(p3, sizeof(p3), "%s", tokens[3]); str_to_upper(p3);
          if (strcmp(p3, "MASK") != 0) {
            err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET HOSTS COUNT");
            return false;
          }
          mask_val = tokens[4];
        } else if (ntokens == 4) {
          mask_val = tokens[3];
        } else {
          err_cat_extra_params(cmd->error_msg, sizeof(cmd->error_msg), "GET HOSTS COUNT");
          return false;
        }

        if (!validar_mascara(mask_val)) {
          err_cat_invalid_mask(cmd->error_msg, sizeof(cmd->error_msg), mask_val);
          return false;
        }

        snprintf(cmd->mask, sizeof(cmd->mask), "%.63s", mask_val);
        cmd->is_valid = true;
        return true;
      }
    }

    /* 4. GET NETMASK MASK <MASK> */
    if (strcmp(t1, "NETMASK") == 0) {
      cmd->primitive = PRIMITIVE_GET_NETMASK;
      if (ntokens < 3) {
        err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg), "GET NETMASK", "MASK");
        return false;
      }
      const char *mask_val = NULL;
      if (ntokens == 4) {
        char p2[MAX_TOKEN_LEN];
        snprintf(p2, sizeof(p2), "%s", tokens[2]); str_to_upper(p2);
        if (strcmp(p2, "MASK") != 0) {
          err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET NETMASK");
          return false;
        }
        mask_val = tokens[3];
      } else if (ntokens == 3) {
        mask_val = tokens[2];
      } else {
        err_cat_extra_params(cmd->error_msg, sizeof(cmd->error_msg), "GET NETMASK");
        return false;
      }

      if (!validar_mascara(mask_val)) {
        err_cat_invalid_mask(cmd->error_msg, sizeof(cmd->error_msg), mask_val);
        return false;
      }

      snprintf(cmd->mask, sizeof(cmd->mask), "%.63s", mask_val);
      cmd->is_valid = true;
      return true;
    }

    /* 6. GET IP CLASS <IP> */
    if (strcmp(t1, "IP") == 0) {
      if (ntokens < 3) {
        err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET IP");
        return false;
      }
      char t2[MAX_TOKEN_LEN];
      snprintf(t2, sizeof(t2), "%s", tokens[2]); str_to_upper(t2);

      if (strcmp(t2, "CLASS") == 0) {
        cmd->primitive = PRIMITIVE_GET_IP_CLASS;
        const char *ip_val = NULL;
        if (ntokens == 4) {
          ip_val = tokens[3];
        } else if (ntokens == 5) {
          char p3[MAX_TOKEN_LEN];
          snprintf(p3, sizeof(p3), "%s", tokens[3]); str_to_upper(p3);
          if (strcmp(p3, "IP") != 0) {
            err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET IP CLASS");
            return false;
          }
          ip_val = tokens[4];
        } else {
          if (ntokens < 4) {
            err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg), "GET IP CLASS", "IP");
          } else {
            err_cat_extra_params(cmd->error_msg, sizeof(cmd->error_msg), "GET IP CLASS");
          }
          return false;
        }

        if (!validar_ip(ip_val)) {
          err_cat_invalid_ip(cmd->error_msg, sizeof(cmd->error_msg), ip_val);
          return false;
        }

        snprintf(cmd->ip, sizeof(cmd->ip), "%.63s", ip_val);
        cmd->is_valid = true;
        return true;
      }
    }

    /* 9. GET RANDOM SUBNETS IP <IP> MASK <MASK> SIZE <NUM> */
    if (strcmp(t1, "RANDOM") == 0) {
      if (ntokens < 3) {
        err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET RANDOM");
        return false;
      }
      char t2[MAX_TOKEN_LEN];
      snprintf(t2, sizeof(t2), "%s", tokens[2]); str_to_upper(t2);

      if (strcmp(t2, "SUBNETS") == 0) {
        cmd->primitive = PRIMITIVE_GET_RANDOM_SUBNETS;
        if (ntokens < 9) {
          err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg),
                                "GET RANDOM SUBNETS", "requiere IP <IP> MASK <MASK> SIZE <NUM>");
          return false;
        }
        if (ntokens > 9) {
          err_cat_extra_params(cmd->error_msg, sizeof(cmd->error_msg), "GET RANDOM SUBNETS");
          return false;
        }

        char p3[MAX_TOKEN_LEN], p5[MAX_TOKEN_LEN], p7[MAX_TOKEN_LEN];
        snprintf(p3, sizeof(p3), "%s", tokens[3]); str_to_upper(p3);
        snprintf(p5, sizeof(p5), "%s", tokens[5]); str_to_upper(p5);
        snprintf(p7, sizeof(p7), "%s", tokens[7]); str_to_upper(p7);

        if (strcmp(p3, "IP") != 0 || strcmp(p5, "MASK") != 0 || strcmp(p7, "SIZE") != 0) {
          err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET RANDOM SUBNETS");
          return false;
        }

        if (!validar_ip(tokens[4])) {
          err_cat_invalid_ip(cmd->error_msg, sizeof(cmd->error_msg), tokens[4]);
          return false;
        }
        if (!validar_mascara(tokens[6])) {
          err_cat_invalid_mask(cmd->error_msg, sizeof(cmd->error_msg), tokens[6]);
          return false;
        }
        if (!es_numero(tokens[8])) {
          err_cat_invalid_numeric(cmd->error_msg, sizeof(cmd->error_msg), "SIZE", tokens[8]);
          return false;
        }

        int size_val = atoi(tokens[8]);
        if (size_val <= 0) {
          err_cat_out_of_range(cmd->error_msg, sizeof(cmd->error_msg),
                               "el tamaño debe ser mayor a 0", tokens[8]);
          return false;
        }

        snprintf(cmd->ip, sizeof(cmd->ip), "%.63s", tokens[4]);
        snprintf(cmd->mask, sizeof(cmd->mask), "%.63s", tokens[6]);
        cmd->size = size_val;
        cmd->has_size = true;
        cmd->is_valid = true;
        return true;
      }
    }
  }

  /* 7. SET ROUTE <NAME> DEST <NET> MASK <MASK> [PRIORITY <NUM>] */
  if (strcmp(t0, "SET") == 0) {
    if (ntokens < 2) {
      err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "comando SET");
      return false;
    }
    char t1[MAX_TOKEN_LEN];
    snprintf(t1, sizeof(t1), "%s", tokens[1]); str_to_upper(t1);

    if (strcmp(t1, "ROUTE") == 0) {
      cmd->primitive = PRIMITIVE_SET_ROUTE;
      if (ntokens < 7) {
        err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg),
                              "SET ROUTE", "requiere <NAME> DEST <NET> MASK <MASK>");
        return false;
      }

      char p3[MAX_TOKEN_LEN], p5[MAX_TOKEN_LEN];
      snprintf(p3, sizeof(p3), "%s", tokens[3]); str_to_upper(p3);
      snprintf(p5, sizeof(p5), "%s", tokens[5]); str_to_upper(p5);

      if ((strcmp(p3, "DEST") != 0 && strcmp(p3, "DESTINATION") != 0 && strcmp(p3, "NET") != 0) ||
          strcmp(p5, "MASK") != 0) {
        err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "SET ROUTE");
        return false;
      }

      if (!validar_ip(tokens[4])) {
        err_cat_invalid_ip(cmd->error_msg, sizeof(cmd->error_msg), tokens[4]);
        return false;
      }
      if (!validar_mascara(tokens[6])) {
        err_cat_invalid_mask(cmd->error_msg, sizeof(cmd->error_msg), tokens[6]);
        return false;
      }

      /* Preservar insensibilidad a mayusculas/minusculas para el nombre de ruta <NAME> */
      snprintf(cmd->name, sizeof(cmd->name), "%.63s", tokens[2]);
      snprintf(cmd->dest, sizeof(cmd->dest), "%.63s", tokens[4]);
      snprintf(cmd->mask, sizeof(cmd->mask), "%.63s", tokens[6]);

      if (ntokens == 9) {
        char p7[MAX_TOKEN_LEN];
        snprintf(p7, sizeof(p7), "%s", tokens[7]); str_to_upper(p7);
        if (strcmp(p7, "PRIORITY") != 0) {
          err_cat_expected_keyword(cmd->error_msg, sizeof(cmd->error_msg), "SET ROUTE", "PRIORITY");
          return false;
        }
        if (!es_numero(tokens[8])) {
          err_cat_invalid_numeric(cmd->error_msg, sizeof(cmd->error_msg), "PRIORITY", tokens[8]);
          return false;
        }
        cmd->priority = atoi(tokens[8]);
        cmd->has_priority = true;
      } else if (ntokens != 7) {
        err_cat_extra_params(cmd->error_msg, sizeof(cmd->error_msg), "SET ROUTE");
        return false;
      }

      cmd->is_valid = true;
      return true;
    }
  }

  /* 8. DEL ROUTE <NAME> */
  if (strcmp(t0, "DEL") == 0) {
    if (ntokens < 2) {
      err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "comando DEL");
      return false;
    }
    char t1[MAX_TOKEN_LEN];
    snprintf(t1, sizeof(t1), "%s", tokens[1]); str_to_upper(t1);

    if (strcmp(t1, "ROUTE") == 0) {
      cmd->primitive = PRIMITIVE_DEL_ROUTE;
      if (ntokens < 3) {
        err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg), "DEL ROUTE", "nombre");
        return false;
      }
      if (ntokens > 3) {
        err_cat_extra_params(cmd->error_msg, sizeof(cmd->error_msg), "DEL ROUTE");
        return false;
      }

      /* Preservar insensibilidad a mayusculas/minusculas para el nombre <NAME> */
      snprintf(cmd->name, sizeof(cmd->name), "%.63s", tokens[2]);
      cmd->is_valid = true;
      return true;
    }
  }

  /* 10. ROUTE IP <IP> [DEBUG] */
  if (strcmp(t0, "ROUTE") == 0) {
    if (ntokens < 2) {
      err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "comando ROUTE");
      return false;
    }
    char t1[MAX_TOKEN_LEN];
    snprintf(t1, sizeof(t1), "%s", tokens[1]); str_to_upper(t1);

    if (strcmp(t1, "IP") == 0) {
      cmd->primitive = PRIMITIVE_ROUTE_IP;
      const char *ip_val = NULL;
      size_t debug_idx = 0;

      if (ntokens >= 3 && validar_ip(tokens[2])) {
        ip_val = tokens[2];
        debug_idx = 3;
      } else if (ntokens >= 4) {
        char p2[MAX_TOKEN_LEN];
        snprintf(p2, sizeof(p2), "%s", tokens[2]); str_to_upper(p2);
        if (strcmp(p2, "IP") == 0 && validar_ip(tokens[3])) {
          ip_val = tokens[3];
          debug_idx = 4;
        }
      }

      if (!ip_val) {
        if (ntokens < 3) {
          err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg), "ROUTE IP", "IP");
        } else {
          err_cat_invalid_ip(cmd->error_msg, sizeof(cmd->error_msg), tokens[2]);
        }
        return false;
      }

      if (debug_idx < ntokens) {
        char pdebug[MAX_TOKEN_LEN];
        snprintf(pdebug, sizeof(pdebug), "%s", tokens[debug_idx]); str_to_upper(pdebug);
        if (strcmp(pdebug, "DEBUG") == 0) {
          cmd->debug = true;
          debug_idx++;
        } else {
          err_cat_unrecognized_param(cmd->error_msg, sizeof(cmd->error_msg), "ROUTE IP", tokens[debug_idx]);
          return false;
        }
      }

      if (debug_idx < ntokens) {
        err_cat_extra_params(cmd->error_msg, sizeof(cmd->error_msg), "ROUTE IP");
        return false;
      }

      snprintf(cmd->ip, sizeof(cmd->ip), "%.63s", ip_val);
      cmd->is_valid = true;
      return true;
    }
  }

  err_cat_unknown_primitive(cmd->error_msg, sizeof(cmd->error_msg), tokens[0]);
  return false;
}

/* Compatibilidad con la interfaz de server.h/main.c: realiza el parsing y reporta el resultado de la validacion */
void handle_command(const char *input, char *output, size_t max_len) {
  if (!output || max_len == 0) return;

  parsed_command_t cmd;
  parse_line(input, &cmd);

  if (!cmd.is_valid) {
    snprintf(output, max_len, "%s", cmd.error_msg);
  } else {
    snprintf(output, max_len, "PARSED OK: primitiva %d validada", (int)cmd.primitive);
  }
}
