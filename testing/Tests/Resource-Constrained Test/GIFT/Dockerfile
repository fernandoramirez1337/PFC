# Use a lightweight base image
FROM alpine:latest

# Install build tools and necessary libraries
RUN apk add --no-cache build-base python3

# Set working directory
WORKDIR /app

# Copy source code to the container
COPY . .

# Ensure the scripts have executable permissions
RUN chmod +x run_test.sh

# Specify the entry point to run the tests
CMD ["./run_test.sh"]
