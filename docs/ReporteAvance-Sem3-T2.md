# Reporte Semanal de Avance – Tarea Corta 2

**Curso:** IC7602

**TAREA CORTA:** 2

**Semana:** 3

**Fecha del reporte:** 08/10/2026


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

El avance se verificó contra la rama `main` al 08/10/2026: se compiló con `make`, se ejecutó
`make test` y se probaron las diez primitivas por `telnet` contra el servidor. Las descripciones
completas de cada tarea están en el reporte de la semana 1, y cada una corresponde a un ticket del
tablero de Jira del grupo.

### Infraestructura de sockets TCP y servidor Telnet (Luis Delgado)

| ID   | Tarea                                 | Estado      | Avance | Problemas |
| ---- | ------------------------------------- | ----------- | -----: | --------- |
| T-01 | Servidor TCP en el puerto 9666        | Finalizada  |   100% | Ninguno |
| T-02 | Concurrencia de clientes              | Finalizada  |   100% | Ninguno |
| T-03 | Buffer de entrada y saneamiento       | Finalizada  |   100% | Ninguno |
| T-04 | Ensamblado de `main.c`                | Finalizada  |   100% | P-04 |
| T-05 | Makefile del proyecto                 | Finalizada  |   100% | Ninguno |
| T-06 | Manejo de desconexiones y errores     | Finalizada  |   100% | Ninguno |
| T-07 | Pruebas de estabilidad y concurrencia | Finalizada  |   100% | Ninguno |

El servidor ya despacha al parser real. Al integrarlo se encontró que las primitivas 1 a 6
respondían `PARSED OK` en lugar del resultado del cálculo (P-04); se corrigió conectando
`handle_command` con el despacho de T-13, y ahora devuelven exactamente los ejemplos del enunciado.

`make`, `make test` y `make clean` funcionan sin advertencias. Las pruebas de estabilidad se
repitieron con `telnet`: cinco sesiones simultáneas recibieron cada una sus diez respuestas sin
mezclarse, y se probaron clientes ociosos, cierres abruptos, líneas demasiado largas, puerto ocupado
y detención con `SIGTERM`.

### Parser del protocolo y validación de sintaxis (Alexandra De Gracia)

| ID   | Tarea                                | Estado      | Avance | Problemas |
| ---- | ------------------------------------ | ----------- | -----: | --------- |
| T-08 | Tokenizador de primitivas            | En progreso |    90% | P-05 |
| T-09 | Conversión y validación de IPv4      | Finalizada  |   100% | Ninguno |
| T-10 | Conversión y validación de máscaras  | Finalizada  |   100% | Ninguno |
| T-11 | Extracción de parámetros por comando | Finalizada  |   100% | Ninguno |
| T-12 | Catálogo de mensajes de error        | Finalizada  |   100% | Ninguno |
| T-13 | Despacho a los módulos de cálculo    | Finalizada  |   100% | P-04 |
| T-14 | Pruebas sintácticas                  | En progreso |    90% | P-05 |

El parser reconoce las diez primitivas sin importar mayúsculas o minúsculas, valida IPs y máscaras
en ambas notaciones y responde mensajes de error específicos, por ejemplo
`ERROR: direccion IP invalida: 999.1.1.1` o
`ERROR: parametro faltante en GET BROADCAST (requiere IP <IP> MASK <MASK>)`. Cuenta con pruebas
unitarias por tarea y con una batería de pruebas sintácticas de comandos válidos, inválidos y
valores extremos.

Falta ajustar la sintaxis de `SET ROUTE` y del parámetro `DEBUG` a la del enunciado (P-05).

### Cálculo de subredes, primitivas 1 a 6 (Rafael Araya)

| ID   | Tarea                                  | Estado      | Avance | Problemas |
| ---- | -------------------------------------- | ----------- | -----: | --------- |
| T-15 | Interfaz pública del módulo            | Finalizada  |   100% | Ninguno |
| T-16 | `GET BROADCAST` y `GET NETWORK NUMBER` | Finalizada  |   100% | Ninguno |
| T-17 | `GET HOSTS RANGE`                      | Finalizada  |   100% | Ninguno |
| T-18 | `GET NETMASK`                          | Finalizada  |   100% | Ninguno |
| T-19 | `GET HOSTS COUNT`                      | Finalizada  |   100% | Ninguno |
| T-20 | `GET IP CLASS`                         | Finalizada  |   100% | Ninguno |
| T-21 | Pruebas unitarias del módulo           | En progreso |    80% | Ninguno |

