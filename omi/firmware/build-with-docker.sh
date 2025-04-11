#!/bin/bash
set -e

# Navigate to script directory
cd "$(dirname "$0")"

# Build the Docker image if needed
docker-compose build

# Get project directory and board name from arguments
PROJECT_DIR=${1:-"firmware/app"}
BOARD=${2:-"xiao_nrf52840_ble_sense"}

echo "Building project from: $PROJECT_DIR for board: $BOARD"

# Run the build process
docker-compose run --rm nrf-builder /bin/bash -c "build-project.sh /workdir/project/$PROJECT_DIR $BOARD && package-firmware.sh"

echo "Build completed. Firmware artifacts are in the build directory."