# Estado de Funcionalidades – Calculadora de Subredes IPv4

En cumplimiento con los requerimientos de la evaluación de la Tarea Corta 2, a continuación se detalla el estado actual de cada funcionalidad, indicando si se encuentra soportada, en integración o no soportada en el servidor GNU C y su infraestructura.

---

## 1. Primitivas del Protocolo Telnet

|   #    | Primitiva / Funcionalidad        | Sintaxis de Ejemplo                                                    |       Estado       | Responsable | Observaciones                                                                      |
| :----: | :------------------------------- | :--------------------------------------------------------------------- | :----------------: | :---------: | :--------------------------------------------------------------------------------- |
| **1**  | Obtener dirección de broadcast   | `GET BROADCAST IP 10.8.2.5 MASK /29`                                   |   **Soportada**    | Rafael Araya  | Implementada con álgebra bitwise en `calculo_subredes.c`. Pruebas unitarias pasan. |
| **2**  | Obtener número de red            | `GET NETWORK NUMBER IP 10.8.2.5 MASK /29`                              |   **Soportada**    | Rafael Araya  | Implementada con álgebra bitwise en `calculo_subredes.c`. Pruebas unitarias pasan. |
| **3**  | Obtener rango de hosts           | `GET HOSTS RANGE IP 10.8.2.5 MASK /29`                                 |   **Soportada**    | Rafael Araya  | Formato `X.X.X.{primero-ultimo}` implementado en `calculo_subredes.c`.             |
| **4**  | Obtener máscara o prefijo CIDR   | `GET NETMASK MASK /27`<br>`GET NETMASK MASK 255.255.255.0`             |   **Soportada**    | Rafael Araya  | Conversión bidireccional prefijo $\leftrightarrow$ máscara decimal punteada.       |
| **5**  | Obtener cantidad de hosts        | `GET HOSTS COUNT MASK /29`                                             |   **Soportada**    | Rafael Araya  | Cálculo con $2^{32-prefix}-2$, manejando casos especiales (/31 y /32).             |
| **6**  | Obtener clase de dirección IP    | `GET IP CLASS 10.8.2.5`                                                |   **Soportada**    | Rafael Araya  | Clasificación estricta de Clases A, B, C, D y E mediante bits más significativos.  |
| **7**  | Generar subredes aleatorias      | `GET RANDOM SUBNETS NETWORK NUMBER 10.0.0.0 MASK /8 NUMBER 3 SIZE /24` | **En Integración** |  Isaac Rojas  | Lógica de generación y algoritmo de no-solapamiento de subredes en desarrollo.     |
| **8**  | Agregar rutas a tabla dinámica   | `SET ROUTE R1 NETWORK NUMBER 192.168.1.0 MASK /24 PRIORITY 10`         | **En Integración** |  Isaac Rojas  | Contrato público definido en `routing.h`; estructura en memoria dinámica.          |
| **9**  | Eliminar rutas de tabla dinámica | `DEL ROUTE R1`                                                         | **En Integración** |  Isaac Rojas  | Contrato público definido en `routing.h`; liberación de memoria.                   |
| **10** | Realizar routing de IP y DEBUG   | `ROUTE IP 8.8.8.8`<br>`ROUTE IP 8.8.8.8 DEBUG true`                    | **En Integración** |  Isaac Rojas  | Longest Prefix Match con resolución de empates por prioridad y traza debug.        |

---

## 2. Componentes de Arquitectura e Infraestructura

| Componente                     | Descripción                                                |       Estado       |     Responsable     | Observaciones                                                                              |
| :----------------------------- | :--------------------------------------------------------- | :----------------: | :-----------------: | :----------------------------------------------------------------------------------------- |
| **Servidor TCP Concurrente**   | Socket en puerto 9666 con `poll()`, no bloqueante          |   **Soportada**    |    Luis Delgado     | Atiende múltiples clientes Telnet concurrentes en un solo hilo. Maneja `SIGINT`/`SIGTERM`. |
| **Saneamiento Telnet**         | Filtrado de secuencias IAC, caracteres de control y `\r\n` |   **Soportada**    |    Luis Delgado     | Buffer de entrada saneado y pruebas unitarias passing.                                     |
| **Parser Léxico y Sintáctico** | Tokenizador de primitivas y validación de IPs y máscaras   | **En Integración** | Alexandra De Gracia | Implementación de validaciones sintácticas completada; pendiente integrar despacho en C.  |
| **Dockerfile Multi-stage**     | Empaquetado minimalista del servidor GNU C                 |   **Soportada**    |     Otto Segura     | Etapa builder con Alpine + `build-base`, runtime limpio con `appuser` no-root (13.8 MB).   |
| **Helm Chart**                 | Chart parametrizado (`deploy/helm/subnet-calculator`)      |   **Soportada**    |     Otto Segura     | Deployment con probes TCP (9666), límites de recursos y Service NodePort (30666).          |
| **Automatización (1 comando)** | Script `scripts/deploy.sh` y `cleanup.sh`                  |   **Soportada**    |     Otto Segura     | Inicializa clúster local, compila imagen, inyecta a Kubernetes y despliega con Helm.       |

---

## 3. Resumen Cuantitativo

- **Total de primitivas:** 10 (6 Soportadas en `main`, 4 En Integración).
- **Total componentes de infraestructura:** 6 (5 Soportados, 1 En Integración).
- **Cumplimiento de automatización:** 100% automatizado mediante script ejecutable sin pasos manuales.