Las seis primitivas responden correctamente por `telnet`. Las máscaras inválidas ya se rechazan
desde el parser, con lo que se cerró P-03, y `make test` ejecuta las pruebas del módulo (P-01). Falta
documentar los casos de prueba paso a paso.

### Subredes aleatorias y motor de enrutamiento, primitivas 7 a 10 (Isaac Rojas)

| ID   | Tarea                                | Estado      | Avance | Problemas |
| ---- | ------------------------------------ | ----------- | -----: | --------- |
| T-22 | Estructura de la tabla de rutas      | Finalizada  |   100% | Ninguno |
| T-23 | `SET ROUTE` y `DEL ROUTE`            | Finalizada  |   100% | P-05 |
| T-24 | `GET RANDOM SUBNETS`                 | Finalizada  |   100% | Ninguno |
| T-25 | `ROUTE IP` — Longest Prefix Match    | Finalizada  |   100% | Ninguno |
| T-26 | `ROUTE IP` — desempate por prioridad | Finalizada  |   100% | Ninguno |
| T-27 | Modo `DEBUG`                         | Finalizada  |   100% | P-05 |
| T-28 | Matriz de pruebas de enrutamiento    | Finalizada  |   100% | Ninguno |

La tabla de rutas es una lista enlazada en memoria dinámica. `ROUTE IP` selecciona la ruta con el
prefijo más específico y desempata por prioridad, y el modo debug muestra cada ruta evaluada, la
aplicación de la máscara y el criterio de selección. `GET RANDOM SUBNETS` genera subredes dentro del
bloque padre sin solapamientos.

La lógica está completa; lo que impide usar las primitivas 8 y 10 con la sintaxis del enunciado es
el reconocimiento del comando en el parser (P-05).

### Automatización, Docker, Kubernetes y Helm (Otto Segura)

| ID   | Tarea                              | Estado      | Avance | Problemas |
| ---- | ---------------------------------- | ----------- | -----: | --------- |
| T-29 | Dockerfile multi-stage             | Finalizada  |   100% | Ninguno |
| T-30 | `Chart.yaml` y `values.yaml`       | Finalizada  |   100% | Ninguno |
| T-31 | Plantilla `deployment.yaml`        | Finalizada  |   100% | Ninguno |
| T-32 | Plantilla `service.yaml`           | Finalizada  |   100% | Ninguno |
| T-33 | Scripts de automatización          | Finalizada  |   100% | Ninguno |
| T-34 | Pruebas de extremo a extremo       | Finalizada  |   100% | Ninguno |
| T-35 | Tabla de estado de funcionalidades | Finalizada  |   100% | Ninguno |

La imagen se construye en dos etapas sobre Alpine y ejecuta el servidor con un usuario sin
privilegios. El chart despliega el servidor con probes TCP sobre el puerto 9666 y lo expone con un
Service NodePort en el 30666. `scripts/deploy.sh` verifica las herramientas, levanta un clúster con
kind o minikube si no hay uno activo, construye la imagen, la carga al clúster e instala el chart;
`scripts/cleanup.sh` lo desinstala.

### Tareas transversales

| ID   | Tarea                             | Estado      | Avance | Problemas |
| ---- | --------------------------------- | ----------- | -----: | --------- |
| T-36 | Documentación técnica por módulo  | Finalizada  |   100% | Ninguno |
| T-37 | Conclusiones y recomendaciones    | Finalizada  |   100% | Ninguno |
| T-38 | Compilación del documento maestro | Finalizada  |   100% | Ninguno |
| T-39 | Reportes de avance                | En progreso |    75% | Ninguno |

El documento maestro integra la documentación de los cinco módulos, con las instrucciones de
ejecución y despliegue, las pruebas, la tabla de funcionalidades y las diez conclusiones y diez
recomendaciones.

