{{/*
Expand the name of the chart.
*/}}
{{- define "subnet-calculator.name" -}}
{{- default .Chart.Name .Values.nameOverride | trunc 63 | trimSuffix "-" }}
{{- end }}

{{/*
Create a default fully qualified app name.
*/}}
{{- define "subnet-calculator.fullname" -}}
{{- if .Values.fullnameOverride }}
{{- .Values.fullnameOverride | trunc 63 | trimSuffix "-" }}
{{- else }}
{{- $name := default .Chart.Name .Values.nameOverride }}
{{- if contains $name .Release.Name }}
{{- .Release.Name | trunc 63 | trimSuffix "-" }}
{{- end }}
{{- printf "%s-%s" .Release.Name $name | trunc 63 | trimSuffix "-" }}
{{- end }}
{{- end }}

{{/*
Common labels
*/}}
{{- define "subnet-calculator.labels" -}}
helm.sh/chart: {{ include "subnet-calculator.name" . }}-{{ .Chart.Version | replace "+" "_" }}
{{ include "subnet-calculator.selectorLabels" . }}
app.kubernetes.io/version: {{ .Chart.AppVersion | quote }}
app.kubernetes.io/managed-by: {{ .Release.Service }}
{{- end }}

{{/*
Selector labels
*/}}
{{- define "subnet-calculator.selectorLabels" -}}
app.kubernetes.io/name: {{ include "subnet-calculator.name" . }}
app.kubernetes.io/instance: {{ .Release.Name }}
{{- end }}
