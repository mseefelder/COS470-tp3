#!/bin/bash

echo "Creating virtualenv..."
virtualenv .

echo "Activating virtualenv..."
source bin/activate

echo "Installing pip dependencies (crossbar)..."
pip install -r pip_requirements.txt


echo "Entering ex1..."
cd ex1

echo "Setting up crossbar structure..."
mkdir crossbar_server

echo "Entering crossbar_server..."
cd crossbar_server
crossbar init

echo "Going back to ex1..."
cd ..

echo "Installing npm dependencies (autobahn and when)..."
npm install autobahn
npm install when

echo "Going back to root..."
cd ..