# Documentación — T2

**Curso:** IC-7602 Redes · **Tarea corta 2** · S2 2026

**Integrantes:** Isaac Rojas Robles · Rafael Araya Álvarez · Luis Diego Delgado Muñoz · Otto Segura Ruiz · Alexandra De Gracia

---

## 1. Qué es este proyecto

Es una calculadora de subredes IPv4 que funciona como servidor. El servidor está escrito en GNU C,
escucha en el puerto TCP 9666 y se usa conectándose con `telnet`: el usuario escribe una primitiva
(un comando de texto) y el servidor responde con el resultado o con un mensaje de error que explica
qué está mal.

Soporta las diez primitivas del enunciado:

| # | Primitiva | Ejemplo | Respuesta |
| - | --------- | ------- | --------- |
| 1 | Dirección de broadcast | `GET BROADCAST IP 10.8.2.5 MASK /29` | `10.8.2.7` |
| 2 | Número de red | `GET NETWORK NUMBER IP 10.8.2.5 MASK /29` | `10.8.2.0` |
| 3 | Rango de hosts | `GET HOSTS RANGE IP 10.8.2.5 MASK /29` | `10.8.2.{1-6}` |
| 4 | Máscara o prefijo CIDR | `GET NETMASK MASK /27` | `255.255.255.224` |
| 5 | Cantidad de hosts | `GET HOSTS COUNT MASK /29` | `6` |
| 6 | Clase de una dirección IP | `GET IP CLASS 10.8.2.5` | `Clase A` |
| 7 | Subredes aleatorias | `GET RANDOM SUBNETS NETWORK NUMBER 10.0.0.0 MASK /8 NUMBER 3 SIZE /24` | Tres subredes `/24` distintas dentro de `10.0.0.0/8` |
| 8 | Agregar una ruta | `SET ROUTE casa NETWORK NUMBER 8.8.0.0 MASK /16 PRIORITY 1` | `OK: ruta casa configurada` |
| 9 | Eliminar una ruta | `DEL ROUTE casa` | `OK: ruta casa eliminada` |
| 10 | Enrutar una dirección IP | `ROUTE IP 8.8.8.8 DEBUG false` | Nombre de la ruta seleccionada |

Las máscaras se aceptan en notación de prefijo (`/24`) o decimal punteada (`255.255.255.0`). Con
`DEBUG true`, la primitiva 10 muestra además cada ruta evaluada y el criterio con el que se
seleccionó.

El sistema se compone de cinco módulos: el servidor TCP, el parser del protocolo, el módulo de
cálculo de subredes (primitivas 1 a 6), el módulo de enrutamiento (primitivas 7 a 10) y la
automatización del despliegue con Docker, Kubernetes y Helm.

---

## 2. Requisitos previos

* GCC instalado
    - con soporte para C11 / GNU11
* make instalado
    - para ejecutar targets como make, make test y make clean
* Sistema tipo Unix/Linux o WSL
    - el proyecto está orientado a compilación con gcc y make
* Opcional: Docker
    - si no se tiene gcc instalado

---

## 3. Instalación y ejecución


### 3.1 Despliegue Automatizado con Kubernetes y Helm

El proyecto cuenta con un flujo de instalación 100% automatizado en un único comando mediante el script `deploy.sh`. Este script valida las dependencias del sistema, levanta el clúster local de Kubernetes (`kind` o `minikube`), construye la imagen Docker mediante un multi-stage build, la carga al clúster e instala el release de Helm.

#### Requisitos del entorno:
- Docker Engine activo.
- `kubectl` instalado.
- `helm` v3+ instalado.
- `kind` o `minikube` instalado (opcional: si no existe un clúster activo, el script lo inicializa automáticamente).

#### Pasos para ejecutar:
1. **Desplegar el servicio en Kubernetes:**
   ```bash
   ./scripts/deploy.sh
   ```
   *(El script compilará el binario, generará la imagen `subnet-calculator:latest`, desplegará los manifiestos y esperará a que el Pod esté en estado `Ready`).*

2. **Conectarse al servidor vía Telnet o Netcat:**
   Una vez concluido el despliegue, el servicio quedará expuesto a través de NodePort en el puerto `30666`:
   ```bash
   telnet localhost 30666
   ```
   o bien:
   ```bash
   nc localhost 30666
   ```

3. **Desinstalar y limpiar recursos:**
   Para retirar el release de Helm de forma limpia sin dejar pods huérfanos:
   ```bash
   ./scripts/cleanup.sh
   ```

### 3.2 Compilación y ejecución local, sin Kubernetes

Para probar el servidor directamente en Linux o WSL hacen falta `gcc`, `make` y `telnet`. En Ubuntu
se instalan con:

```bash
sudo apt-get update && sudo apt-get install -y build-essential telnet
```

Desde la raíz del repositorio:

```bash
make servidor     # compila el servidor
./servidor        # lo levanta en el puerto 9666
```

Debe aparecer `Servidor escuchando en el puerto 9666`. Si se quiere usar otro puerto, se indica como
argumento: `./servidor 9000`.

En otra terminal, conectarse y escribir las primitivas:

```text
$ telnet localhost 9666
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
GET BROADCAST IP 10.8.2.5 MASK /29
10.8.2.7
GET IP CLASS 10.8.2.5
Clase A
```

