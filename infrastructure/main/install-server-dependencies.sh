#!/bin/bash

PRIVATE_KEY=$1

echo "Pipeline is running install server dependencies check"
echo "$PRIVATE_KEY" | wc -c
echo "$HOME"
ls -la /home/runner
echo "$PRIVATE_KEY" > ~/.ssh/temp_key.pem
chmod 400 ~/.ssh/temp_key.pem

ssh -o StrictHostKeyChecking=no -i ~/.ssh/temp_key.pem ec2-user@ec2-18-196-124-42.eu-central-1.compute.amazonaws.com << EOF
  echo "Server connected via SSH"

  # Git Installation prüfen und ggf. installieren
  if ! command -v git &> /dev/null; then
    echo "Git nicht gefunden, Installation beginnt..."
    sudo yum install -y git
  else
    echo "Git ist bereits installiert!"
  fi

  # Docker Installation prüfen und ggf. installieren
  if ! command -v docker &> /dev/null; then
    echo "Docker nicht gefunden, Installation beginnt..."
    sudo yum update -y
    sudo yum install -y docker
    sudo systemctl enable docker
    sudo systemctl start docker
  else
    echo "Docker ist bereits installiert!"
  fi

  if ! command -v doxygen &> /dev/null; then
    echo "Doxygen nicht gefunden, Installation beginnt..."
    sudo yum install -y doxygen
  else
    echo "Doxygen ist bereits installiert!"
  fi

  # Rust (Cargo) Installation prüfen und ggf. installieren
  if ! command -v cargo &> /dev/null; then
    echo "Cargo (Rust) nicht gefunden, Installation beginnt..."
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
    source "$HOME/.cargo/env"
  else
    echo "Cargo (Rust) ist bereits installiert!"
  fi

  # Entwickler-Toolset installieren
  sudo dnf groupinstall -y "Development Tools"

  # Zusätzliche Einzelpakete
  sudo dnf install -y gcc cmake make openssl-devel pkgconf curl file

  sudo usermod -aG docker ec2-user
  # Docker Compose Installation prüfen und ggf. installieren
  if ! command -v docker-compose &> /dev/null; then
    echo "Docker Compose nicht gefunden, Installation beginnt..."
    sudo curl -L "https://github.com/docker/compose/releases/latest/download/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
    sudo chmod +x /usr/local/bin/docker-compose
  else
    echo "Docker Compose ist bereits installiert!"
  fi

  echo "Server finished install server dependencies check"
EOF

rm -f ~/.ssh/temp_key.pem
