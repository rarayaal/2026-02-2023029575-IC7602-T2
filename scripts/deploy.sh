#!/usr/bin/env bash
set -euo pipefail

echo "==> Verificando herramientas..."
command -v docker >/dev/null 2>&1 || { echo "Error: docker no instalado"; exit 1; }
command -v kubectl >/dev/null 2>&1 || { echo "Error: kubectl no instalado"; exit 1; }
command -v helm >/dev/null 2>&1 || { echo "Error: helm no instalado"; exit 1; }

echo "==> Verificando cluster de Kubernetes..."
CLUSTER_TYPE=""
if kubectl cluster-info >/dev/null 2>&1; then
    if command -v minikube >/dev/null 2>&1 && minikube status >/dev/null 2>&1; then
        CLUSTER_TYPE="minikube"
    elif command -v kind >/dev/null 2>&1 && kind get clusters 2>/dev/null | grep -q "kind"; then
        CLUSTER_TYPE="kind"
    fi
else
    if command -v kind >/dev/null 2>&1; then
        echo "Iniciando cluster kind..."
        cat <<EOF | kind create cluster --config=-
kind: Cluster
apiVersion: kind.x-k8s.io/v1alpha4
nodes:
- role: control-plane
  extraPortMappings:
  - containerPort: 30666
    hostPort: 30666
    protocol: TCP
EOF
        CLUSTER_TYPE="kind"
    elif command -v minikube >/dev/null 2>&1; then
        echo "Iniciando minikube..."
        minikube start
        CLUSTER_TYPE="minikube"
    else
        echo "Error: No hay cluster activo ni kind/minikube instalado."
        exit 1
    fi
fi

echo "==> Construyendo imagen Docker..."
docker build -t subnet-calculator:latest .

echo "==> Cargando imagen en cluster..."
if [ "${CLUSTER_TYPE}" = "kind" ]; then
    kind load docker-image subnet-calculator:latest
elif [ "${CLUSTER_TYPE}" = "minikube" ]; then
    minikube image load subnet-calculator:latest
fi

echo "==> Desplegando con Helm..."
helm upgrade --install subnet-calculator ./deploy/helm/subnet-calculator \
  --set image.repository="subnet-calculator" \
  --set image.tag="latest" \
  --set image.pullPolicy="Never"

kubectl rollout status deployment/subnet-calculator --timeout=60s

TARGET_HOST="localhost"
[ "${CLUSTER_TYPE}" = "minikube" ] && TARGET_HOST=$(minikube ip)

echo "==> Despliegue listo. Conectate con: telnet ${TARGET_HOST} 30666"
