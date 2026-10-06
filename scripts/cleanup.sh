#!/usr/bin/env bash
set -euo pipefail

RELEASE_NAME="subnet-calculator"

echo "Desinstalando release de Helm: ${RELEASE_NAME}..."
if helm status "${RELEASE_NAME}" >/dev/null 2>&1; then
    helm uninstall "${RELEASE_NAME}"
    echo "Release ${RELEASE_NAME} desinstalado con éxito."
else
    echo "El release ${RELEASE_NAME} no se encuentra instalado."
fi
