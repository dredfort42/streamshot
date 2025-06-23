FROM debian:stable-slim AS env

# Install build essentials and any dependencies you need
RUN apt-get update && \
    apt-get install -y build-essential git curl nasm && \
    rm -rf /var/lib/apt/lists/*

# Set work directory
WORKDIR /app

# Copy project files
COPY ./Makefile ./Makefile

RUN mkdir -p /app/saved_files
RUN mkdir -p /app/src

# Prepare the environment
RUN make build_check

# Use a multi-stage build to keep the final image small
FROM env AS runtime

# Set work directory
WORKDIR /app

# Copy project files
COPY ./include ./include
COPY ./src ./src
COPY ./tests ./tests

# Build project
RUN make

# Start your project (replace with your actual start command)
CMD ["./streamshot", "-i", "rtsp://root:pass@81.27.244.138:8554/ch0_unicast_firststream", "-e", "10", "--output-format", "jpg", "-o", "/app/saved_files/test.jpg"]

# To build the Docker image, run:
# docker build -t streamshot .

# To run the container with the volume mount, use:
# docker run -v ./saved_files:/app/saved_files streamshot