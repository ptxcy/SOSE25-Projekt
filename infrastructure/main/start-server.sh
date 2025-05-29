#!/bin/bash

PRIVATE_KEY=$1

echo "Pipeline is running: Starting server with Docker Compose (profile: deploy)"

ssh -o StrictHostKeyChecking=no -i <(echo "$PRIVATE_KEY") ec2-user@ec2-3-66-164-207.eu-central-1.compute.amazonaws.com << EOF
  echo "Server connected via SSH"

  cd /home/ec2-user/SOSE25-Projekt || exit

  # Starte den Server mit Docker Compose
  echo "Starting Docker Compose with profile 'deploy'..."
  docker-compose --profile deploy up -d

  echo "Server successfully started!"
EOF
