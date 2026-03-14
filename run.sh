#!/bin/bash

# remove previous setup
rm -rf build
rm -rf web/node_modules
mkdir -p build

# package checks
if [[ ! -x "$(command -v cmake)" ]]; then
    echo "CMake must be installed to run the project"
    exit 1
fi

if [[ ! -x "$(command -v make)" ]]; then
    echo "make must be installed to run the project"
    exit 1
fi

if [[ ! -x "$(command -v npm)" ]]; then
    echo "npm must be installed to run the project"
    exit 1
fi

# run raytracer endpoint
cd build
cmake .. -DDEV_FILE=test_net.c -DUSE_DEBUG_RAYLIB=ON && make && ./at &

# run web server
cd ../web
npm i && npm run dev