**Estados permitidos:** Pendiente / En progreso / Bloqueada / Finalizada.

---

## 3. Commits realizados

Período del 01/10/2026, después del reporte anterior, al 08/10/2026. No se incluyen los commits de
merge.

| Integrante | Fecha | Commit | Descripción | Archivos principales modificados |
| ---------- | ---------- | --------- | ----------- | -------------------------------- |
| Luis Delgado | 01/10/2026 | `edfd182` | Reporte de avance de la semana 2 | `docs/ReporteAvance-Sem2-T2.md`, `docs/ReporteAvance-Sem2-T2.pdf` |
| Alexandra De Gracia | 03/10/2026 | `806f320` | Parser de las diez primitivas (T-08) | `calculadora_subredes/parser.c`, `calculadora_subredes/parser.h`, `calculadora_subredes/prueba_parser.c`, `Makefile` |
| Alexandra De Gracia | 03/10/2026 | `bfaa24d` | Validación y conversión de IPv4 (T-09) | `calculadora_subredes/parser.c`, `calculadora_subredes/parser.h`, `calculadora_subredes/prueba_parser.c` |
| Alexandra De Gracia | 03/10/2026 | `fecc6e3` | Validación de máscaras (T-10) y cierre de T-08 y T-09 | `calculadora_subredes/parser.c`, `calculadora_subredes/parser.h`, `calculadora_subredes/prueba_parser.c` |
| Otto Segura | 04/10/2026 | `194e4cb` | Dockerfile multi-stage y `.dockerignore` | `Dockerfile`, `.dockerignore` |
| Otto Segura | 05/10/2026 | `a81cf01` | Helm Chart de la calculadora | `deploy/helm/subnet-calculator/Chart.yaml`, `deploy/helm/subnet-calculator/values.yaml`, `deploy/helm/subnet-calculator/templates/deployment.yaml`, `deploy/helm/subnet-calculator/templates/service.yaml` |
| Otto Segura | 05/10/2026 | `9c81372` | Scripts de despliegue y limpieza | `scripts/deploy.sh`, `scripts/cleanup.sh` |
| Isaac Rojas | 05/10/2026 | `b4e6091` | Tabla de rutas como lista enlazada | `calculadora_subredes/routing.c`, `calculadora_subredes/prueba_routing.c` |
| Alexandra De Gracia | 05/10/2026 | `5a29410` | Extracción de parámetros por comando (T-11) | `calculadora_subredes/param_extractor.c`, `calculadora_subredes/param_extractor.h`, `calculadora_subredes/prueba_param_extractor.c` |
| Alexandra De Gracia | 05/10/2026 | `60cc0fa` | Catálogo de mensajes de error (T-12) | `calculadora_subredes/error_catalog.c`, `calculadora_subredes/error_catalog.h`, `calculadora_subredes/parser.c`, `calculadora_subredes/prueba_error_catalog.c` |
| Alexandra De Gracia | 05/10/2026 | `30006ec` | Despacho a los módulos de cálculo (T-13) | `calculadora_subredes/dispatcher.c`, `calculadora_subredes/dispatcher.h`, `calculadora_subredes/prueba_dispatcher.c`, `Makefile` |
| Alexandra De Gracia | 05/10/2026 | `255dfbc` | Pruebas sintácticas (T-14) | `calculadora_subredes/prueba_sintactica.c`, `Makefile` |
| Alexandra De Gracia | 05/10/2026 | `0005459` | Documentación de las tareas T-08 a T-14 | `calculadora_subredes/parser.h`, `calculadora_subredes/param_extractor.h`, `calculadora_subredes/dispatcher.h` |
| Otto Segura | 05/10/2026 | `48db9f9` | Tabla de estado de funcionalidades y corrección del helper del chart | `docs/tabla_funcionalidades.md`, `deploy/helm/subnet-calculator/templates/_helpers.tpl` |
| Isaac Rojas | 05/10/2026 | `907f6ce` | Primitivas de enrutamiento y subredes aleatorias integradas al parser | `calculadora_subredes/routing.c`, `calculadora_subredes/routing.h`, `calculadora_subredes/parser.c`, `calculadora_subredes/prueba_routing.c`, `Makefile` |
| Luis Delgado | 06/10/2026 | `ddbb151` | Conexión de las primitivas 1 a 6 al despacho de cálculo | `calculadora_subredes/parser.c`, `Makefile` |
| Luis Delgado | 06/10/2026 | `e396980` | Documentación del servidor TCP y pruebas con `telnet` | `docs/servidor.md` |

