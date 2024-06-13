#!/bin/sh

# Resource limits
MEMORY_LIMIT="128m"   # Limit memory to 128MB
CPU_LIMIT="0.5"       # Limit CPU to half of a core
CPUSET="0"            # Use only the first CPU
PIDS_LIMIT="50"       # Limit to 50 processes
BLKIO_WEIGHT="500"    # Set I/O weight (default 500, range 10-1000)

# Build Docker image (assuming Dockerfile is in the current directory)
docker build -t test_rc_gift .

# Run GIFT128 test in a resource-constrained container
docker run --rm \
    --memory $MEMORY_LIMIT \
    --cpus $CPU_LIMIT \
    --cpuset-cpus $CPUSET \
    --pids-limit $PIDS_LIMIT \
    --blkio-weight $BLKIO_WEIGHT \
    test_rc_gift
