Isaac Rojas Robles - 2022104884
Rafael Araya Álvarez - 2023029575
Otto Segura Ruiz - 2020426226
Luis Diego Delgado Muñoz - 2020030408 
Alexandra De Gracia - 2026801601

### Primitivas de momento
1. `GETBROADCAST IP`: Calculo de direccion IP de broadcast.
2. `GET NETWORK NUMBER IP`: Calculo de la direccion de red.
3. `GET HOSTS RANGE IP`: Rango de hosts utilizables (`A.B.C.{X-Y}`).
4. `GET NETMASK MASK`: Conversion bidireccional entre prefijo CIDR `/bits` y mascara decimal `X.X.X.X`.
5. `GET HOSTS COUNT MASK`: Cantidad de hosts utilizables.
6. `GET IP CLASS`: Clasificacion estricta de clase de IP (Clase A, B, C, D o E).

### Requisitos Pruebas Unitarias
- Compilador `gcc` (soporte para C11/GNU11)
- `make`

### Instrucciones para ejecutar las pruebas

1. Compilar y ejecutar las pruebas unitarias
   ```bash
   make test
   ```

2. Compilar el binario
   ```bash
   make
   ```

3. Ejecutar las pruebas
   ```bash
   ./prueba_calculo_subredes
   ```

4. Limpiar archivos innecesarios
   ```bash
   make clean
   ```