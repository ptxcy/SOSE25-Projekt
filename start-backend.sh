#!/bin/bash

if [ "$1" != "--clean-build" ] && [ ! -z "$1" ]; then
  echo "Invalid Parameter. Use --clean-build or provide no parameter."
  exit 1
fi

if [ "$1" == "--clean-build" ]; then
  echo "Stopping all running Docker containers..."
  docker stop $(docker ps -q)

  echo "Running system prune..."
  docker system prune -a -f

  echo "All containers stopped and system pruned."
fi

echo "Starting Dev Environment..."
docker network create sose25-projekt
docker-compose build 3d-calculation_unit
docker-compose build pong-calculation_unit
docker-compose up -d --build mongodb authproxy
