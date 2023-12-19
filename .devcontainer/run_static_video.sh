#!/bin/bash

# Build the Docker image
docker build -t run-static-video-image $(pwd)/.devcontainer/.

XSOCK=/tmp/.X11-unix
XAUTH=/tmp/.docker.xauth
xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | sudo xauth -f $XAUTH nmerge -
sudo chmod 777 $XAUTH
X11PORT=`echo $DISPLAY | sed 's/^[^:]*:\([^\.]\+\).*/\1/'`
TCPPORT=`expr 6000 + $X11PORT`
sudo ufw allow from 172.17.0.0/16 to any port $TCPPORT proto tcp 

# Run the Docker container interactively with the /project directory mounted
sudo docker run -it --rm -v $(pwd):/workspace  -e EXECUTE_BIN_BASH=1 -e DISPLAY=$DISPLAY -v $XAUTH:$XAUTH \
   -e XAUTHORITY=$XAUTH --net=host run-static-video-image