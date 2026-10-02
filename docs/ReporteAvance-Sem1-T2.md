# Reporte Semanal de Avance – Tarea Corta 2

**Curso:** IC7602

**TAREA CORTA:** 2

**Semana:** 1

**Fecha del reporte:** 24/9/2026


## 1. Integrantes del grupo

| Carné      | Nombre completo          |
| ---------- | ------------------------ |
| 2020030408 | Luis Diego Delgado Muñoz |
| 2026801601 | Alexandra De Gracia      |
| 2023029575 | Rafael Araya             |
| 2022104884 | Isaac Rojas Robles       |
| 2020426226 | Otto Segura              |

---

## 2. Estado de las tareas

Conforme a lo indicado en el enunciado, para el primer reporte se presenta únicamente la lista de
tareas planificadas, una breve descripción de cada una y la persona responsable. La división se
realizó por módulos aislados con contratos de interfaz en C (`.h`), de modo que cada integrante
pueda compilar y probar su componente de forma independiente y se eviten dependencias cruzadas
durante el desarrollo.

### Infraestructura de sockets TCP y servidor Telnet · Luis Diego Delgado Muñoz

| ID   | Tarea                              | Descripción                                                                                             | Responsable(s) | Estado    | Avance |
| ---- | ---------------------------------- | ------------------------------------------------------------------------------------------------------- | -------------- | --------- | -----: |
| T-01 | Servidor TCP en el puerto 9666     | Implementar el ciclo de vida del socket en GNU C: `socket`, `setsockopt` con `SO_REUSEADDR`, `bind`, `listen` y `accept` | Luis Delgado | Pendiente | 0% |
| T-02 | Concurrencia de clientes           | Atender múltiples conexiones Telnet simultáneas sin que una sesión bloquee a las demás, mediante `select`, `poll`, `epoll` o hilos POSIX | Luis Delgado | Pendiente | 0% |
| T-03 | Buffer de entrada y saneamiento    | Filtrar caracteres de control, normalizar los saltos de línea de Telnet (`\r\n` / `\n`) y garantizar la terminación nula de las cadenas | Luis Delgado | Pendiente | 0% |
| T-04 | Ensamblado de `main.c`             | Coordinar el despacho de cada comando recibido hacia el parser mediante `handle_command(const char *input, char *output, size_t max_len)` | Luis Delgado | Pendiente | 0% |
| T-05 | Makefile del proyecto              | Definir la compilación con banderas estrictas (`-Wall -Wextra -pedantic -std=gnu11`) y los objetivos de build y limpieza | Luis Delgado | Pendiente | 0% |
| T-06 | Manejo de desconexiones y errores  | Controlar cierres abruptos del cliente, errores de socket y liberación de descriptores sin fugas de recursos | Luis Delgado | Pendiente | 0% |
| T-07 | Pruebas de estabilidad y concurrencia | Validar el servidor con múltiples sesiones `telnet localhost 9666` simultáneas y documentar los casos probados | Luis Delgado | Pendiente | 0% |

### Parser del protocolo y validación de sintaxis · Alexandra De Gracia

| ID   | Tarea                              | Descripción                                                                                             | Responsable(s) | Estado    | Avance |
| ---- | ---------------------------------- | ------------------------------------------------------------------------------------------------------- | -------------- | --------- | -----: |
| T-08 | Tokenizador de primitivas          | Implementar en `parser.c` el análisis léxico y sintáctico que descompone las diez primitivas del protocolo | Alexandra De Gracia | Pendiente | 0% |
| T-09 | Conversión y validación de IPv4    | Convertir direcciones en notación decimal punteada a entero de 32 bits y validar rango de octetos, caracteres no numéricos y formatos malformados | Alexandra De Gracia | Pendiente | 0% |
| T-10 | Conversión y validación de máscaras| Soportar e intercambiar las dos notaciones de máscara: prefijo CIDR (`/bits`) y decimal punteada (`X.X.X.X`) | Alexandra De Gracia | Pendiente | 0% |
| T-11 | Extracción de parámetros por comando | Identificar y extraer los tokens `IP`, `MASK`, `NETWORK NUMBER`, `NUMBER`, `SIZE`, `PRIORITY` y `DEBUG` según la primitiva | Alexandra De Gracia | Pendiente | 0% |
| T-12 | Catálogo de mensajes de error      | Centralizar los mensajes de error descriptivos (parámetros faltantes, máscara inválida, sintaxis desconocida) en lugar de fallos genéricos | Alexandra De Gracia | Pendiente | 0% |
| T-13 | Despacho a los módulos de cálculo  | Enviar los argumentos ya validados y convertidos a las funciones de cálculo de subredes y de enrutamiento | Alexandra De Gracia | Pendiente | 0% |
| T-14 | Pruebas sintácticas                | Diseñar y documentar la batería de pruebas con comandos válidos, combinaciones inválidas y valores extremos | Alexandra De Gracia | Pendiente | 0% |

