#!/bin/bash

PRIVATE_KEY=$1

echo "Pipeline is running: Stop Docker, clean up, and clone repository"

ssh -o StrictHostKeyChecking=no -i <(echo "$PRIVATE_KEY") ec2-user@ec2-3-66-164-207.eu-central-1.compute.amazonaws.com << EOF
  echo "Server connected via SSH"

  echo "Stopping all running Docker containers..."
  docker ps -q | xargs -r docker stop

  echo "Cleaning up Docker..."
  docker system prune -a -f
  docker volume prune -f
  docker network prune -f
  docker rm -f $(docker ps -aq) || true

  echo "Cloning repository..."
  cd /home/ec2-user
  if [ -d "project-repo" ]; then
    rm -rf project-repo
  fi
  git clone https://github.com/ptxcy/SOSE25-Projekt.git

  echo "Server finished setup"
EOF