Para salir de `telnet` se presiona `Ctrl+]` y luego se escribe `quit`. El servidor se detiene con
`Ctrl+C`.

Otros objetivos del `Makefile`:

| Comando | Qué hace |
| ------- | -------- |
| `make` | Compila el servidor y todos los programas de prueba |
| `make test` | Compila y ejecuta las pruebas unitarias de todos los módulos |
| `make clean` | Borra los binarios y archivos objeto |

### 3.3 Funcionamiento del servidor

El servidor (`server.c`, `server.h` y `main.c`) es la puerta de entrada de la calculadora. Escucha en
el puerto TCP 9666, acepta varias sesiones de `telnet` al mismo tiempo, limpia lo que escribe cada
cliente y le pasa cada línea al parser. No hace ningún cálculo de subredes: su contrato con el resto
del sistema es una sola función, acordada desde la planificación.

```c
void handle_command(const char *input, char *output, size_t max_len);
```

```text
 telnet 1 ─┐
 telnet 2 ─┼──> servidor (puerto 9666) ──> parser ──┬──> cálculo de subredes (primitivas 1 a 6)
 telnet N ─┘    poll() + saneamiento       handle_   └──> enrutamiento (primitivas 7 a 10)
                                           command
```

**Ciclo de vida del socket.** Al arrancar se crea un socket TCP, se activa `SO_REUSEADDR` para
poder reiniciar el servidor sin esperar a que el sistema libere el puerto, se asocia al 9666 con
`bind` y se queda escuchando con `listen`. Cada cliente nuevo se acepta con `accept`, hasta un
máximo de 64 conexiones simultáneas; al cliente 65 se le responde
`ERROR: servidor lleno, intente mas tarde` y se cierra su conexión.

**Concurrencia con `poll()`.** Todas las sesiones se atienden en un solo hilo. `poll()` vigila a la
vez el socket de escucha y los de todos los clientes, y avisa cuál tiene datos para leer o espacio
para escribir. Los sockets son no bloqueantes, así que un cliente lento o inactivo no detiene a los
demás. Se prefirió a un hilo por cliente porque la tabla de rutas es compartida por todas las
sesiones: con un solo hilo nunca se ejecutan dos comandos al mismo tiempo y no hacen falta mutex.

```text
           ┌────────────────────────────────────────────────────┐
           v                                                    │
   poll() sobre todos los sockets                               │
           │                                                    │
           ├─ socket de escucha ──> accept ──> registrar cliente ┤
           │                                                    │
           ├─ cliente con datos ──> recv ──> saneamiento ──> handle_command
           │                         │                          │
           │                         └─ 0 bytes o error ──> cerrar cliente
           │                                                    │
           └─ cliente listo para escribir ──> send ─────────────┘
```

**Saneamiento de la entrada Telnet.** `telnet` no envía solo el texto: al conectarse puede negociar
opciones con bytes de control y termina cada línea con `\r\n`. Además, TCP puede partir un comando
en varios segmentos. El servidor procesa la entrada byte por byte y guarda el estado de cada
cliente:

| Entrada | Tratamiento |
| ------- | ----------- |
| Negociación Telnet (byte `IAC` = `0xFF` y sus opciones) | Se descarta |
| `\r\n`, `\r\0` o `\n` | Fin de línea: el comando se entrega al parser terminado en `\0` |
| Tabulador | Se convierte en espacio |
| Otros caracteres de control y bytes no ASCII | Se descartan |
| Línea de más de 1023 caracteres | Se descarta y se responde `ERROR: comando demasiado largo`, sin cerrar la sesión |
| Línea vacía | Se ignora |

**Errores y desconexiones.** Si un cliente se desconecta de golpe, se cierra solo su descriptor y los
demás siguen atendidos. Si un cliente no lee sus respuestas, estas se acumulan en un buffer propio;
si se llena, se desconecta solo a ese cliente. `SIGINT` (`Ctrl+C`) y `SIGTERM`, que es la señal que
envía Kubernetes al detener un pod, hacen que el servidor cierre todas las conexiones de forma
ordenada antes de terminar. Cada conexión y desconexión queda registrada en la salida estándar, que
se puede consultar con `kubectl logs`.

---

## 4. Pruebas realizadas

### 4.1 Casos probados

