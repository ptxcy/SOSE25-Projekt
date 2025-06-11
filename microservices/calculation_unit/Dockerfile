FROM --platform=linux/amd64 ubuntu:24.04

WORKDIR /app

# Installiere fehlende Abhängigkeiten
RUN apt update && apt install -y \
    gcc \
    libc6 \
    libc6-dev \
    libgcc-s1 \
    build-essential \
    ca-certificates && \
    ldconfig

COPY . .

# Erhalte SERVICE_PORT aus build-arg und setze ihn als Umgebungsvariable
ARG SERVICE_PORT
ENV SERVICE_PORT=${SERVICE_PORT}
CMD ["sh", "-c", "./target/release/calculation_unit $SERVICE_PORT"]