Rafael Araya no realizó commits durante este período.

---

## 4. Pull Requests (PRs)

| Integrante | PR | Descripción | Estado | Tareas relacionadas |
| ---------- | ----- | ----------- | ------ | ------------------- |
| Alexandra De Gracia | PR #3 | Parser de las diez primitivas con validación de IPv4 y máscaras | Mergeado | T-08 a T-10 |
| Alexandra De Gracia | PR #4 | Extracción de parámetros, catálogo de errores, despacho y pruebas sintácticas | Mergeado | T-11 a T-14 |
| Otto Segura | PR #5 | Dockerfile, Helm Chart, scripts de despliegue y tabla de funcionalidades | Mergeado | T-29 a T-33, T-35 |
| Isaac Rojas | PR #6 | Contrato y tabla de rutas, `SET`/`DEL ROUTE`, `ROUTE IP` con debug y subredes aleatorias | Mergeado | T-22 a T-28 |
| Luis Delgado | PR #7 | Conexión de las primitivas 1 a 6 al despacho, documentación del servidor y reporte de la semana 2 | Mergeado | T-04, T-05, T-07, T-36, T-37, T-39 |

Rafael Araya no creó Pull Requests durante este período.

---

## 5. Problemas

### Problemas de la semana anterior

| ID   | Problema | Estado |
| ---- | -------- | ------ |
| P-01 | `make test` no ejecutaba las pruebas del módulo de subredes | **Resuelto.** El Makefile se actualizó en el PR #6 y `make test` ejecuta las ocho suites de pruebas del proyecto |
| P-02 | Los contratos entre módulos no estaban cerrados en `main` | **Resuelto.** `routing.h` está en `main`, se mantuvo el nombre `calculo_subredes.h` y `handle_command` devuelve `void` en el servidor y en el parser |
| P-03 | Las funciones de conversión no informaban errores ante entradas inválidas | **Resuelto.** El parser valida IPs y máscaras antes de llamar al módulo de cálculo y responde un mensaje específico, por ejemplo ante `999.1.1.1` o `255.0.255.0` |

### Problemas nuevos

