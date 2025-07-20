#!/bin/bash

PRIVATE_KEY=$1

echo "Pipeline is running: Configuring Nginx to serve Doxygen HTML directly"
echo "$PRIVATE_KEY" > ~/.ssh/temp_key.pem
chmod 400 ~/.ssh/temp_key.pem

ssh -o StrictHostKeyChecking=no -i ~/.ssh/temp_key.pem ec2-user@ec2-18-196-124-42.eu-central-1.compute.amazonaws.com << EOF
  echo "Server connected via SSH"

  # Nginx-Konfiguration anlegen (mit alias!)
  sudo tee /etc/nginx/conf.d/doxygen.conf > /dev/null << 'NGINXCONF'
server {
    listen 80;
    server_name localhost;

    location /doxygen/ {
        alias /home/ec2-user/SOSE25-Projekt/client/doc/doxygen-doc/html/;
        index index.html;
        try_files \$uri \$uri/ =404;
    }
}
NGINXCONF

  # Zugriffsrechte sicherstellen
  sudo chmod o+x /home/ec2-user/SOSE25-Projekt
  sudo chmod -R o+rX /home/ec2-user/SOSE25-Projekt/client/doc/doxygen-doc/html

  # nginx starten, falls nicht aktiv
  sudo systemctl is-active nginx || sudo systemctl start nginx

  # nginx testen und neu laden
  sudo nginx -t && sudo systemctl reload nginx
  echo "Nginx wurde konfiguriert und neu geladen"
EOF

rm -f ~/.ssh/temp_key.pem
