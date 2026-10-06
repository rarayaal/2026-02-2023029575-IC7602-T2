# Reporte Semanal de Avance – Tarea Corta 2

**Curso:** IC7602

**TAREA CORTA:** 2

**Semana:** 2

**Fecha del reporte:** 01/10/2026


## 1. Integrantes del grupo

| Carné      | Nombre completo          |
| ---------- | ------------------------ |
| 2020030408 | Luis Diego Delgado Muñoz |
| 2026801601 | Alexandra De Gracia      |
| 2023029575 | Rafael Araya Álvarez     |
| 2022104884 | Isaac Rojas Robles       |
| 2020426226 | Otto Segura Ruiz         |

---

## 2. Estado de las tareas

El grupo se organiza con un tablero de Jira, propuesto y creado por Otto Segura, en el que cada
tarea de este reporte corresponde a un ticket. Las descripciones completas de cada tarea
están en el reporte de la semana 1.

### Infraestructura de sockets TCP y servidor Telnet (Luis Delgado)

| ID   | Tarea                                 | Estado      | Avance | Problemas |
| ---- | ------------------------------------- | ----------- | -----: | --------- |
| T-01 | Servidor TCP en el puerto 9666        | Finalizada  |   100% | Ninguno |
| T-02 | Concurrencia de clientes              | Finalizada  |   100% | Ninguno |
| T-03 | Buffer de entrada y saneamiento       | Finalizada  |   100% | Ninguno |
| T-04 | Ensamblado de `main.c`                | En progreso |    70% | P-02 |
| T-05 | Makefile del proyecto                 | En progreso |    60% | P-01 |
| T-06 | Manejo de desconexiones y errores     | Finalizada  |   100% | Ninguno |
| T-07 | Pruebas de estabilidad y concurrencia | En progreso |    50% | Ninguno |

El servidor escucha en el puerto 9666 y atiende hasta 64 clientes simultáneos con `poll()` en un
solo hilo y sockets no bloqueantes. El saneamiento de la entrada descarta la negociación Telnet
(bytes IAC) y los caracteres de control, acepta `\r\n`, `\n` y `\r\0` como fin de línea y rechaza
con un mensaje de error las líneas que exceden el tamaño máximo, sin cerrar la sesión. El servidor
se detiene de forma ordenada con `SIGINT` y `SIGTERM`.

T-01, T-02, T-03 y T-06 están implementadas, probadas y ya integradas en `main`. `main.c` despacha
cada comando a un `handle_command` provisional mientras se integra el parser, por lo que cualquier
comando responde por ahora que no está implementado.

Se probaron clientes ociosos, comandos fragmentados en varios envíos, cierres abruptos del cliente y más conexiones que el máximo
permitido, verificando que al final no queden descriptores abiertos. Esas pruebas se hicieron con
`nc`; falta repetirlas con `telnet` y documentarlas, que es lo que pide T-07.

### Parser del protocolo y validación de sintaxis (Alexandra De Gracia)

| ID   | Tarea                                | Estado    | Avance | Problemas |
| ---- | ------------------------------------ | --------- | -----: | --------- |
| T-08 | Tokenizador de primitivas            | Pendiente |     0% | Ninguno |
| T-09 | Conversión y validación de IPv4      | Pendiente |     0% | P-03 |
| T-10 | Conversión y validación de máscaras  | Pendiente |     0% | P-03 |
| T-11 | Extracción de parámetros por comando | Pendiente |     0% | Ninguno |
| T-12 | Catálogo de mensajes de error        | Pendiente |     0% | Ninguno |
| T-13 | Despacho a los módulos de cálculo    | Pendiente |     0% | P-02 |
| T-14 | Pruebas sintácticas                  | Pendiente |     0% | Ninguno |

### Cálculo de subredes, primitivas 1 a 6 (Rafael Araya)