| # | Qué se probó | Cómo reproducirlo | Resultado |
| --- | --- | --- | --- |
| 1 | Crear y destruir una tabla de rutas | Ejecutar `make prueba_routing && ./prueba_routing` | `[OK] Crear y destruir tabla`; también se verifica que destruir `NULL` sea seguro. |
| 2 | Insertar una ruta | Ejecutar la prueba con `route_set(table, "oficina", 0x0A080205, 29, 3)` | La función retorna `ROUTE_STATUS_OK` y la ruta queda registrada. |
| 3 | Actualizar una ruta existente | Registrar dos veces el nombre `oficina`, primero con `0x0A080205/29` y luego con `0xC0A80199/24`, prioridad `8` | `route_lookup` devuelve `192.168.1.0/24` con prioridad `8`, sin duplicar la ruta. |
| 4 | Longest Prefix Match | Configurar `10.0.0.0/8`, `10.8.0.0/16` y `10.8.2.0/24`; consultar `10.8.2.5` | Se selecciona la ruta `servidores`, porque `/24` es el prefijo más específico. |
| 5 | Desempate por prioridad | Configurar dos rutas `/16` sobre `10.8.0.0`, con prioridades `1` y `10` | Entre rutas con igual prefijo se selecciona la de mayor prioridad; una ruta `/24` sigue ganando por LPM. |
| 6 | IP sin ruta coincidente | Consultar `172.16.0.1` sin una ruta que la contenga | `route_lookup` retorna `ROUTE_STATUS_NO_MATCH`. |
| 7 | Modo DEBUG | Ejecutar `route_lookup_debug` para `10.8.2.5` | La traza muestra rutas evaluadas, IP enmascarada, coincidencias y la selección `servidores`. |
| 8 | Generar subredes sin solapamiento | Generar `8` subredes `/24` dentro de `10.0.0.0/8` | Todas pertenecen a la red padre, tienen prefijo `/24` y poseen redes distintas. |
| 9 | Rechazar subred hija demasiado grande | Solicitar una subred `/16` dentro de un padre `/24` | Se retorna `ROUTE_STATUS_SUBNET_TOO_LARGE`. |
| 10 | Rechazar cantidad excesiva de subredes | Solicitar `5` subredes `/32` dentro de `10.0.0.0/30` | Se retorna `ROUTE_STATUS_INSUFFICIENT_SPACE`, porque solo caben cuatro bloques. |
| 11 | Rechazar cantidad cero | Solicitar `0` subredes con padre e hijo `/24` | Se retorna `ROUTE_STATUS_INVALID_COUNT`. |
| 12 | Validar argumentos de rutas | Probar nombre `NULL`, nombre vacío, nombre demasiado largo y prefijos `-1` y `33` | Se retornan los códigos de error correspondientes y aparece `[OK] Validaciones de route_set`. |
| 13 | Eliminar y liberar una ruta | Crear y eliminar `casa`; repetir la eliminación y eliminar un nombre inexistente | La primera operación retorna `ROUTE_STATUS_OK`; las siguientes retornan `ROUTE_STATUS_NOT_FOUND`. |

### 4.2 Casos probados · Parser del protocolo y validación de sintaxis 

| # | Qué se probó | Cómo reproducirlo | Resultado |
|---|---|---|---|
| 1 | **Análisis léxico y tokenización de primitivas (T-08)** | Ejecutar `./prueba_parser` con primitivas 1 a 10 usando espacios múltiples y tabulaciones (ej. `GET  BROADCAST  IP  192.168.1.10  MASK  /24`). | Identifica correctamente la primitiva (`PRIMITIVE_GET_BROADCAST`) y separa los tokens de entrada sin desbordamiento de búfer. |
| 2 | **Rechazo de primitivas desconocidas y líneas vacías (T-08)** | Ejecutar `./prueba_parser` probando `INVALID COMMAND HERE` y cadenas vacías `""`. |  Retorna `is_valid = false`, asigna `PRIMITIVE_UNKNOWN` y genera el mensaje de error correspondiente desde el catálogo. |
| 3 | **Validación de formato IPv4 y rango de octetos (T-09)** | Probar en `./prueba_parser` las direcciones `192.168.1.1`, `256.0.0.1`, `-1.0.0.0` y `abc.168.1.1`. |  Acepta octetos en el rango `[0, 255]` y rechaza valores mayores a 255, signos negativos y caracteres alfabéticos. |
| 4 | **Conversión de IPv4 a entero `uint32_t` (T-09)** | Probar `convertir_ip_a_entero("0.0.0.0")`, `convertir_ip_a_entero("192.168.1.1")` y `convertir_ip_a_entero("255.255.255.255")`. |  Genera los enteros de 32 bits exactos `0U`, `3232235777U` y `4294967295U` en orden de red. |
| 5 | **Conversión e intercambio CIDR / Decimal (T-10)** | Probar `parsear_mascara_a_cidr("/24", &p)` y `parsear_mascara_a_cidr("255.255.255.0", &p)`. |  Convierte bidireccionalmente ambas notaciones al prefijo `24` o su equivalente binario `0xFFFFFF00U`. |
| 6 | **Validación de contigüidad en máscaras decimales (T-10)** | Probar `validar_mascara("255.255.255.0")` frente a `validar_mascara("255.0.255.0")` en `./prueba_parser`. |  Acepta máscaras contiguas desde el MSB y rechaza máscaras no contiguas o discontinuas. |
| 7 | **Extracción de parámetros por comando (T-11)** | Ejecutar `./prueba_param_extractor` para comandos válidos extrayendo `IP`, `MASK`, `NAME`, `NETWORK NUMBER`, `PRIORITY`, `SIZE` y `DEBUG`. |  Puebla correctamente la estructura `extracted_params_t` y permite la consulta mediante accesores individuales. |
| 8 | **Manejo de parámetros opcionales y valores por defecto (T-11)** | Extraer parámetros de `ROUTE IP 8.8.8.8` (sin `DEBUG`) y de `ROUTE IP 8.8.8.8 DEBUG true`. |  Asigna `debug = false` cuando no se indica y `debug = true` cuando se indica `true`, garantizando un estado neutro sin basura en memoria. |
| 9 | **Catálogo centralizado de errores descriptivos (T-12)** | Ejecutar `./prueba_error_catalog` ante sintaxis inválida, parámetros faltantes e IP/máscaras malformadas. |  Genera mensajes formateados específicos como `"ERROR: direccion IP invalida: 999.168.1.10"` y `"ERROR: mascara de red invalida: /33"`. |
| 10 | **Despacho a los módulos de cálculo (T-13)** | Ejecutar `./prueba_dispatcher` invocando `dispatch_command_line` para las 10 primitivas soportadas. |  Envía los argumentos validados a las funciones del módulo de cálculo (`obtener_ip_broadcast`, `obtener_numero_red`, etc.) retornando respuestas correctas. |
| 11 | **Batería sintáctica completa de 75 casos (T-14)** | Ejecutar `./prueba_sintactica` para verificar 25 casos válidos, 30 combinaciones inválidas y 20 valores extremos. | 75/75 (100% de éxito), comprobando valores límites como `0.0.0.0/0`, `255.255.255.255/32`, `SIZE 65535` y rechazo de orden incorrecto. |

