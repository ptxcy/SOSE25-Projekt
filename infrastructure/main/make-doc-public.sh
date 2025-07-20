#!/bin/bash

PRIVATE_KEY=$1

echo "Pipeline is running: Configuring Nginx to serve multiple Doxygen HTML docs"
echo "$PRIVATE_KEY" > ~/.ssh/temp_key.pem
chmod 400 ~/.ssh/temp_key.pem

ssh -o StrictHostKeyChecking=no -i ~/.ssh/temp_key.pem ec2-user@ec2-18-196-124-42.eu-central-1.compute.amazonaws.com << EOF
  echo "Server connected via SSH"

  # Nginx-Konfiguration mit mehreren Aliases
  sudo tee /etc/nginx/conf.d/doxygen.conf > /dev/null << 'NGINXCONF'
server {
    listen 80;
    server_name localhost;

    location /doxygen/ {
        alias /home/ec2-user/SOSE25-Projekt/client/doc/doxygen-doc/html/;
        index index.html;
        try_files \$uri \$uri/ =404;
    }

    location /pong-doc/ {
        alias /home/ec2-user/SOSE25-Projekt/microservices/pong/target/doc/pong/;
        index index.html;
        try_files \$uri \$uri/ =404;
    }

    location /static.files/ {
        alias /home/ec2-user/SOSE25-Projekt/microservices/pong/target/doc/static.files/;
        try_files \$uri \$uri/ =404;
    }

    location /crates.js {
        alias /home/ec2-user/SOSE25-Projekt/microservices/pong/target/doc/crates.js;
    }
}
NGINXCONF

  # Zugriffsrechte für beide Doku-Verzeichnisse sicherstellen
  sudo chmod o+x /home/ec2-user/SOSE25-Projekt
  sudo chmod -R o+rX /home/ec2-user/SOSE25-Projekt/client/doc/doxygen-doc/html
  sudo chmod -R o+rX /home/ec2-user/SOSE25-Projekt/microservices/pong/target/doc/pong

  # nginx starten oder neu laden
  sudo systemctl is-active nginx || sudo systemctl start nginx
  sudo nginx -t && sudo systemctl reload nginx
  echo "Nginx wurde für beide Dokumentationen konfiguriert und neu geladen"
EOF

rm -f ~/.ssh/temp_key.pem