| ID   | Tarea                                  | Estado      | Avance | Problemas |
| ---- | -------------------------------------- | ----------- | -----: | --------- |
| T-15 | Interfaz pública del módulo            | Finalizada  |   100% | P-02 |
| T-16 | `GET BROADCAST` y `GET NETWORK NUMBER` | Finalizada  |   100% | Ninguno |
| T-17 | `GET HOSTS RANGE`                      | Finalizada  |   100% | Ninguno |
| T-18 | `GET NETMASK`                          | En progreso |    90% | P-03 |
| T-19 | `GET HOSTS COUNT`                      | Finalizada  |   100% | Ninguno |
| T-20 | `GET IP CLASS`                         | Finalizada  |   100% | Ninguno |
| T-21 | Pruebas unitarias del módulo           | En progreso |    60% | P-01 |

El módulo de cálculo ya está en `main` con las seis primitivas implementadas mediante operadores
bitwise, desacoplado de los sockets, y con una prueba unitaria por primitiva que reproduce los
ejemplos del enunciado. Las seis pruebas pasan. El encabezado se publicó como `calculo_subredes.h`
en lugar de `subnet_calc.h`.

Falta documentar los casos de prueba y corregir el objetivo `make test`, que hoy no los ejecuta
(P-01).

### Subredes aleatorias y motor de enrutamiento, primitivas 7 a 10 (Isaac Rojas)

| ID   | Tarea                                | Estado      | Avance | Problemas |
| ---- | ------------------------------------ | ----------- | -----: | --------- |
| T-22 | Estructura de la tabla de rutas      | Finalizada  |   100% | P-02 |
| T-23 | `SET ROUTE` y `DEL ROUTE`            | En progreso |    50% | Ninguno |
| T-24 | `GET RANDOM SUBNETS`                 | Pendiente   |     0% | Ninguno |
| T-25 | `ROUTE IP` — Longest Prefix Match    | En progreso |    50% | Ninguno |
| T-26 | `ROUTE IP` — desempate por prioridad | En progreso |    50% | Ninguno |
| T-27 | Modo `DEBUG`                         | Pendiente   |     0% | Ninguno |
| T-28 | Matriz de pruebas de enrutamiento    | Pendiente   |     0% | Ninguno |

El contrato público del módulo quedó diseñado en `routing.h`: la tabla de rutas como tipo opaco, los
códigos de estado y las firmas para insertar o actualizar una ruta, eliminarla por nombre y buscar
la mejor coincidencia para una IP según prefijo y prioridad.

### Automatización, Docker, Kubernetes y Helm (Otto Segura)

| ID   | Tarea                              | Estado    | Avance | Problemas |
| ---- | ---------------------------------- | --------- | -----: | --------- |
| T-29 | Dockerfile multi-stage             | Pendiente |     0% | Ninguno |
| T-30 | `Chart.yaml` y `values.yaml`       | Pendiente |     0% | Ninguno |
| T-31 | Plantilla `deployment.yaml`        | Pendiente |     0% | Ninguno |
| T-32 | Plantilla `service.yaml`           | Pendiente |     0% | Ninguno |
| T-33 | Scripts de automatización          | Pendiente |     0% | Ninguno |
| T-34 | Pruebas de extremo a extremo       | Pendiente |     0% | Ninguno |
| T-35 | Tabla de estado de funcionalidades | Pendiente |     0% | Ninguno |

### Tareas transversales

| ID   | Tarea                            | Estado      | Avance | Problemas |
| ---- | -------------------------------- | ----------- | -----: | --------- |
| T-36 | Documentación técnica por módulo | En progreso |    10% | Ninguno |
| T-37 | Conclusiones y recomendaciones   | Pendiente   |     0% | Ninguno |
| T-38 | Compilación del documento maestro| Pendiente   |     0% | Ninguno |
| T-39 | Reportes de avance               | En progreso |    50% | Ninguno |

El `README.md` describe las primitivas 1 a 6 y cómo ejecutar sus pruebas unitarias.