### 4.3 Automatización, Docker, Kubernetes y Helm

| # | Qué se probó | Cómo reproducirlo | Resultado |
|---|---|---|---|
| 1 | **Compilación y empaquetado multi-stage (T-29)** | Ejecutar `docker build -t subnet-calculator:test .` | Compilación limpia en etapa builder con Alpine + `build-base`. Genera una imagen final de solo **13.8 MB** con usuario sin privilegios `appuser`. |
| 2 | **Ejecución y aislamiento del contenedor Docker (T-29)** | Ejecutar `docker run -d --rm -p 9666:9666 subnet-calculator:test` y conectar con `printf "GET NETMASK /24\r\n" \| nc localhost 9666` | El contenedor inicia correctamente bajo `appuser`, escucha en el puerto TCP 9666 y responde a las solicitudes a través del socket. |
| 3 | **Validación sintáctica del Helm Chart (T-30 a T-32)** | Ejecutar `helm lint deploy/helm/subnet-calculator` | `1 chart(s) linted, 0 chart(s) failed`. Cero errores de sintaxis o formato en plantillas y valores. |
| 4 | **Renderizado de plantillas de Kubernetes (T-31 y T-32)** | Ejecutar `helm template test-release deploy/helm/subnet-calculator` | Genera correctamente los manifiestos de `Deployment` y `Service` con nombres normalizados, límites de recursos (100m CPU / 64Mi RAM) y sondas TCP en puerto 9666. |
| 5 | **Sondas de salud TCP (liveness y readiness) (T-31)** | Inspeccionar el Pod en ejecución mediante `kubectl describe pod -l app.kubernetes.io/name=subnet-calculator` | Las sondas `tcpSocket` al puerto 9666 pasan exitosamente; el pod alcanza el estado `1/1 Ready` sin reinicios inesperados. |
| 6 | **Exposición del servicio vía NodePort (T-32)** | Inspeccionar los servicios con `kubectl get svc subnet-calculator` | El servicio expone el puerto interno 9666 mapeado externamente al puerto del nodo `30666/TCP`. |
| 7 | **Script de despliegue en un solo comando (T-33)** | Ejecutar `./scripts/deploy.sh` en un entorno limpio sin clúster activo | Inicializa el clúster local `kind`, compila la imagen, la inyecta al nodo, ejecuta `helm upgrade --install` y realiza el rollout exitoso sin requerir intervención manual. |
| 8 | **Script de desinstalación limpia (T-33)** | Ejecutar `./scripts/cleanup.sh` tras haber desplegado el release | Ejecuta `helm uninstall subnet-calculator` y remueve de forma segura los recursos del clúster. |
| 9 | **Prueba de extremo a extremo (E2E) vía Telnet / Socket (T-34)** | Con el clúster activo, ejecutar `printf "GET BROADCAST IP 10.8.2.5 MASK /29\r\n" \| nc -w 2 localhost 30666` | La conexión ingresa por el NodePort `30666`, es enrutada al contenedor en Kubernetes y el servidor GNU C atiende y responde la consulta por el socket TCP. |
| 10 | **Consistencia de la tabla de estado de funcionalidades (T-35)** | Verificar la integridad y completitud de la sección 5 de este documento | Matriz consolidada cubriendo las 10 primitivas y componentes de infraestructura con sus respectivos responsables y estados. |

### 4.4 Servidor TCP y sesiones Telnet

Las pruebas 2 a 10 parten de tener el servidor corriendo en una terminal con `./servidor`.

