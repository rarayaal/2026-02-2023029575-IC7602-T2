#include "server.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Alimenta el lector con una secuencia de bytes y devuelve el estado que
 * produjo el ultimo byte. */
static line_status_t alimentar(line_reader_t *lector, const char *bytes,
                               size_t cantidad) {
  line_status_t estado = LINE_PENDING;
  for (size_t i = 0; i < cantidad; i++) {
    estado = line_reader_feed(lector, (unsigned char)bytes[i]);
  }
  return estado;
}

void probar_fin_de_linea(void) {
  line_reader_t lector;
  line_reader_init(&lector);

  /* \r\n de telnet: el \r cierra la linea y el \n no genera una vacia. */
  assert(alimentar(&lector, "GET IP CLASS 10.8.2.5\r", 22) == LINE_READY);
  assert(strcmp(lector.line, "GET IP CLASS 10.8.2.5") == 0);
  assert(alimentar(&lector, "\n", 1) == LINE_PENDING);

  /* \n solo, como lo envia nc. */
  assert(alimentar(&lector, "DEL ROUTE casa\n", 15) == LINE_READY);
  assert(strcmp(lector.line, "DEL ROUTE casa") == 0);

  /* \r\0, la otra forma valida en Telnet. */
  assert(alimentar(&lector, "ROUTE IP 8.8.8.8\r", 17) == LINE_READY);
  assert(alimentar(&lector, "\0", 1) == LINE_PENDING);
  assert(alimentar(&lector, "A\n", 2) == LINE_READY);
  assert(strcmp(lector.line, "A") == 0);

  /* Una linea vacia se reporta vacia; el servidor es quien la ignora. */
  assert(alimentar(&lector, "\n", 1) == LINE_READY);
  assert(lector.line[0] == '\0');

  printf("[OK] Prueba servidor: fin de linea\n");
}

void probar_comando_fragmentado(void) {
  line_reader_t lector;
  line_reader_init(&lector);

  assert(alimentar(&lector, "GET NETM", 8) == LINE_PENDING);
  assert(alimentar(&lector, "ASK MASK /27\r\n", 14) == LINE_PENDING);
  assert(strcmp(lector.line, "GET NETMASK MASK /27") == 0);

  printf("[OK] Prueba servidor: comando fragmentado\n");
}

void probar_negociacion_telnet(void) {
  line_reader_t lector;
  line_reader_init(&lector);

  /* IAC DO 1, IAC WILL 3, subnegociacion con un IAC IAC adentro e IAC NOP. */
  const char entrada[] = "\xFF\xFD\x01\xFF\xFB\x03"
                         "GET \xFF\xFA\x18\x01\xFF\xFFxyz\xFF\xF0"
                         "IP\xFF\xF1 CLASS 10.8.2.5\n";
  assert(alimentar(&lector, entrada, sizeof(entrada) - 1) == LINE_READY);
  assert(strcmp(lector.line, "GET IP CLASS 10.8.2.5") == 0);

  printf("[OK] Prueba servidor: negociacion Telnet\n");
}

void probar_caracteres_de_control(void) {
  line_reader_t lector;
  line_reader_init(&lector);

  const char entrada[] = "GET\tIP\x07 CL\x1B[AASS\x7F \xC3\xB1" "1.2.3.4\n";
  assert(alimentar(&lector, entrada, sizeof(entrada) - 1) == LINE_READY);
  assert(strcmp(lector.line, "GET IP CL[AASS 1.2.3.4") == 0);

  printf("[OK] Prueba servidor: caracteres de control\n");
}

void probar_linea_demasiado_larga(void) {
  line_reader_t lector;
  line_reader_init(&lector);

  for (size_t i = 0; i < SERVER_LINE_MAX + 100; i++) {
    assert(line_reader_feed(&lector, 'A') == LINE_PENDING);
  }
  assert(line_reader_feed(&lector, '\n') == LINE_TOO_LONG);

  /* El lector se recupera y la siguiente linea llega completa. */
  assert(alimentar(&lector, "GET HOSTS COUNT MASK /29\n", 25) == LINE_READY);
  assert(strcmp(lector.line, "GET HOSTS COUNT MASK /29") == 0);

  /* El maximo exacto (sin contar el nulo) todavia se acepta. */
  for (size_t i = 0; i < SERVER_LINE_MAX - 1; i++) {
    line_reader_feed(&lector, 'B');
  }
  assert(line_reader_feed(&lector, '\n') == LINE_READY);
  assert(strlen(lector.line) == SERVER_LINE_MAX - 1);

  printf("[OK] Prueba servidor: linea demasiado larga\n");
}

int main(void) {
  probar_fin_de_linea();
  probar_comando_fragmentado();
  probar_negociacion_telnet();
  probar_caracteres_de_control();
  probar_linea_demasiado_larga();
  printf("Todas las pruebas del servidor pasaron\n");
  return 0;
}