**Estados permitidos:** Pendiente / En progreso / Bloqueada / Finalizada.

---

## 3. Commits realizados

Período del 25/09/2026 al 01/10/2026. No se incluyen los commits de merge.

| Integrante | Fecha | Commit | Descripción | Archivos principales modificados |
| ---------- | ---------- | --------- | ----------- | -------------------------------- |
| Rafael Araya | 01/10/2026 | `427bbd8` | README inicial con los integrantes | `README.md` |
| Rafael Araya | 01/10/2026 | `4162f57` | Módulo de cálculo de subredes (primitivas 1 a 6), pruebas unitarias y Makefile inicial | `calculadora_subredes/calculo_subredes.c`, `calculadora_subredes/calculo_subredes.h`, `calculadora_subredes/prueba_calculo_subredes.c`, `Makefile` |
| Isaac Rojas | 01/10/2026 | `e1dae6f` | Contrato público del módulo de enrutamiento | `calculadora_subredes/routing.h` |
| Luis Delgado | 01/10/2026 | `4f2c569` | Reporte de avance de la semana 1 | `docs/ReporteAvance-Sem1-T2.md`, `docs/ReporteAvance-Sem1-T2.pdf` |
| Luis Delgado | 01/10/2026 | `3f045e3` | Servidor TCP concurrente con `poll` en el puerto 9666 | `calculadora_subredes/server.c`, `calculadora_subredes/server.h` |
| Luis Delgado | 01/10/2026 | `fafc1cc` | `main` con despacho a `handle_command` y parser provisional | `calculadora_subredes/main.c`, `calculadora_subredes/parser_stub.c` |
| Luis Delgado | 01/10/2026 | `0757220` | Pruebas unitarias del saneamiento Telnet | `calculadora_subredes/prueba_server.c` |
| Luis Delgado | 01/10/2026 | `f5a586b` | Objetivos del servidor en el Makefile | `Makefile`, `.gitignore` |

Alexandra De Gracia y Otto Segura no realizaron commits durante este período.

---

## 4. Pull Requests (PRs)

| Integrante | PR | Descripción | Estado | Tareas relacionadas |
| ---------- | ----- | ----------- | ------ | ------------------- |
| Rafael Araya | PR #1 | Módulo de cálculo de subredes con las primitivas 1 a 6 y sus pruebas unitarias | Mergeado | T-15 a T-21 |
| Luis Delgado | PR #2 | Servidor TCP concurrente con `poll`, saneamiento Telnet y pruebas unitarias | Mergeado | T-01 a T-07 |

Isaac Rojas, Alexandra De Gracia y Otto Segura no crearon ningún Pull Request durante este período.
El contrato de Isaac Rojas está en la rama `Isaac`.

---

## 5. Problemas

| ID   | Problema / Bloqueo | Tarea afectada | Responsable | Acción tomada / Próximo paso |
| ---- | ------------------ | -------------- | ----------- | ---------------------------- |
| P-01 | `make test` no ejecuta las pruebas del módulo de subredes: depende de un binario dentro de `calculadora_subredes/`, pero este se genera en la raíz, y las reglas de los `.o` no coinciden con las rutas reales. Las pruebas pasan al compilarlas manualmente | T-05, T-21 | Rafael Araya, Luis Delgado | Se le comunicó a Rafael. Corregir las reglas y agregar el binario de pruebas al `.gitignore` |
| P-02 | Los contratos entre módulos todavía no están cerrados en `main`: `routing.h` está en la rama `Isaac`, el encabezado de subredes se llama `calculo_subredes.h` y no `subnet_calc.h` como se planificó, y falta confirmar el tipo de retorno de `handle_command` | T-04, T-13, T-15, T-22 | Isaac Rojas, Rafael Araya, Alexandra De Gracia, Luis Delgado | Integrar `routing.h` a `main`, mantener el nombre `calculo_subredes.h` y confirmar la firma que usa el servidor |
| P-03 | Las funciones de conversión del módulo de subredes no informan errores: `ip_a_entero` devuelve 0 ante una entrada inválida, que no se distingue de `0.0.0.0`, y acepta octetos mayores a 255; `convertir_mascara_red` acepta máscaras no contiguas | T-09, T-10, T-18 | Alexandra De Gracia, Rafael Araya | Definir si el parser valida antes de llamar al módulo o si las funciones devuelven un código de estado |