| # | Qué se probó | Cómo reproducirlo | Resultado |
|---|---|---|---|
| 1 | **Saneamiento de la entrada (T-03)** | Ejecutar `make test_server` | Pasan las cinco pruebas: fin de línea (`\r\n`, `\n`, `\r\0`), comando fragmentado en varios envíos, negociación Telnet, caracteres de control y línea demasiado larga. |
| 2 | **Sesión básica (T-01)** | `telnet localhost 9666` y escribir `GET IP CLASS 10.8.2.5` | Responde `Clase A`. |
| 3 | **Varias sesiones simultáneas (T-02)** | Abrir tres terminales con `telnet localhost 9666` y enviar comandos en todas, alternando | Cada terminal recibe solo sus propias respuestas. |
| 4 | **Un cliente inactivo no bloquea (T-02)** | Abrir una sesión sin escribir nada y enviar comandos desde otra | La segunda sesión responde de inmediato. |
| 5 | **Cierre abrupto del cliente (T-06)** | Cerrar a la fuerza una terminal con `telnet` abierto, o ejecutar `pkill -9 telnet` | El servidor registra la desconexión y sigue atendiendo a los demás. |
| 6 | **Comando demasiado largo (T-03)** | Pegar una línea de más de 1023 caracteres | Responde `ERROR: comando demasiado largo` y la sesión sigue funcionando. |
| 7 | **Puerto ocupado (T-01)** | Con un servidor corriendo, ejecutar `./servidor` en otra terminal | `bind: Address already in use` y termina con código 1. |
| 8 | **Puerto inválido (T-01)** | `./servidor 99999` | `Puerto invalido: 99999 (debe estar entre 1 y 65535)`. |
| 9 | **Detención ordenada (T-06)** | `Ctrl+C` o `kill -TERM <pid>` | Cierra las conexiones, imprime `Servidor detenido` y termina con código 0. |
| 10 | **Reinicio inmediato (T-01)** | Detener el servidor y volver a levantarlo enseguida | Arranca sin error de puerto ocupado, gracias a `SO_REUSEADDR`. |

La prueba 3 también se puede reproducir de forma automática. Este comando abre cinco sesiones de
`telnet` en paralelo; cada una envía diez comandos `GET BROADCAST` con IPs distintas y al final se
muestra cuántas respuestas recibió cada sesión:

```bash
for c in 1 2 3 4 5; do
  ( { sleep 0.5; for i in $(seq 1 10); do printf 'GET BROADCAST IP 10.%d.2.%d MASK /29\r\n' $c $i; sleep 0.1; done; sleep 1; } \
    | telnet localhost 9666 > /tmp/sesion.$c 2>&1 ) &
done; wait
for c in 1 2 3 4 5; do echo "sesion $c:"; tr -d '\r' < /tmp/sesion.$c | grep '^10\.' | sort | uniq -c; done
```

Resultado obtenido: cada sesión recibió exactamente sus diez respuestas, siete con broadcast
`10.c.2.7` (IPs `.1` a `.7`) y tres con `10.c.2.15` (IPs `.8` a `.10`, que caen en la siguiente
subred /29), sin ninguna respuesta de otra sesión.

Durante el desarrollo también se probó con 20 clientes en paralelo enviando 50 comandos cada uno, y
con 70 conexiones simultáneas: se aceptaron 64 y se rechazaron 6 con `servidor lleno`. Con
`ls /proc/<pid>/fd` se verificó que, al cerrarse todas las sesiones, el proceso solo conserva sus
descriptores base: entrada, salida, error y el socket de escucha.

---

## 5. Qué funciona y qué no

### 1. Primitivas del Protocolo Telnet