### Cálculo de subredes, primitivas 1 a 6 · Rafael Araya

| ID   | Tarea                              | Descripción                                                                                             | Responsable(s) | Estado    | Avance |
| ---- | ---------------------------------- | ------------------------------------------------------------------------------------------------------- | -------------- | --------- | -----: |
| T-15 | Interfaz pública `subnet_calc.h`   | Definir los tipos (`uint32_t`) y las firmas del módulo, desacoplado por completo de los sockets para permitir pruebas aisladas | Rafael Araya | Pendiente | 0% |
| T-16 | `GET BROADCAST` y `GET NETWORK NUMBER` | Calcular la dirección de broadcast y el número de red a partir de la IP y la máscara, usando operadores bitwise | Rafael Araya | Pendiente | 0% |
| T-17 | `GET HOSTS RANGE`                  | Obtener el primer y el último host utilizable y formatear la salida según el requerimiento (por ejemplo `10.8.2.{1-6}`) | Rafael Araya | Pendiente | 0% |
| T-18 | `GET NETMASK`                      | Conversión bidireccional entre prefijo CIDR y notación decimal punteada                                  | Rafael Araya   | Pendiente | 0% |
| T-19 | `GET HOSTS COUNT`                  | Calcular la cantidad de hosts utilizables según la máscara, contemplando los casos particulares de `/31` y `/32` | Rafael Araya | Pendiente | 0% |
| T-20 | `GET IP CLASS`                     | Clasificar la dirección en clase A, B, C, D o E mediante inspección de los bits más significativos       | Rafael Araya   | Pendiente | 0% |
| T-21 | Pruebas unitarias del módulo       | Documentar los casos de prueba paso a paso y contrastar los resultados contra calculadoras de subredes de referencia | Rafael Araya | Pendiente | 0% |

### Subredes aleatorias y motor de enrutamiento, primitivas 7 a 10 · Isaac Rojas Robles

| ID   | Tarea                              | Descripción                                                                                             | Responsable(s) | Estado    | Avance |
| ---- | ---------------------------------- | ------------------------------------------------------------------------------------------------------- | -------------- | --------- | -----: |
| T-22 | Estructura de la tabla de rutas    | Diseñar la tabla de enrutamiento en memoria (lista enlazada dinámica o arreglo dinámico ordenado) y su interfaz `routing.h` | Isaac Rojas | Pendiente | 0% |
| T-23 | `SET ROUTE` y `DEL ROUTE`          | Insertar y actualizar rutas con nombre, red destino, máscara y prioridad; eliminarlas por nombre liberando la memoria dinámica | Isaac Rojas | Pendiente | 0% |
| T-24 | `GET RANDOM SUBNETS`               | Generar N subredes válidas dentro del bloque padre, garantizando mediante validación bitwise que ninguna se solape con otra | Isaac Rojas | Pendiente | 0% |
| T-25 | `ROUTE IP` — Longest Prefix Match  | Implementar la búsqueda de ruta para una IP dada seleccionando la máscara más específica                 | Isaac Rojas    | Pendiente | 0% |
| T-26 | `ROUTE IP` — desempate por prioridad | Resolver los empates entre rutas de igual prefijo mediante el valor de `PRIORITY`                      | Isaac Rojas    | Pendiente | 0% |
| T-27 | Modo `DEBUG`                       | Construir la traza explicativa del cálculo: rutas evaluadas, aplicación de la máscara, coincidencias y criterio de desempate. Por defecto `false` | Isaac Rojas | Pendiente | 0% |
| T-28 | Matriz de pruebas de enrutamiento  | Cubrir solapamiento de prefijos, colisiones de prioridad, rutas inexistentes y validación del modo debug | Isaac Rojas    | Pendiente | 0% |

### Automatización, Docker, Kubernetes y Helm · Otto Segura

| ID   | Tarea                              | Descripción                                                                                             | Responsable(s) | Estado    | Avance |
| ---- | ---------------------------------- | ------------------------------------------------------------------------------------------------------- | -------------- | --------- | -----: |
| T-29 | Dockerfile multi-stage             | Compilar el código C en una primera etapa con `gcc`/`make` y ejecutar el binario en una imagen base minimalista | Otto Segura | Pendiente | 0% |
| T-30 | `Chart.yaml` y `values.yaml`       | Definir la metadata del chart y las variables configurables: puerto 9666, réplicas, recursos de CPU y memoria, tipo de servicio | Otto Segura | Pendiente | 0% |
| T-31 | Plantilla `deployment.yaml`        | Definir el despliegue del servidor en Kubernetes a partir de los valores parametrizados                  | Otto Segura    | Pendiente | 0% |
| T-32 | Plantilla `service.yaml`           | Exponer el puerto TCP 9666 mediante NodePort o LoadBalancer                                              | Otto Segura    | Pendiente | 0% |
| T-33 | Scripts de automatización          | Levantar el clúster local y ejecutar `helm install` con un único comando, sin pasos manuales             | Otto Segura    | Pendiente | 0% |
| T-34 | Pruebas de extremo a extremo       | Levantar el pod en Kubernetes y verificar la comunicación desde el host mediante `telnet <IP-Nodo> <NodePort>` | Otto Segura | Pendiente | 0% |
| T-35 | Tabla de estado de funcionalidades | Elaborar la tabla obligatoria de funcionalidades soportadas y no soportadas requerida por la documentación | Otto Segura   | Pendiente | 0% |

