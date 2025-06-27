#!/bin/bash

# Script to generate documentation for the C++ client project
# Usage: ./generate-docs.sh

echo "🔧 Generating documentation for SOSE25 Game Client..."

# Navigate to client directory
cd "$(dirname "$0")"

# Check if Doxygen is installed
if ! command -v doxygen &> /dev/null; then
    echo "❌ Doxygen is not installed. Please install it first:"
    echo "   brew install doxygen"
    exit 1
fi

# Check if Doxyfile exists
if [ ! -f "Doxyfile" ]; then
    echo "❌ Doxyfile not found. Please run this script from the client directory."
    exit 1
fi

# Create docs directory if it doesn't exist
mkdir -p docs

# Generate documentation
echo "📚 Running Doxygen..."
doxygen Doxyfile

# Check if documentation was generated successfully
if [ -d "docs/html" ]; then
    echo "✅ Documentation generated successfully!"
    echo "📖 Open docs/html/index.html in your browser to view the documentation"
    
    # Ask if user wants to open the documentation
    read -p "🌐 Would you like to open the documentation now? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        open docs/html/index.html
    fi
else
    echo "❌ Documentation generation failed. Check the output above for errors."
    exit 1
fi