|   #    | Primitiva / Funcionalidad        | Sintaxis de Ejemplo                                                    |       Estado       | Responsable | Observaciones                                                                      |
| :----: | :------------------------------- | :--------------------------------------------------------------------- | :----------------: | :---------: | :--------------------------------------------------------------------------------- |
| **1**  | Obtener dirección de broadcast   | `GET BROADCAST IP 10.8.2.5 MASK /29`                                   |   **Soportada**    | Rafael Araya  | Implementada con álgebra bitwise en `calculo_subredes.c`. Pruebas unitarias pasan. |
| **2**  | Obtener número de red            | `GET NETWORK NUMBER IP 10.8.2.5 MASK /29`                              |   **Soportada**    | Rafael Araya  | Implementada con álgebra bitwise en `calculo_subredes.c`. Pruebas unitarias pasan. |
| **3**  | Obtener rango de hosts           | `GET HOSTS RANGE IP 10.8.2.5 MASK /29`                                 |   **Soportada**    | Rafael Araya  | Formato `X.X.X.{primero-ultimo}` implementado en `calculo_subredes.c`.             |
| **4**  | Obtener máscara o prefijo CIDR   | `GET NETMASK MASK /27`<br>`GET NETMASK MASK 255.255.255.0`             |   **Soportada**    | Rafael Araya  | Conversión bidireccional prefijo $\leftrightarrow$ máscara decimal punteada.       |
| **5**  | Obtener cantidad de hosts        | `GET HOSTS COUNT MASK /29`                                             |   **Soportada**    | Rafael Araya  | Cálculo con $2^{32-prefix}-2$, manejando casos especiales (/31 y /32).             |
| **6**  | Obtener clase de dirección IP    | `GET IP CLASS 10.8.2.5`                                                |   **Soportada**    | Rafael Araya  | Clasificación estricta de Clases A, B, C, D y E mediante bits más significativos.  |
| **7**  | Generar subredes aleatorias      | `GET RANDOM SUBNETS NETWORK NUMBER 10.0.0.0 MASK /8 NUMBER 3 SIZE /24` | **Soportada** |  Isaac Rojas  | `route_generate_random_subnets` valida prefijos y capacidad, normaliza la red padre y genera bloques hijos distintos y alineados, sin solapamientos. La prueba aislada de P4 pasa. |
| **8**  | Agregar rutas a tabla dinámica   | `SET ROUTE R1 NETWORK NUMBER 192.168.1.0 MASK /24 PRIORITY 10`         | **Soportada** |  Isaac Rojas  | `route_set` usa una lista enlazada opaca, normaliza la red con AND bitwise y actualiza una ruta existente cuando se repite el nombre. El servidor conserva la tabla durante su ejecución. |
| **9**  | Eliminar rutas de tabla dinámica | `DEL ROUTE R1`                                                         | **Soportada** |  Isaac Rojas  | `route_delete` elimina por nombre y libera el nodo; informa cuando la ruta no existe. La operación está integrada en `handle_command` y cubierta por pruebas. |
| **10** | Realizar routing de IP y DEBUG   | `ROUTE IP 8.8.8.8`<br>`ROUTE IP 8.8.8.8 DEBUG true`                    | **Soportada** |  Isaac Rojas  | `route_lookup` aplica Longest Prefix Match y desempata por mayor prioridad. `route_lookup_debug` muestra rutas evaluadas, máscara aplicada, coincidencias y selección final. |
| **11** | Reconocimiento en el parser de la primitiva 1: GET BROADCAST | `GET BROADCAST IP 192.168.1.10 MASK /24` | **Soportada** | Alexandra De Gracia | Tokenización, validación IPv4/máscara y extracción completadas |
| **12** | Reconocimiento en el parser de la primitiva 2: GET NETWORK NUMBER | `GET NETWORK NUMBER IP 192.168.1.50 MASK /24` | **Soportada** | Alexandra De Gracia | Análisis sintáctico y extracción de IP y máscara CIDR/decimal completados. |
| **13** | Reconocimiento en el parser de la primitiva 3: GET HOSTS RANGE | `GET HOSTS RANGE IP 192.168.1.10 MASK /24` | **Soportada** | Alexandra De Gracia | Validación sintáctica y extracción de parámetros IP y MASK completadas. |
| **14** | Reconocimiento en el parser de la primitiva 4: GET NETMASK | `GET NETMASK MASK /24` | **Soportada** | Alexandra De Gracia | Soporta la forma canónica `MASK <MASK>` y abreviada `<MASK>` en notación CIDR o decimal. |
| **15** | Reconocimiento en el parser de la primitiva 5: GET HOSTS COUNT | `GET HOSTS COUNT MASK /24` | **Soportada** | Alexandra De Gracia | Soporta ambas notaciones de máscara e identifica el parámetro opcional MASK. |
| **16** | Reconocimiento en el parser de la primitiva 6: GET IP CLASS | `GET IP CLASS 10.8.2.5` | **Soportada** | Alexandra De Gracia | Soporta prefijo explícito `IP <IP>` u omisión directa de la palabra clave. |
| **17** | Reconocimiento en el parser de la primitiva 7: GET RANDOM SUBNETS | `GET RANDOM SUBNETS NETWORK NUMBER 10.0.0.0 MASK /16 NUMBER 4 SIZE /24` | **Soportada** | Alexandra De Gracia | Extrae NETWORK NUMBER y MASK de la red padre, valida que NUMBER sea un entero mayor a 0 y extrae la máscara SIZE de las subredes. |
| **18** | Reconocimiento en el parser de la primitiva 8: SET ROUTE | `SET ROUTE casa NETWORK NUMBER 192.168.1.0 MASK /24 PRIORITY 10` | **Soportada** | Alexandra De Gracia | Extrae NAME, NETWORK NUMBER, MASK y PRIORITY, que es obligatorio. |
| **19** | Reconocimiento en el parser de la primitiva 9: DEL ROUTE | `DEL ROUTE casa` | **Soportada** | Alexandra De Gracia | Valida nombre de ruta y rechaza parámetros adicionales no permitidos. |
| **20** | Reconocimiento en el parser de la primitiva 10: ROUTE IP | `ROUTE IP 8.8.8.8 DEBUG true` | **Soportada** | Alexandra De Gracia | Identifica la IP de destino y procesa el parámetro opcional DEBUG con valor `true` o `false`; si se omite, es `false`. |
| **21** | Conversión y validación IPv4 | `validar_ip("192.168.1.1")`, `convertir_ip_a_entero("10.0.0.1")` | **Soportada** | Alexandra De Gracia | Valida octetos en [0-255], rechaza caracteres no numéricos y convierte a `uint32_t`. |
| **22** | Conversión e intercambio de máscaras  | `parsear_mascara_a_cidr("/24")`, `validar_mascara("255.255.255.0")` | **Soportada** | Alexandra De Gracia | Valida la contigüidad binaria y permite la conversión entre `/N` y `A.B.C.D`. |
| **23** | Catálogo de errores descriptivos | `err_cat_invalid_ip(...)`, `err_cat_missing_param(...)` | **Soportada** | Alexandra De Gracia | Reemplaza errores genéricos por mensajes descriptivos centralizados en `error_catalog.c`. |
| **24** | Despacho a módulos de cálculo | `dispatch_command_line("GET BROADCAST IP ... MASK ...", &res)` | **Soportada** | Alexandra De Gracia | Transfiere argumentos validados hacia las funciones de cálculo de subredes e interfaz de rutas. |
| **25** | Batería de pruebas sintácticas | `./prueba_sintactica` | **Soportada** | Alexandra De Gracia | 75/75 pruebas  (25 válidas, 30 inválidas y 20 extremas). |

---

