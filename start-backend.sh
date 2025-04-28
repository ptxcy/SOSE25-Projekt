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
docker compose up -d --build authproxy request-sync calculation_unit mongodb
