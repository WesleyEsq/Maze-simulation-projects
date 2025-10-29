#!/bin/bash

# ---------------------

echo "Instalation underway, this script should be done with a sudo..."

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")


echo "----------------------------------"
echo "1- Installing dependencies..."
echo "----------------------------------"
echo "This could take from a few minutes to nothing..."
sudo apt-get install -y build-essential libgtk-3-dev

# Verificar si la instalación de dependencias fue exitosa
if [ $? -ne 0 ]; then
    echo "Error: A dependency failed to get install"
    echo "Please try again or install the dependency that failed manually"
    exit 1
fi

echo "Dependencies were done correctly"

echo " "
echo "----------------------------------"
echo "Step2- Compiling the code"
echo "----------------------------------"
echo "The executable will be on'$SCRIPT_DIR/maze/maiz'..."
gcc -o "$SCRIPT_DIR/maze/maiz" "$SCRIPT_DIR/maze/gui.c" -Wall -pthread `pkg-config --cflags --libs gtk+-3.0` -export-dynamic -lm -D_DEFAULT_SOURCE


if [ $? -ne 0 ]; then
    echo "Error: Compilation failed"
    exit 1
fi
