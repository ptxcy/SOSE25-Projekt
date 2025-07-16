#!/bin/bash

PRIVATE_KEY=$1

echo "Pipeline is running: Stop Docker, clean up, and update repository"
echo "$PRIVATE_KEY" | wc -c
echo "$HOME"
ls -la /home/runner
echo "$PRIVATE_KEY" > ~/.ssh/temp_key.pem
chmod 400 ~/.ssh/temp_key.pem

ssh -o StrictHostKeyChecking=no -i ~/.ssh/temp_key.pem ec2-user@ec2-18-196-124-42.eu-central-1.compute.amazonaws.com << EOF
  echo "Server connected via SSH"

  echo "Stopping all running Docker containers, except mongodb-projekt..."
  docker ps --format "{{.ID}} {{.Names}}" | grep -v "mongodb-projekt" | awk '{print $1}' | xargs -r docker stop

  echo "Cleaning up Docker, except volumes related to mongodb-projekt..."
  docker system prune -a -f
  docker volume prune -f
  docker network prune -f
  docker rm -f $(docker ps -aq)

  echo "Updating repository..."
  cd /home/ec2-user/SOSE25-Projekt
  git fetch origin
  git reset --hard origin/main
  git pull origin main

  echo "Server finished setup"
EOF

rm -f ~/.ssh/temp_key.pem
