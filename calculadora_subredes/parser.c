#include "parser.h"
#include "error_catalog.h"
#include "calculo_subredes.h"
#include "routing.h"
#include "dispatcher.h"

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

    /* 9. GET RANDOM SUBNETS con sintaxis compatible y sintaxis completa. */
    if (strcmp(t1, "RANDOM") == 0) {
      if (ntokens < 3) {
        err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "GET RANDOM");
        return false;
      }
      char t2[MAX_TOKEN_LEN];
      snprintf(t2, sizeof(t2), "%s", tokens[2]); str_to_upper(t2);

      if (strcmp(t2, "SUBNETS") == 0) {
        cmd->primitive = PRIMITIVE_GET_RANDOM_SUBNETS;
        if (ntokens == 9) {
          char p3[MAX_TOKEN_LEN], p5[MAX_TOKEN_LEN], p7[MAX_TOKEN_LEN];
          snprintf(p3, sizeof(p3), "%s", tokens[3]); str_to_upper(p3);
          snprintf(p5, sizeof(p5), "%s", tokens[5]); str_to_upper(p5);
          snprintf(p7, sizeof(p7), "%s", tokens[7]); str_to_upper(p7);

          if (strcmp(p3, "IP") != 0 || strcmp(p5, "MASK") != 0 ||
              strcmp(p7, "SIZE") != 0 || !validar_ip(tokens[4]) ||
              !validar_mascara(tokens[6]) || !es_numero(tokens[8])) {
            err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg),
                                   "GET RANDOM SUBNETS");
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

        if (ntokens != 12) {
          err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg),
                                "GET RANDOM SUBNETS",
                                "requiere NETWORK NUMBER <IP> MASK <MASK> NUMBER <NUM> SIZE <MASK>");
          return false;
        }

        char p3[MAX_TOKEN_LEN], p4[MAX_TOKEN_LEN], p6[MAX_TOKEN_LEN];
        char p8[MAX_TOKEN_LEN], p10[MAX_TOKEN_LEN];
        snprintf(p3, sizeof(p3), "%s", tokens[3]); str_to_upper(p3);
        snprintf(p4, sizeof(p4), "%s", tokens[4]); str_to_upper(p4);
        snprintf(p6, sizeof(p6), "%s", tokens[6]); str_to_upper(p6);
        snprintf(p8, sizeof(p8), "%s", tokens[8]); str_to_upper(p8);
        snprintf(p10, sizeof(p10), "%s", tokens[10]); str_to_upper(p10);

        if (strcmp(p3, "NETWORK") != 0 || strcmp(p4, "NUMBER") != 0 ||
            strcmp(p6, "MASK") != 0 || strcmp(p8, "NUMBER") != 0 ||
            strcmp(p10, "SIZE") != 0 || !validar_ip(tokens[5]) ||
            !validar_mascara(tokens[7]) || !es_numero(tokens[9]) ||
            !validar_mascara(tokens[11])) {
          err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg),
                                 "GET RANDOM SUBNETS");
          return false;
        }
        int size_val = atoi(tokens[9]);
        if (size_val <= 0) {
          err_cat_out_of_range(cmd->error_msg, sizeof(cmd->error_msg),
                               "el numero de redes debe ser mayor a 0", tokens[9]);
          return false;
        }

        snprintf(cmd->ip, sizeof(cmd->ip), "%.63s", tokens[5]);
        snprintf(cmd->mask, sizeof(cmd->mask), "%.63s", tokens[7]);
        snprintf(cmd->subnet_mask, sizeof(cmd->subnet_mask), "%.63s", tokens[11]);
        cmd->size = size_val;
        cmd->has_size = true;
        cmd->has_subnet_mask = true;
        cmd->is_valid = true;
        return true;
      }
    }
  }

  /* 7. SET ROUTE <NAME> NETWORK NUMBER <NET> MASK <MASK> PRIORITY <NUM>
     o  SET ROUTE <NAME> DEST <NET> MASK <MASK> PRIORITY <NUM> */
  if (strcmp(t0, "SET") == 0) {
    if (ntokens < 2) {
      err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "comando SET");
      return false;
    }
    char t1[MAX_TOKEN_LEN];
    snprintf(t1, sizeof(t1), "%s", tokens[1]); str_to_upper(t1);

    if (strcmp(t1, "ROUTE") == 0) {
      cmd->primitive = PRIMITIVE_SET_ROUTE;
      if (ntokens < 3) {
        err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg),
                              "SET ROUTE", "nombre");
        return false;
      }

      size_t ip_idx = 0;
      size_t mask_kw_idx = 0;
      size_t mask_val_idx = 0;
      size_t prio_kw_idx = 0;
      size_t prio_val_idx = 0;

      char p3[MAX_TOKEN_LEN];
      snprintf(p3, sizeof(p3), "%s", tokens[3]); str_to_upper(p3);

      if (strcmp(p3, "NETWORK") == 0) {
        if (ntokens < 5) {
          err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg),
                                "SET ROUTE", "NETWORK NUMBER");
          return false;
        }
        char p4[MAX_TOKEN_LEN];
        snprintf(p4, sizeof(p4), "%s", tokens[4]); str_to_upper(p4);
        if (strcmp(p4, "NUMBER") != 0) {
          err_cat_expected_keyword(cmd->error_msg, sizeof(cmd->error_msg), "SET ROUTE", "NUMBER");
          return false;
        }
        ip_idx = 5;
        mask_kw_idx = 6;
        mask_val_idx = 7;
        prio_kw_idx = 8;
        prio_val_idx = 9;
      } else if (strcmp(p3, "DEST") == 0 || strcmp(p3, "DESTINATION") == 0 || strcmp(p3, "NET") == 0) {
        ip_idx = 4;
        mask_kw_idx = 5;
        mask_val_idx = 6;
        prio_kw_idx = 7;
        prio_val_idx = 8;
      } else {
        err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "SET ROUTE");
        return false;
      }

      if (ntokens <= prio_kw_idx) {
        if (ntokens <= mask_val_idx) {
          err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg), "SET ROUTE", "MASK");
        } else {
          err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg), "SET ROUTE", "PRIORITY");
        }
        return false;
      }

      if (ntokens > prio_val_idx + 1) {
        err_cat_extra_params(cmd->error_msg, sizeof(cmd->error_msg), "SET ROUTE");
        return false;
      }

      char pmask_kw[MAX_TOKEN_LEN];
      snprintf(pmask_kw, sizeof(pmask_kw), "%s", tokens[mask_kw_idx]); str_to_upper(pmask_kw);
      if (strcmp(pmask_kw, "MASK") != 0) {
        err_cat_expected_keyword(cmd->error_msg, sizeof(cmd->error_msg), "SET ROUTE", "MASK");
        return false;
      }

      if (!validar_ip(tokens[ip_idx])) {
        err_cat_invalid_ip(cmd->error_msg, sizeof(cmd->error_msg), tokens[ip_idx]);
        return false;
      }

      if (!validar_mascara(tokens[mask_val_idx])) {
        err_cat_invalid_mask(cmd->error_msg, sizeof(cmd->error_msg), tokens[mask_val_idx]);
        return false;
      }

      char pprio_kw[MAX_TOKEN_LEN];
      snprintf(pprio_kw, sizeof(pprio_kw), "%s", tokens[prio_kw_idx]); str_to_upper(pprio_kw);
      if (strcmp(pprio_kw, "PRIORITY") != 0) {
        err_cat_expected_keyword(cmd->error_msg, sizeof(cmd->error_msg), "SET ROUTE", "PRIORITY");
        return false;
      }

      if (!es_numero(tokens[prio_val_idx])) {
        err_cat_invalid_numeric(cmd->error_msg, sizeof(cmd->error_msg), "PRIORITY", tokens[prio_val_idx]);
        return false;
      }

      /* Preservar insensibilidad a mayusculas/minusculas para el nombre de ruta <NAME> */
      snprintf(cmd->name, sizeof(cmd->name), "%.63s", tokens[2]);
      snprintf(cmd->dest, sizeof(cmd->dest), "%.63s", tokens[ip_idx]);
      snprintf(cmd->mask, sizeof(cmd->mask), "%.63s", tokens[mask_val_idx]);
      cmd->priority = atoi(tokens[prio_val_idx]);
      cmd->has_priority = true;

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

  /* 10. ROUTE IP <IP> [DEBUG [true|false]] */
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

      /* DEBUG es opcional, por defecto false */
      cmd->debug = false;

      if (debug_idx < ntokens) {
        char pdebug[MAX_TOKEN_LEN];
        snprintf(pdebug, sizeof(pdebug), "%s", tokens[debug_idx]); str_to_upper(pdebug);
        if (strcmp(pdebug, "DEBUG") == 0) {
          debug_idx++;
          if (debug_idx < ntokens) {
            char pval[MAX_TOKEN_LEN];
            snprintf(pval, sizeof(pval), "%s", tokens[debug_idx]); str_to_upper(pval);
            if (strcmp(pval, "TRUE") == 0) {
              cmd->debug = true;
              debug_idx++;
            } else if (strcmp(pval, "FALSE") == 0) {
              cmd->debug = false;
              debug_idx++;
            } else {
              err_cat_invalid_syntax(cmd->error_msg, sizeof(cmd->error_msg), "valor invalido para DEBUG (esperado true o false)");
              return false;
            }
          } else {
            /* DEBUG requiere obligatoriamente valor true o false cuando esta presente */
            err_cat_missing_param(cmd->error_msg, sizeof(cmd->error_msg), "ROUTE IP DEBUG", "valor booleano (true o false)");
            return false;
          }
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
    static route_table_t *route_table;
    uint32_t ip = 0;
    uint32_t network = 0;
    int prefix = -1;
    route_status_t status = ROUTE_STATUS_OK;

    switch (cmd.primitive) {
      case PRIMITIVE_SET_ROUTE:
        /* La tabla persiste entre comandos durante la vida del servidor. */
        if (!route_table) route_table = route_table_create();
        if (!route_table || !convertir_ip_a_entero(cmd.dest, &network) ||
            !parsear_mascara_a_cidr(cmd.mask, &prefix)) {
          snprintf(output, max_len, "ERROR: no se pudo preparar SET ROUTE");
          return;
        }
        status = route_set(route_table, cmd.name, network, prefix,
                           cmd.has_priority ? cmd.priority : 0);
        if (status == ROUTE_STATUS_OK) {
          snprintf(output, max_len, "OK: ruta %s configurada", cmd.name);
        } else {
          snprintf(output, max_len, "ERROR: no se pudo configurar la ruta");
        }
        return;

      case PRIMITIVE_DEL_ROUTE:
        if (!route_table) {
          snprintf(output, max_len, "ERROR: ruta no encontrada: %s", cmd.name);
          return;
        }
        status = route_delete(route_table, cmd.name);
        if (status == ROUTE_STATUS_OK) {
          snprintf(output, max_len, "OK: ruta %s eliminada", cmd.name);
        } else {
          snprintf(output, max_len, "ERROR: ruta no encontrada: %s", cmd.name);
        }
        return;

      case PRIMITIVE_ROUTE_IP: {
        route_match_t match;
        char trace[1024];
        if (!route_table || !convertir_ip_a_entero(cmd.ip, &ip)) {
          snprintf(output, max_len, "ERROR: no existe una ruta para %s", cmd.ip);
          return;
        }
        if (cmd.debug) {
          /* DEBUG conserva la misma seleccion y solo agrega su explicacion. */
          status = route_lookup_debug(route_table, ip, &match, trace,
                                      sizeof(trace));
          if (status == ROUTE_STATUS_OK) {
            snprintf(output, max_len, "%s%s", trace, match.name);
          } else {
            snprintf(output, max_len, "%s", trace);
          }
        } else {
          status = route_lookup(route_table, ip, &match);
          if (status == ROUTE_STATUS_OK) {
            snprintf(output, max_len, "%s", match.name);
          } else {
            snprintf(output, max_len, "ERROR: no existe una ruta para %s", cmd.ip);
          }
        }
        return;
      }

      case PRIMITIVE_GET_RANDOM_SUBNETS: {
        if (!cmd.has_subnet_mask ||
            !convertir_ip_a_entero(cmd.ip, &network) ||
            !parsear_mascara_a_cidr(cmd.mask, &prefix)) {
          snprintf(output, max_len,
                   "ERROR: GET RANDOM SUBNETS requiere SIZE <mascara>");
          return;
        }
        int subnet_prefix = -1;
        if (!parsear_mascara_a_cidr(cmd.subnet_mask, &subnet_prefix)) {
          snprintf(output, max_len, "ERROR: mascara de subred invalida");
          return;
        }
        route_subnet_t *subnets = calloc((size_t)cmd.size, sizeof(*subnets));
        if (!subnets) {
          snprintf(output, max_len, "ERROR: memoria insuficiente");
          return;
        }
        status = route_generate_random_subnets(network, prefix, subnet_prefix,
                                               (size_t)cmd.size, subnets);
        if (status != ROUTE_STATUS_OK) {
          free(subnets);
          snprintf(output, max_len,
                   "ERROR: no se pudieron generar las subredes");
          return;
        }
        size_t used = 0;
        output[0] = '\0';
        for (int index = 0; index < cmd.size; index++) {
          char subnet_ip[16];
          entero_a_ip(subnets[index].network, subnet_ip, sizeof(subnet_ip));
          int written = snprintf(output + used, max_len - used, "%s%s/%u",
                                 index == 0 ? "" : " ", subnet_ip,
                                 subnets[index].prefix);
          if (written < 0 || (size_t)written >= max_len - used) break;
          used += (size_t)written;
        }
        free(subnets);
        return;
      }

      default: {
        /* Primitivas 1 a 6: el despacho de T-13 llama al modulo de calculo. */
        extracted_params_t params;
        dispatch_result_t result;
        dispatch_result_init(&result);
        if (!extraer_parametros_comando(&cmd, &params)) {
          err_cat_invalid_syntax(result.error_msg, sizeof(result.error_msg),
                                 "extraccion de parametros");
        } else if (dispatch_extracted_params(&params, &result)) {
          snprintf(output, max_len, "%s", result.result_str);
          return;
        }
        snprintf(output, max_len, "%s", result.error_msg);
        return;
      }
    }
  }
}