### 2. Componentes de Arquitectura e Infraestructura

| Componente                     | Descripción                                                |       Estado       |     Responsable     | Observaciones                                                                              |
| :----------------------------- | :--------------------------------------------------------- | :----------------: | :-----------------: | :----------------------------------------------------------------------------------------- |
| **Servidor TCP Concurrente**   | Socket en puerto 9666 con `poll()`, no bloqueante          |   **Soportada**    |    Luis Delgado     | Atiende múltiples clientes Telnet concurrentes en un solo hilo. Maneja `SIGINT`/`SIGTERM`. |
| **Saneamiento Telnet**         | Filtrado de secuencias IAC, caracteres de control y `\r\n` |   **Soportada**    |    Luis Delgado     | Buffer de entrada saneado; las pruebas unitarias pasan.                                    |
| **Dockerfile Multi-stage**     | Empaquetado minimalista del servidor GNU C                 |   **Soportada**    |     Otto Segura     | Etapa builder con Alpine + `build-base`, runtime limpio con `appuser` no-root (13.8 MB).   |
| **Helm Chart**                 | Chart parametrizado (`deploy/helm/subnet-calculator`)      |   **Soportada**    |     Otto Segura     | Deployment con probes TCP (9666), límites de recursos y Service NodePort (30666).          |
| **Automatización (1 comando)** | Script `scripts/deploy.sh` y `cleanup.sh`                  |   **Soportada**    |     Otto Segura     | Inicializa clúster local, compila imagen, inyecta a Kubernetes y despliega con Helm.       |
| **Parser Léxico y Sintáctico** | Análisis léxico, sintáctico y semántico de primitivas Telnet, conversión y validación de direcciones IPv4 y máscaras, extracción de parámetros y despacho a módulos de cálculo. | **Soportada** | Alexandra De Gracia | Implementación de las validaciones sintácticas, extracción de parámetros, catálogo de errores, despacho y batería de pruebas completada mediante  |
| **Conversión y validación IPv4** | Conversión de direcciones IPv4 en notación decimal punteada a enteros de 32 bits y validación de rangos y formatos. | **Soportada** | Alexandra De Gracia | Valida octetos en `[0,255]`, rechaza formatos inválidos y realiza la conversión a `uint32_t`. |
| **Conversión y validación de máscaras** | Soporte e intercambio entre notación CIDR (`/bits`) y máscara decimal punteada (`X.X.X.X`). | **Soportada** | Alexandra De Gracia | Incluye validación de contigüidad de máscaras y conversión bidireccional. |
| **Extracción de parámetros por comando** | Identificación y extracción de parámetros `IP`, `MASK`, `NETWORK NUMBER`, `NUMBER`, `SIZE`, `PRIORITY` y `DEBUG` según la primitiva. | **Soportada** | Alexandra De Gracia | la estructura `extracted_params_t`, incluyendo parámetros opcionales y valores por defecto. |
| **Catálogo de mensajes de error** | Centralización de mensajes descriptivos para parámetros faltantes, máscaras inválidas, IP inválida y sintaxis desconocida. | **Soportada** | Alexandra De Gracia | Mediante `error_catalog.c`, reemplazando mensajes genéricos por errores específicos. |
| **Despacho a los módulos de cálculo** | Envío de argumentos validados y convertidos hacia las funciones de cálculo de subredes y enrutamiento. | **Soportada** | Alexandra De Gracia | Mediante `dispatch_command_line`, conectando el parser con los módulos de cálculo. |
| **Pruebas sintácticas** | Batería de pruebas para comandos válidos, combinaciones inválidas y valores extremos. | **Soportada** | Alexandra De Gracia |  **75/75 pruebas**, incluyendo 25 casos válidos, 30 inválidos y 20 valores extremos. |

---

## 6. Conclusiones
El uso de una tabla opaca y una lista enlazada permite administrar rutas sin exponer la estructura interna ni al servidor.

El algoritmo de Longest Prefix Match permite seleccionar correctamente la ruta más específica cuando existen redes superpuestas.

Abstraer las direcciones IPv4 y las mascaras de red como enteros sin signo usando operaciones bitwise permite hacer los calculos de subred en tiempo de O(1) sin sobrecosto de memoria, lo que demuestra que operar a nivel de bits es cuantitativa y cualitativamente superior a procesar o manipular las cadenas. 

Validación estricta en dos fases y aislamiento de los motores de cálculo:
Separar el análisis léxico/sintáctico (tokenización y orden de parámetros) de la validación semántica de valores (comprobación de octetos, contigüidad de máscaras y rangos CIDR) permitió que el catálogo de errores centralizado entregue retroalimentación precisa al usuario sin delegar validaciones ambiguas a los módulos de cálculo y enrutamiento. Esta barrera de validación garantiza que las funciones de cómputo reciban únicamente tipos nativos limpios, evitando desbordamientos o estados indefinidos en el despacho.


