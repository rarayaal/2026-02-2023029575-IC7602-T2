# Builder
FROM alpine:3.20 AS builder

# Instalar dependencias
RUN apk add --no-cache build-base

WORKDIR /build

COPY Makefile ./
COPY calculadora_subredes/ ./calculadora_subredes/

# Compilar el binario
RUN make servidor



# Runtime
FROM alpine:3.20

# Crear usuario no privilegiado
RUN addgroup -S appgroup && adduser -S appuser -G appgroup

WORKDIR /app

COPY --from=builder /build/servidor /app/servidor

# Garantizar permisos
RUN chown -R appuser:appgroup /app
EXPOSE 9666
USER appuser
ENTRYPOINT ["/app/servidor"]
