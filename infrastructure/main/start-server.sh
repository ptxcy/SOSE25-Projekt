#!/bin/bash

PRIVATE_KEY=$1

echo "Pipeline is running: Starting server with Docker Compose (profile: deploy)"
echo "$PRIVATE_KEY" > ~/.ssh/temp_key.pem
chmod 400 ~/.ssh/temp_key.pem

ssh -o StrictHostKeyChecking=no -i ~/.ssh/temp_key.pem ec2-user@ec2-18-196-124-42.eu-central-1.compute.amazonaws.com << EOF
  echo "Server connected via SSH"

  cd /home/ec2-user/SOSE25-Projekt || exit

  # Starte den Server mit Docker Compose
  echo "Starting Docker Compose with profile 'deploy'..."
  chmod +x start-backend.sh
  ./start-backend.sh

  echo "Server successfully started!"
EOF

rm -f ~/.ssh/temp_key.pem