El diseño del modulo de cálculo calculo_subredes.h aislado de red TCP y del parser facilita la ejecución de las pruebas unitarias deterministas y automatizadas, la arquitectura garantizó que los componentes se pudieran integrar con el dispatcher.c y el server.c sin que haya dependencias cruzadas ni efectos secundarios. 

 El desarrollo del módulo Parser del protocolo y validación de sintaxis establece una capa de entrada crítica que garantiza la descomposición léxica de las primitivas en parser.c, la validación precisa de direcciones IPv4 y máscaras en notaciones CIDR y decimal punteada, la extracción de tokens clave (IP, MASK, NUMBER, SIZE, PRIORITY, DEBUG) y el manejo centralizado de errores mediante un catálogo descriptivo, asegurando que solo argumentos estructuralmente válidos sean despachados a los módulos de cálculo de subredes y enrutamiento.

Implementar un multi-stage build en Docker para compilar código GNU C permite separar estrictamente el entorno de desarrollo (compilador gcc, make y bibliotecas de desarrollo) del entorno de ejecución, logrando una imagen final minimalista de apenas 13.8 MB basada en Alpine Linux; esto no solo reduce drásticamente el consumo de recursos y el tiempo de transferencia en el clúster, sino que minimiza la superficie de ataque y fortalece la seguridad al ejecutar el servicio bajo un usuario no privilegiado (appuser).

La orquestación del servidor mediante un Helm Chart parametrizado junto con sondas de salud (livenessProbe y readinessProbe) basadas en sockets TCP nativos (tcpSocket al puerto 9666) garantiza alta disponibilidad y resiliencia en Kubernetes; el orquestador valida continuamente que el socket del servidor Telnet esté aceptando conexiones activas y permite desacoplar los parámetros de infraestructura (puerto NodePort 30666, límites de memoria y réplicas) sin modificar el código fuente de la aplicación.

Separar el servidor del parser mediante una única función, `handle_command`, permitió construir y probar la parte de red antes de que existieran los demás módulos, y después integrarlos sin cambiar el servidor.

Con `poll()` en un solo hilo se atienden varias sesiones simultáneas sin mecanismos de sincronización. Como los cálculos son muy rápidos, ningún cliente nota que los comandos se ejecutan uno a la vez, y la tabla de rutas compartida queda protegida de forma natural.

---

## 7. Recomendaciones
Antes de integrar nuevas funcionalidades de routing con el parser, se recomienda probarlas primero mediante prueba_routing.c.

Se recomienda conservar las validaciones de prefijo, capacidad y memoria, aunque el parser también valide los datos. La doble frontera protege el contrato público frente a llamadas incorrectas desde otros módulos.

Al calcular la cantidad de los hosts usables y el rango de direcciones, se recomienda implementar validaciones condicionales explicitas para prefijos como /31 y /32, aplicar ciegamente la formula en estos casos generar desbordamientos negativos, por lo que controlar los límites a nivel de código previene comportamientos indefinidos y garantiza que la respuesta sea consistente a cualquier mascara. 

Al transformar las mascaras en decimal a CIDR, se recomienda que se valide explicitamente los bits 1 que sean continuos de izquierda a derecha, lo que previene que mascaras binarias invalidas o no contiguas sean procesadas incorrectamente por el motor de calculo, regresando un mensaje de error al cliente. 

Para mitigar discrepancias entre la especificación del protocolo, los casos de prueba sintácticos y los argumentos esperados por los despachadores (como ocurrió con variaciones entre DEST vs. NETWORK NUMBER o flags de DEBUG), se recomienda formalizar la sintaxis de las primitivas mediante tablas declarativas de comandos o estructuras de metadatos (especificando parámetros obligatorios, opcionales, tipos esperados y valores por defecto). Esto elimina la validación condicional dispersa en el código, reduce falsos negativos en las pruebas extremas y simplifica la incorporación de nuevas primitivas al protocolo.


Al abordar la implementación de manera secuencial iniciando por el tokenizador de primitivas y la conversión de formatos de red, garantizar un riguroso control de límites de memoria durante la extracción de tokens para prevenir desbordamientos de búfer, y ejecutar tempranamente la batería de pruebas sintácticas con combinaciones inválidas y valores extremos antes de conectar el despacho definitivo hacia los módulos de cálculo.

En entornos locales de desarrollo con Kubernetes (como Kind o Minikube), se recomienda inyectar las imágenes de Docker directamente a los nodos del clúster (kind load docker-image o minikube image load) y configurar en Helm la política imagePullPolicy: IfNotPresent o Never; esto evita fallos de despliegue (ImagePullBackOff) derivados de intentos fallidos de descargar imágenes privadas o locales desde registros externos como Docker Hub.

Para el despliegue de servicios TCP con sockets crudos (como servidores Telnet) en clústeres locales, se recomienda preconfigurar el mapeo de puertos del nodo en la definición del clúster (mapeando el hostPort directamente al containerPort del NodePort, ej. 30666); esto garantiza el acceso directo desde localhost en la máquina anfitriona sin requerir túneles adicionales ni redirecciones manuales de puertos durante la evaluación.

Se recomienda tratar todo lo que llega por el socket como bytes crudos y no como texto: `telnet` envía negociación y fines de línea propios, y TCP puede partir o juntar comandos. Conviene limpiar la entrada en un solo lugar, antes del parser, y cubrir ese saneamiento con pruebas unitarias.

En cualquier servidor que se vaya a desplegar en contenedores, se recomienda manejar `SIGTERM` y cerrar las conexiones de forma ordenada, porque Kubernetes envía esa señal al detener un pod y, si se ignora, termina el proceso a la fuerza tras el período de gracia.