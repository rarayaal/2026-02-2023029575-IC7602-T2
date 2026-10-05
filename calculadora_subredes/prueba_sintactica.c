#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    const char *id;
    const char *categoria;
    const char *input;
    bool expected_valid;
    const char *motivo;
} test_case_t;

static test_case_t test_battery[] = {
    /* ========================================================================= */
    /* CATEGORÍA A: COMANDOS VÁLIDOS (T14-01 a T14-25)                           */
    /* ========================================================================= */
    {"T14-01", "Válida", "GET BROADCAST IP 192.168.1.10 MASK /24", true, "Sintaxis GET BROADCAST con IP y máscara CIDR válidas"},
    {"T14-02", "Válida", "GET BROADCAST IP 10.0.0.5 MASK 255.255.255.0", true, "Sintaxis GET BROADCAST con IP y máscara decimal válidas"},
    {"T14-03", "Válida", "GET BROADCAST IP 172.16.0.1 MASK /16", true, "Sintaxis GET BROADCAST con IP de Clase B y prefijo /16"},
    {"T14-04", "Válida", "GET NETWORK NUMBER IP 192.168.1.50 MASK /24", true, "Sintaxis GET NETWORK NUMBER con IP y CIDR válidos"},
    {"T14-05", "Válida", "GET NETWORK NUMBER IP 10.10.10.10 MASK 255.255.0.0", true, "Sintaxis GET NETWORK NUMBER con máscara decimal contigua"},
    {"T14-06", "Válida", "GET HOSTS RANGE IP 192.168.1.10 MASK /24", true, "Sintaxis GET HOSTS RANGE con parámetros válidos"},
    {"T14-07", "Válida", "GET HOSTS RANGE IP 172.16.100.5 MASK 255.255.240.0", true, "Sintaxis GET HOSTS RANGE con máscara subneteada"},
    {"T14-08", "Válida", "GET NETMASK MASK /24", true, "Sintaxis GET NETMASK con palabra clave MASK y CIDR"},
    {"T14-09", "Válida", "GET NETMASK MASK 255.255.255.0", true, "Sintaxis GET NETMASK con palabra clave MASK y formato decimal"},
    {"T14-10", "Válida", "GET NETMASK /16", true, "Sintaxis abreviada GET NETMASK con prefijo directo"},
    {"T14-11", "Válida", "GET HOSTS COUNT MASK /24", true, "Sintaxis GET HOSTS COUNT con MASK CIDR"},
    {"T14-12", "Válida", "GET HOSTS COUNT /29", true, "Sintaxis abreviada GET HOSTS COUNT con prefijo CIDR"},
    {"T14-13", "Válida", "GET HOSTS COUNT MASK 255.255.255.224", true, "Sintaxis GET HOSTS COUNT con máscara decimal"},
    {"T14-14", "Válida", "GET IP CLASS 10.8.2.5", true, "Sintaxis GET IP CLASS con dirección IPv4 directa"},
    {"T14-15", "Válida", "GET IP CLASS IP 172.16.0.1", true, "Sintaxis GET IP CLASS con prefijo de palabra clave IP"},
    {"T14-16", "Válida", "GET IP CLASS 192.168.1.1", true, "Sintaxis GET IP CLASS para IPv4 Clase C"},
    {"T14-17", "Válida", "SET ROUTE casa DEST 192.168.1.0 MASK /24", true, "Sintaxis SET ROUTE con nombre, destino y máscara sin prioridad opcional"},
    {"T14-18", "Válida", "SET ROUTE oficina DEST 10.0.0.0 MASK 255.0.0.0 PRIORITY 10", true, "Sintaxis SET ROUTE con parámetro opcional PRIORITY"},
    {"T14-19", "Válida", "SET ROUTE r3 DEST 172.16.0.0 MASK /12 PRIORITY 1", true, "Sintaxis SET ROUTE con prioridad de alto nivel 1"},
    {"T14-20", "Válida", "DEL ROUTE casa", true, "Sintaxis DEL ROUTE con nombre de ruta válido"},
    {"T14-21", "Válida", "DEL ROUTE oficina_remota", true, "Sintaxis DEL ROUTE con guion bajo en nombre"},
    {"T14-22", "Válida", "GET RANDOM SUBNETS IP 10.0.0.0 MASK /16 SIZE 4", true, "Sintaxis GET RANDOM SUBNETS con IP, máscara y tamaño entero positivo"},
    {"T14-23", "Válida", "GET RANDOM SUBNETS IP 192.168.0.0 MASK 255.255.0.0 SIZE 10", true, "Sintaxis GET RANDOM SUBNETS con máscara decimal"},
    {"T14-24", "Válida", "ROUTE IP 8.8.8.8", true, "Sintaxis ROUTE IP con dirección de destino"},
    {"T14-25", "Válida", "ROUTE IP 1.1.1.1 DEBUG", true, "Sintaxis ROUTE IP con modificador de depuración DEBUG"},

    /* ========================================================================= */
    /* CATEGORÍA B: COMBINACIONES INVÁLIDAS (T14-26 a T14-55)                    */
    /* ========================================================================= */
    {"T14-26", "Inválida", "INVALID COMMAND HERE", false, "Comando totalmente desconocido"},
    {"T14-27", "Inválida", "GET BROADCAST", false, "Primitiva incompleta sin parámetros IP ni MASK"},
    {"T14-28", "Inválida", "GET NETWORK", false, "Primitiva GET NETWORK incompleta"},
    {"T14-29", "Inválida", "GET HOSTS", false, "Primitiva GET HOSTS incompleta"},
    {"T14-30", "Inválida", "SET ROUTE", false, "Primitiva SET ROUTE sin argumentos"},
    {"T14-31", "Inválida", "GET BROADCAST IP 192.168.1.10", false, "Falta el parámetro obligatorio MASK"},
    {"T14-32", "Inválida", "GET BROADCAST MASK /24", false, "Falta el parámetro obligatorio IP"},
    {"T14-33", "Inválida", "GET NETWORK NUMBER MASK /24", false, "Falta la IP de red obligatoria"},
    {"T14-34", "Inválida", "GET HOSTS RANGE IP 192.168.1.1", false, "Falta la máscara obligatoria"},
    {"T14-35", "Inválida", "SET ROUTE casa MASK /24", false, "Falta el parámetro DEST en SET ROUTE"},
    {"T14-36", "Inválida", "SET ROUTE casa DEST 192.168.1.0", false, "Falta la MASK en SET ROUTE"},
    {"T14-37", "Inválida", "DEL ROUTE", false, "Falta el nombre de la ruta a eliminar"},
    {"T14-38", "Inválida", "GET RANDOM SUBNETS IP 10.0.0.0 MASK /16", false, "Falta el parámetro SIZE obligatorio"},
    {"T14-39", "Inválida", "ROUTE", false, "Falta el parámetro IP en comando ROUTE"},
    {"T14-40", "Inválida", "GET BROADCAST IP 192.168.1.10 MASK /24 EXTRA", false, "Parámetros adicionales no permitidos al final"},
    {"T14-41", "Inválida", "DEL ROUTE casa EXTRA_ARG", false, "Argumento no permitido tras el nombre de la ruta"},
    {"T14-42", "Inválida", "GET BROADCAST MASK /24 IP 192.168.1.10", false, "Orden incorrecto: MASK antes de IP"},
    {"T14-43", "Inválida", "SET ROUTE DEST 192.168.1.0 MASK /24 casa", false, "Orden incorrecto: nombre de ruta posicionado tras MASK"},
    {"T14-44", "Inválida", "GET BROADCAST IP 999.168.1.10 MASK /24", false, "Formato IPv4 inválido: primer octeto 999 > 255"},
    {"T14-45", "Inválida", "GET BROADCAST IP 192.168.1.10.5 MASK /24", false, "Formato IPv4 malformado: posee 5 octetos"},
    {"T14-46", "Inválida", "GET BROADCAST IP 192.168.1 MASK /24", false, "Formato IPv4 incompleto: solo 3 octetos"},
    {"T14-47", "Inválida", "GET BROADCAST IP abc.def.ghi.jkl MASK /24", false, "IPv4 con caracteres alfabéticos"},
    {"T14-48", "Inválida", "GET NETMASK MASK /33", false, "Prefijo CIDR fuera de rango (33 > 32)"},
    {"T14-49", "Inválida", "GET NETMASK MASK /-1", false, "Prefijo CIDR negativo fuera de rango"},
    {"T14-50", "Inválida", "GET NETMASK MASK /abc", false, "Prefijo CIDR no numérico"},
    {"T14-51", "Inválida", "SET ROUTE casa DEST 10.0.0.0 MASK /8 PRIORITY diez", false, "Valor de PRIORITY no numérico"},
    {"T14-52", "Inválida", "GET RANDOM SUBNETS IP 10.0.0.0 MASK /16 SIZE cuatro", false, "Valor de SIZE no numérico"},
    {"T14-53", "Inválida", "GET RANDOM SUBNETS IP 10.0.0.0 MASK /16 SIZE 0", false, "Valor de SIZE debe ser mayor que 0"},
    {"T14-54", "Inválida", "GET RANDOM SUBNETS IP 10.0.0.0 MASK /16 SIZE -5", false, "Valor de SIZE negativo no permitido"},
    {"T14-55", "Inválida", "", false, "Entrada de comandos completamente vacía"},

    /* ========================================================================= */
    /* CATEGORÍA C: VALORES EXTREMOS (T14-56 a T14-75)                           */
    /* ========================================================================= */
    {"T14-56", "Extremo", "GET BROADCAST IP 0.0.0.0 MASK /0", true, "Límite inferior de IPv4 (0.0.0.0) y máscara CIDR /0"},
    {"T14-57", "Extremo", "GET BROADCAST IP 255.255.255.255 MASK /32", true, "Límite superior de IPv4 (255.255.255.255) y máscara /32"},
    {"T14-58", "Extremo", "GET NETWORK NUMBER IP 1.1.1.1 MASK /1", true, "IPv4 1.1.1.1 con prefijo CIDR mínimo /1"},
    {"T14-59", "Extremo", "GET HOSTS RANGE IP 254.254.254.254 MASK /31", true, "IPv4 254.254.254.254 con prefijo subred punto a punto /31"},
    {"T14-60", "Extremo", "GET NETMASK MASK /0", true, "Límite mínimo de máscara CIDR /0"},
    {"T14-61", "Extremo", "GET NETMASK MASK /32", true, "Límite máximo de máscara CIDR /32"},
    {"T14-62", "Extremo", "GET NETMASK MASK 0.0.0.0", true, "Máscara en formato decimal decimal 0.0.0.0"},
    {"T14-63", "Extremo", "GET NETMASK MASK 255.255.255.255", true, "Máscara decimal completa 255.255.255.255"},
    {"T14-64", "Extremo", "GET HOSTS COUNT MASK /0", true, "Cálculo de hosts para prefijo límite /0"},
    {"T14-65", "Extremo", "GET HOSTS COUNT MASK /32", true, "Cálculo de hosts para prefijo límite /32"},
    {"T14-66", "Extremo", "GET IP CLASS 0.0.0.0", true, "Clase de dirección IP límite inferior 0.0.0.0"},
    {"T14-67", "Extremo", "GET IP CLASS 255.255.255.255", true, "Clase de dirección IP límite superior 255.255.255.255"},
    {"T14-68", "Extremo", "SET ROUTE r1 DEST 0.0.0.0 MASK 0.0.0.0 PRIORITY 0", true, "Ruta por defecto 0.0.0.0/0 con prioridad 0"},
    {"T14-69", "Extremo", "SET ROUTE r2 DEST 255.255.255.255 MASK 255.255.255.255 PRIORITY 65535", true, "Ruta con IP destino y máscara límites y alta prioridad"},
    {"T14-70", "Extremo", "GET RANDOM SUBNETS IP 0.0.0.0 MASK /0 SIZE 1", true, "Generación de subredes con SIZE mínimo válido 1"},
    {"T14-71", "Extremo", "GET RANDOM SUBNETS IP 10.0.0.0 MASK /8 SIZE 65535", true, "Generación de subredes con SIZE límite superior 65535"},
    {"T14-72", "Extremo", "GET NETMASK MASK 255.0.255.0", false, "Máscara decimal discontinua no contigua (inválida extrema)"},
    {"T14-73", "Extremo", "GET BROADCAST IP 192.168.1.10 MASK /33", false, "Máscara CIDR /33 inmediatamente fuera de rango superior"},
    {"T14-74", "Extremo", "GET BROADCAST IP 256.0.0.1 MASK /24", false, "Octeto IPv4 256 inmediatamente fuera de rango superior"},
    {"T14-75", "Extremo", "ROUTE IP 0.0.0.0 DEBUG", true, "ROUTE IP con IP 0.0.0.0 y flag de depuración DEBUG"}
};

