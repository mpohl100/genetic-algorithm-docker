#!/bin/bash

# Build the Docker image
docker build -t develop-image $(pwd)/.devcontainer/.

# Run the Docker container interactively with the /project directory mounted
docker run -it -v $(pwd):/workspace --device /dev/video0:/dev/video0 -e DISPLAY=$DISPLAY -e EXECUTE_BIN_BASH=1 develop-image
