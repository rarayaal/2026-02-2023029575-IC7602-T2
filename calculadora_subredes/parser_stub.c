#include <stdio.h>

/* handle_command provisional para poder probar el servidor por telnet antes de
 * que el parser (P2) este integrado. El Makefile deja de enlazar este archivo
 * en cuanto exista parser.c. */
void handle_command(const char *input, char *output, size_t max_len) {
  snprintf(output, max_len, "ERROR: comando no implementado todavia: %s",
           input);
}