int main(void) {
    size_t total_cases = sizeof(test_battery) / sizeof(test_battery[0]);
    size_t pass_count = 0;
    size_t fail_count = 0;
    size_t valid_count = 0;
    size_t invalid_count = 0;
    size_t extreme_count = 0;

    printf("===============================================================================\n");
    printf("        BATERÍA DE PRUEBAS SINTÁCTICAS T-14 - CALCULADORA DE SUBREDES          \n");
    printf("===============================================================================\n\n");

    for (size_t i = 0; i < total_cases; i++) {
        test_case_t tc = test_battery[i];
        parsed_command_t cmd;
        parser_init_command(&cmd);

        bool parse_res = parse_line(tc.input, &cmd);
        bool test_passed = (parse_res == tc.expected_valid && cmd.is_valid == tc.expected_valid);

        if (strcmp(tc.categoria, "Válida") == 0) valid_count++;
        else if (strcmp(tc.categoria, "Inválida") == 0) invalid_count++;
        else if (strcmp(tc.categoria, "Extremo") == 0) extreme_count++;

        const char *status_str = test_passed ? "PASS" : "FAIL";
        const char *exp_str = tc.expected_valid ? "Aceptada" : "Rechazada";
        const char *got_str = cmd.is_valid ? "Aceptada" : "Rechazada";

        if (test_passed) {
            pass_count++;
            printf("[%s] PASS | Cat: %-8s | Exp: %-9s | Got: %-9s | Comando: \"%s\"\n",
                   tc.id, tc.categoria, exp_str, got_str, tc.input);
        } else {
            fail_count++;
            printf("[%s] FAIL | Cat: %-8s | Exp: %-9s | Got: %-9s | Comando: \"%s\"\n",
                   tc.id, tc.categoria, exp_str, got_str, tc.input);
            printf("       -> Motivo de fallo: %s\n", tc.motivo);
            if (strlen(cmd.error_msg) > 0) {
                printf("       -> Error catalogado: %s\n", cmd.error_msg);
            }
        }
    }

    printf("\n===============================================================================\n");
    printf("                           RESUMEN DE RESULTADOS T-14                           \n");
    printf("===============================================================================\n");
    printf("Pruebas Válidas:    %zu\n", valid_count);
    printf("Pruebas Inválidas:  %zu\n", invalid_count);
    printf("Pruebas Extremas:   %zu\n", extreme_count);
    printf("Total de Pruebas:   %zu\n", total_cases);
    printf("PASS:               %zu\n", pass_count);
    printf("FAIL:               %zu\n", fail_count);
    printf("===============================================================================\n");

    return (fail_count == 0) ? 0 : 1;
}
