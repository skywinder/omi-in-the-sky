FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ARG DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    git \
    cmake \
    ninja-build \
    gperf \
    ccache \
    dfu-util \
    device-tree-compiler \
    wget \
    python3-pip \
    python3-setuptools \
    python3-wheel \
    python3-dev \
    python3-venv \
    xz-utils \
    file \
    make \
    gcc \
    g++ \
    libsdl2-dev \
    libmagic1 \
    unzip \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Install west and other Python dependencies
RUN pip3 install --no-cache-dir west adafruit-nrfutil

# Set up working directory
WORKDIR /workdir

# Create a script to initialize nRF Connect SDK
RUN echo '#!/bin/bash\n\
if [ ! -d "ncs" ]; then\n\
    # Initialize west workspace\n\
    west init -m https://github.com/nrfconnect/sdk-nrf --mr v2.7.0 ncs\n\
    cd ncs\n\
    west update\n\
    west zephyr-export\n\
    pip3 install --no-cache-dir -r zephyr/scripts/requirements.txt\n\
    pip3 install --no-cache-dir -r nrf/scripts/requirements.txt\n\
    pip3 install --no-cache-dir -r bootloader/mcuboot/scripts/requirements.txt\n\
fi\n\
' > /usr/local/bin/init-ncs.sh && chmod +x /usr/local/bin/init-ncs.sh

# Create a script to build projects
RUN echo '#!/bin/bash\n\
cd /workdir/ncs\n\
\n\
# Check if project directory was provided\n\
if [ -z "$1" ]; then\n\
    echo "Error: Project directory not specified"\n\
    echo "Usage: build-project.sh <project_directory> <board_name>"\n\
    exit 1\n\
fi\n\
\n\
PROJECT_DIR=$1\n\
BOARD=${2:-xiao_nrf52840_ble_sense}\n\
\n\
# Build the project\n\
west build -p always -b $BOARD $PROJECT_DIR\n\
\n\
# Output build artifacts\n\
echo "Build completed. Files are located in /workdir/ncs/build/zephyr/"\n\
' > /usr/local/bin/build-project.sh && chmod +x /usr/local/bin/build-project.sh

# Create a script to package the firmware
RUN echo '#!/bin/bash\n\
cd /workdir/ncs/build/zephyr\n\
\n\
# Generate OTA package\n\
adafruit-nrfutil dfu genpkg --dev-type 0x0052 --dev-revision 0xCE68 --application zephyr.hex zephyr.zip\n\
echo "OTA package created: /workdir/ncs/build/zephyr/zephyr.zip"\n\
' > /usr/local/bin/package-firmware.sh && chmod +x /usr/local/bin/package-firmware.sh

# Create an entrypoint script
RUN echo '#!/bin/bash\n\
# Initialize NCS if not already done\n\
init-ncs.sh\n\
\n\
# Execute the command provided to docker run\n\
exec "$@"\n\
' > /usr/local/bin/entrypoint.sh && chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["/bin/bash"]