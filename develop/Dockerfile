# Use an official Ubuntu as a parent image
FROM ubuntu:latest

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
# Use an official Ubuntu as a parent image
FROM ubuntu:latest

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    python3-pip \
    wget \
    clang \
 && rm -rf /var/lib/apt/lists/*

# Install Conan
RUN pip3 install conan==1.*

# Install additional GCC versions (change as needed)
RUN apt-get update && apt-get install -y \
    g++-11 \
 && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 100

# Set Clang as the default
RUN update-alternatives --install /usr/bin/cc cc /usr/bin/clang 100
RUN update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 100 

# Set the working directory
WORKDIR /workspace

# Define the entry point
CMD ["/bin/bash"]