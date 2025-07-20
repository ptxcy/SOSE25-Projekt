#!/bin/bash

# Script to generate documentation for the C++ client project on the server

PRIVATE_KEY=$1

if [ -z "$PRIVATE_KEY" ]; then
    echo "Usage: $0 <PRIVATE_KEY>"
    exit 1
fi

echo "$PRIVATE_KEY" > ~/.ssh/temp_key.pem
chmod 400 ~/.ssh/temp_key.pem

ssh -o StrictHostKeyChecking=no -i ~/.ssh/temp_key.pem ec2-user@ec2-3-124-115-76.eu-central-1.compute.amazonaws.com << EOF
  cd /home/ec2-user/SOSE25-Projekt/client
  if ! command -v doxygen &> /dev/null; then
    echo "Doxygen is not installed. Please install it first."
    exit 1
  fi
  if [ ! -f "Doxyfile" ]; then
    echo "Doxyfile not found. Please run this script from the client directory."
    exit 1
  fi
  cd doc
  mkdir -p doxygen-doc
  cd ..
  doxygen Doxyfile
  if [ -d "doc/doxygen-doc/html" ]; then
    echo "Documentation generated successfully!"
    echo "Open doc/doxygen-doc/html/index.html in your browser to view the documentation"
  else
    echo "Documentation generation failed. Check the output above for errors."
    exit 1
  fi
EOF

rm -f ~/.ssh/temp_key.pem
