#!/bin/sh

# Function to clean up environment
cleanup() {
    echo "Cleaning up..."

    # Kill any remaining processes (adjust as needed)
    pkill -f test

    # Remove temporary files
    rm -f data.bin
    rm -f /tmp/test*

    # Unset environment variables (example)
    unset VAR_NAME

    # Remove shared memory segments, message queues, and semaphores
    for id in $(ipcs -m | awk '/0x/ {print $2}'); do ipcrm -m $id; done
    for id in $(ipcs -q | awk '/0x/ {print $2}'); do ipcrm -q $id; done
    for id in $(ipcs -s | awk '/0x/ {print $2}'); do ipcrm -s $id; done

    echo "Cleanup complete."
}

# Function to handle errors
handle_error() {
    echo "An error occurred. Cleaning up before exiting."
    cleanup
    exit 1
}

# Trap any script errors and call the handle_error function
trap 'handle_error' ERR

# Clean up before running tests
cleanup

# Run the Python script
echo "Generating 2MB data in data.bin..."
python3 util.py

# Compile and build the GIFT128 test using c99 standard
echo "Compiling test.c"
gcc -o test test.c GIFT/GIFT.c

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation of test.c failed."
    handle_error
fi

echo "Compilation successful."

# Run the test with resource profiling
echo "Running test..."
/usr/bin/time -v ./test && size test

# Clean up after running tests
cleanup

echo "All tasks completed."