### Tareas transversales · Todo el grupo

| ID   | Tarea                              | Descripción                                                                                             | Responsable(s) | Estado    | Avance |
| ---- | ---------------------------------- | ------------------------------------------------------------------------------------------------------- | -------------- | --------- | -----: |
| T-36 | Documentación técnica por módulo   | Cada integrante redacta las instrucciones, diagramas y casos de prueba reproducibles de su propio módulo | Los 5          | Pendiente | 0% |
| T-37 | Conclusiones y recomendaciones     | Cada integrante aporta dos conclusiones y dos recomendaciones técnicas justificadas, para completar las diez de cada tipo que exige la rúbrica | Los 5 | Pendiente | 0% |
| T-38 | Compilación del documento maestro  | Integrar la documentación en Markdown, unificar el estilo visual y compilarla a PDF                      | Otto Segura, con apoyo de Luis Delgado | Pendiente | 0% |
| T-39 | Reportes de avance                 | Consolidar y enviar el reporte semanal bajo el formato `T2R{número}`, con responsabilidad rotativa entre los integrantes | Los 5 | Pendiente | 0% |

**Estados permitidos:** Pendiente / En progreso / Bloqueada / Finalizada.

---

## 3. Commits realizados

Al tratarse del primer reporte de avance, no se registran commits durante este período.

---

## 4. Pull Requests (PRs)

Ningún integrante del grupo creó Pull Requests durante este período.

---

## 5. Problemas

No se presentaron problemas o bloqueos durante esta semana.

---

## 6. Resumen de participación individual

| Integrante               | Tareas asignadas | Commits | PRs | Resumen del trabajo realizado                                                      |
| ------------------------ | ---------------: | ------: | --: | ---------------------------------------------------------------------------------- |
| Luis Diego Delgado Muñoz |                7 |       0 |   0 | Participó en la planificación y asumió la infraestructura de sockets TCP y el servidor Telnet |
| Alexandra De Gracia      |                7 |       0 |   0 | Participó en la planificación y asumió el parser del protocolo y la validación de sintaxis |
| Rafael Araya             |                7 |       0 |   0 | Participó en la planificación y asumió el cálculo de subredes de las primitivas 1 a 6 |
| Isaac Rojas Robles       |                7 |       0 |   0 | Participó en la planificación y asumió las subredes aleatorias y el motor de enrutamiento |
| Otto Segura              |                7 |       0 |   0 | Participó en la planificación y asumió la automatización con Docker, Kubernetes y Helm |

Las tareas transversales T-36 a T-39 son responsabilidad compartida de los cinco integrantes y no se
contabilizan en la columna anterior.

---

## 7. Plan para la siguiente semana

| ID           | Tarea / Actividad                                   | Responsable(s)      | Resultado esperado                                                            |
| ------------ | --------------------------------------------------- | ------------------- | ----------------------------------------------------------------------------- |
| T-15, T-22   | Publicar los contratos de interfaz                  | Rafael Araya, Isaac Rojas | `subnet_calc.h` y `routing.h` acordados y disponibles en `main` para el resto del equipo |
| T-01 a T-05  | Implementar el servidor TCP                         | Luis Delgado        | Servidor escuchando en el puerto 9666, atendiendo sesiones concurrentes de `telnet` |
| T-08 a T-11  | Implementar el parser                               | Alexandra De Gracia | Primitivas tokenizadas y parámetros de IP y máscara validados en ambas notaciones |
| T-16 a T-20  | Implementar las primitivas 1 a 6                    | Rafael Araya        | Cálculo de broadcast, número de red, rango de hosts, máscara, conteo y clase    |
| T-22 a T-27  | Implementar las primitivas 7 a 10                   | Isaac Rojas         | Subredes aleatorias, tabla de rutas y resolución por Longest Prefix Match       |
| T-29 a T-33  | Construir la imagen y el Helm Chart                 | Otto Segura         | Despliegue del servidor en Kubernetes mediante un único comando automatizado    |
| T-36, T-37   | Redactar la documentación de cada módulo            | Los 5               | Documentación técnica por componente, con conclusiones y recomendaciones        |

---

## 8. Observaciones

- La firma de la función de despacho entre el servidor y el parser quedó definida desde la
  planificación como `handle_command(const char *input, char *output, size_t max_len)`.