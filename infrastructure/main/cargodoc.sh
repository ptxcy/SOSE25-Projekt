
#!/bin/bash

PRIVATE_KEY=$1

echo "Pipeline is running: Stop Docker, clean up, and update repository"
echo "$PRIVATE_KEY" | wc -c
echo "$HOME"
ls -la /home/runner
echo "$PRIVATE_KEY" > ~/.ssh/temp_key.pem
chmod 400 ~/.ssh/temp_key.pem

ssh -o StrictHostKeyChecking=no -i ~/.ssh/temp_key.pem ec2-user@ec2-3-124-115-76.eu-central-1.compute.amazonaws.com << EOF
  cd /home/ec2-user/SOSE25-Projekt
  cd ./microservices/calculation_unit && cargo doc && cd ..
  cd ./pong && cargo doc
EOF

rm -f ~/.ssh/temp_key.pem
