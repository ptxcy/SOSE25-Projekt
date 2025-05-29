#!/bin/bash

PRIVATE_KEY=$1

echo "Pipeline is running: Stop Docker, clean up, and clone repository"
echo "$PRIVATE_KEY" > ~/.ssh/temp_key.pem
chmod 400 ~/.ssh/temp_key.pem

ssh -o StrictHostKeyChecking=no -i ~/.ssh/temp_key.pem ec2-user@ec2-3-66-164-207.eu-central-1.compute.amazonaws.com << EOF
  echo "Server connected via SSH"

  echo "Stopping all running Docker containers..."
  docker ps -q | xargs -r docker stop

  echo "Cleaning up Docker..."
  docker system prune -a -f
  docker volume prune -f
  docker network prune -f
  docker rm -f \$(docker ps -aq) || true

  echo "Cloning repository..."
  cd /home/ec2-user
  if [ -d "SOSE25-Projekt" ]; then
    rm -rf SOSE25-Projekt
  fi
  git clone https://github.com/ptxcy/SOSE25-Projekt.git

  echo "Server finished setup"
EOF

rm -f ~/.ssh/temp_key.pem
