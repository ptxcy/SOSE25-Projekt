@echo off

if "%~1" NEQ "--clean-build" if NOT "%~1" == "" (
  echo Invalid Parameter. Use --clean-build or provide no parameter.
  exit /b 1
)

if "%~1" == "--clean-build" (
  echo Stopping all running Docker containers...
  for /f %%i in ('docker ps -q') do (
    docker stop %%i
  )

  echo Running system prune...
  docker system prune -a -f

  echo All containers stopped and system pruned.
)

echo Starting Dev Environment...
docker compose up -d --build authproxy request-sync calculation_unit mongodb