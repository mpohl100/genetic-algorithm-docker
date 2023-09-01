#!/bin/bash

# Build the Docker image
docker build -t develop-image $(pwd)/.devcontainer/.

# Run the Docker container interactively with the /project directory mounted
docker run -it -v $(pwd):/workspace develop-image