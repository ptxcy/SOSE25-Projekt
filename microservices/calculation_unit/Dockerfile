FROM --platform=linux/amd64 ubuntu:24.04

WORKDIR /app

# Installiere fehlende Abhängigkeiten und stelle sicher, dass die Bibliotheken unter /usr/lib landen
RUN apt update && apt install -y \
    gcc \
    libc6 \
    libc6-dev \
    libgcc-s1 \
    build-essential \
    ca-certificates && \
    ldconfig

COPY . .

#CMD ["sh", "-c", "ls -l /usr/lib && ./target/release/calculation_unit"]
CMD ["./target/release/calculation_unit"]