| ID   | Problema / Bloqueo | Tarea afectada | Responsable | Acción tomada / Próximo paso |
| ---- | ------------------ | -------------- | ----------- | ---------------------------- |
| P-04 | Al integrar los módulos, las primitivas 1 a 6 respondían `PARSED OK: primitiva N validada` en lugar del resultado, porque `handle_command` no llamaba al despacho de T-13 | T-04, T-13 | Luis Delgado | **Resuelto.** Se conectó `handle_command` con el despacho y se enlazaron sus módulos en el Makefile (PR #7) |
| P-05 | El parser espera `SET ROUTE <NAME> DEST <NET> MASK <MASK>` y `ROUTE IP <IP> DEBUG`, mientras que el enunciado define `SET ROUTE {NAME} NETWORK NUMBER {Y.Y.Y.Y} MASK {máscara} PRIORITY {NUMBER}` y `ROUTE IP {IP} DEBUG {true/false}`. Con la sintaxis del enunciado, las primitivas 8 y 10 responden error de sintaxis, aunque el motor de rutas funciona | T-08, T-14, T-23, T-27 | Alexandra De Gracia | Se le comunicó. Ajustar ambas primitivas y las pruebas sintácticas que usan la sintaxis anterior |
| P-06 | Una dirección con octetos válidos no siempre es una máscara válida: `255.255.0.255` tiene todos sus octetos entre 0 y 255, pero sus bits en 1 no son continuos | T-10 | Alexandra De Gracia | **Resuelto.** Se valida con operaciones bitwise que, al invertir la máscara y sumarle 1, el resultado sea una potencia de 2, lo que garantiza que no haya ceros entre los unos |
| P-07 | Los saltos de línea `\r\n` de Telnet y los espacios al inicio o repetidos entre palabras deformaban la tokenización de los comandos | T-08 | Alexandra De Gracia | **Resuelto.** Se agregó una etapa previa de saneamiento en el parser que recorta los espacios sobrantes, elimina `\r\n` y convierte la primitiva a mayúsculas antes de analizarla |

---

## 6. Resumen de participación individual

| Integrante | Tareas asignadas | Commits | PRs | Resumen del trabajo realizado |
| ---------- | ---------------: | ------: | --: | ----------------------------- |
| Luis Diego Delgado Muñoz | 7 | 3 | 1 | Integración de las primitivas 1 a 6 con el servidor, validación del Makefile, pruebas de concurrencia con `telnet`, documentación del servidor y reporte de avance de la semana 2 |
| Alexandra De Gracia | 7 | 8 | 2 | Parser de las diez primitivas con validación de IPs y máscaras, extracción de parámetros, catálogo de errores, despacho a los módulos de cálculo, pruebas sintácticas y documentación en los encabezados |
| Rafael Araya Álvarez | 7 | 0 | 0 | Sin commits ni Pull Requests durante este período |
| Isaac Rojas Robles | 7 | 2 | 1 | Tabla de rutas en lista enlazada, `SET` y `DEL ROUTE`, `ROUTE IP` con Longest Prefix Match, prioridad y modo debug, subredes aleatorias sin solapamiento y sus pruebas |
| Otto Segura Ruiz | 7 | 4 | 1 | Dockerfile multi-stage, Helm Chart, scripts de despliegue y limpieza, y tabla de estado de funcionalidades |

Las tareas T-36 a T-39 son responsabilidad compartida y no se contabilizan en la columna de tareas
asignadas.

---

## 7. Plan para la siguiente semana

| ID           | Tarea / Actividad | Responsable(s) | Resultado esperado |
| ------------ | ----------------- | -------------- | ------------------ |
| P-05, T-08, T-14 | Ajustar la sintaxis de `SET ROUTE` y `DEBUG` al enunciado | Alexandra De Gracia | Las diez primitivas funcionan por `telnet` con la sintaxis exacta del enunciado |
| T-21         | Documentar los casos de prueba del módulo de subredes | Rafael Araya | Casos paso a paso contrastados con una calculadora de referencia |
| T-36 a T-38  | Incorporar el documento maestro al repositorio | Otto Segura, con apoyo de Luis Delgado | Documentación en Markdown y PDF en el repositorio, con la tabla de funcionalidades actualizada |
| —            | Video de explicación de cada módulo | Todos | Video con una sección de dos minutos por parte |

---

## 8. Observaciones

- La entrega se trasladó, por lo que se presenta este reporte adicional, como indica el enunciado.

- Las diez primitivas se probaron por `telnet` contra el servidor compilado desde `main`. Las
primitivas 1 a 7 y 9 responden correctamente, y los errores de entrada devuelven mensajes
específicos. Las primitivas 8 y 10 dependen de P-05.

- El sistema acepta indistintamente las máscaras en notación CIDR (`/24`) y decimal punteada
(`255.255.255.0`), y ambas se convierten internamente a un entero de 32 bits (`uint32_t`), sin
importar cómo las escriba el usuario.

- El módulo de cálculo nunca procesa texto directamente. El parser actúa como un filtro que valida y
limpia los datos antes de entregarlos en estructuras nativas de C, de modo que un comando mal formado
no puede hacer fallar al servidor.

- La tabla de rutas se expone como un tipo opaco y se implementa como una lista enlazada, de modo que
los demás módulos solo pueden modificarla por medio de las funciones de `routing.h`.

- `GET RANDOM SUBNETS` usa una semilla basada en la hora del sistema. Las subredes generadas son
pseudoaleatorias y no están pensadas para usos que requieran seguridad criptográfica.

- El Helm Chart despliega una sola réplica. Esto es necesario mientras la tabla de rutas viva en la
memoria del proceso: con varias réplicas, cada pod tendría su propia tabla y una ruta creada en una
sesión podría no existir en otra.
