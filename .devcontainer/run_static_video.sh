#!/bin/bash

# Build the Docker image
docker build -t run-static-video-image $(pwd)/.devcontainer/.

# Run the Docker container interactively with the /project directory mounted
docker run -it -v $(pwd):/workspace  -e EXECUTE_BIN_BASH=1 -e DISPLAY=$DISPLAY run-static-video-image