---

## 6. Resumen de participación individual

| Integrante | Tareas asignadas | Commits | PRs | Resumen del trabajo realizado |
| ---------- | ---------------: | ------: | --: | ----------------------------- |
| Luis Diego Delgado Muñoz | 7 | 5 | 1 | Servidor TCP concurrente con `poll`, saneamiento de la entrada Telnet, `main` con despacho provisional, pruebas unitarias del servidor, objetivos del Makefile y reportes de avance |
| Alexandra De Gracia | 7 | 0 | 0 | Sin commits ni Pull Requests durante este período |
| Rafael Araya Álvarez | 7 | 2 | 1 | Creación del repositorio y módulo de cálculo de subredes con las primitivas 1 a 6, sus pruebas unitarias, el Makefile inicial y el README |
| Isaac Rojas Robles | 7 | 1 | 0 | Diseño del contrato público del módulo de enrutamiento, que cubre las primitivas 8, 9 y 10 |
| Otto Segura Ruiz | 7 | 0 | 0 | Propuso y creó el tablero de Jira con el que el grupo organiza las tareas. Sin commits ni Pull Requests durante este período |

Las tareas T-36 a T-39 son responsabilidad compartida y no se contabilizan en la columna de tareas
asignadas.

---

## 7. Plan para la siguiente semana

| ID           | Tarea / Actividad | Responsable(s) | Resultado esperado |
| ------------ | ----------------- | -------------- | ------------------ |
| T-04, T-05, T-07 | Integrar el parser al servidor y cerrar sus pruebas | Luis Delgado | `main.c` despacha al parser real, compilación con `make` validada y pruebas con `telnet` documentadas |
| T-08 a T-14  | Implementar el parser | Alexandra De Gracia | Las diez primitivas tokenizadas y validadas, con mensajes de error descriptivos, integradas con el servidor |
| P-01, T-18, T-21 | Corregir el Makefile y cerrar el módulo de subredes | Rafael Araya | `make test` ejecuta las pruebas y los casos quedan documentados |
| P-02, P-03   | Cerrar los contratos entre módulos y definir dónde se validan las entradas | Alexandra, Rafael, Isaac, Luis | `routing.h` en `main` y firmas acordadas |
| T-23 a T-28  | Implementar las primitivas 7 a 10 | Isaac Rojas | Tabla de rutas, subredes aleatorias, Longest Prefix Match con prioridad y modo debug, con sus pruebas |
| T-29 a T-35  | Construir la imagen y el Helm Chart | Otto Segura | El servidor se despliega en Kubernetes con un único comando |
| T-36 a T-38  | Redactar y compilar la documentación | Todos | Documento en PDF con instrucciones, pruebas, tabla de funcionalidades, diez conclusiones y diez recomendaciones |

---

## 8. Observaciones

- El servidor ya permite probar por `telnet` cualquier módulo que se integre: basta con que el parser
implemente `handle_command`, y el Makefile enlaza un parser provisional mientras tanto.

- Se eligió `poll()` en un solo hilo como modelo de concurrencia. Al no haber hilos, la tabla de rutas
que comparten todas las sesiones no necesita mecanismos de sincronización.

- El servidor termina cada respuesta con `\r\n`. El formato exacto de salida de cada primitiva,
incluido el separador de `GET RANDOM SUBNETS`, queda por definir en el parser.

- El grupo acordó que Luis Delgado consolida y envía todos los reportes de avance, en lugar de la
rotación indicada en el primer reporte.
