#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <inttypes.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <papi.h>
#include "SAND/SAND.h"

#define DATA_SIZE (2 * 1024 * 1024)  // 2 MB

// Function to get current time in microseconds (for higher precision in embedded systems)
uint64_t get_time_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000 + (uint64_t)ts.tv_nsec / 1000;
}

// Prints string as hex
static void phex(uint8_t* str) {
    uint8_t len = 16;
    unsigned char i;
    for (i = 0; i < len; ++i)
        printf("%.2x", str[i]);
    printf("\n");
}

void PrintData(const char* title, uint8_t* data, size_t length) {
    printf("%s:\n", title);
    for (size_t i = 0; i < length; i += 16) {
        phex(data + i);
    }
    printf("\n");
}

int main() {
    uint64_t *PlainText = (uint64_t *)malloc(DATA_SIZE);
    uint64_t *CipherText = (uint64_t *)malloc(DATA_SIZE);
    uint64_t MasterKey[2];
    uint64_t RoundKey[SAND128_ROUNDS];

    if (!PlainText || !CipherText) {
        fprintf(stderr, "Memory allocation failed\n");
        free(PlainText);
        free(CipherText);
        return EXIT_FAILURE;
    }

    int file = open("data.bin", O_RDONLY);
    if (file < 0) {
        fprintf(stderr, "Failed to open file\n");
        free(PlainText);
        free(CipherText);
        return EXIT_FAILURE;
    }

    ssize_t bytesRead = read(file, PlainText, DATA_SIZE);
    if (bytesRead != DATA_SIZE) {
        fprintf(stderr, "Failed to read file\n");
        close(file);
        free(PlainText);
        free(CipherText);
        return EXIT_FAILURE;
    }
    close(file);

    //MasterKey[0] = 0x1716151413121110;
    //MasterKey[1] = 0x1F1E1D1C1B1A1918;

    // Hexadecimal representation of the NIST key
    uint64_t NIST_key[] = {
        0xa6d2ae2816157e2b, 0x3c4fcf098815f7ab
    };

    // Copy the key values to MasterKey
    memcpy(MasterKey, NIST_key, sizeof(NIST_key));

    KeySchedule(MasterKey, RoundKey, SAND128_ROUNDS, 0);

    // Print key
    PrintData("Key", (uint8_t*)MasterKey, sizeof(MasterKey));

    // Print first 64 bytes of plaintext
    PrintData("Plaintext (first 64 bytes)", (uint8_t*)PlainText, 64);

    // PAPI variables
    int retval;
    int EventSet = PAPI_NULL;
    long long values[2];

    // Initialize PAPI library
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        fprintf(stderr, "PAPI library init error!\n");
        exit(1);
    }

    // Create the Event Set
    retval = PAPI_create_eventset(&EventSet);
    if (retval != PAPI_OK) {
        fprintf(stderr, "PAPI create event set error!\n");
        exit(1);
    }

    // Add the Total Cycles Event
    retval = PAPI_add_event(EventSet, PAPI_TOT_CYC);
    if (retval != PAPI_OK) {
        fprintf(stderr, "PAPI add event error!\n");
        exit(1);
    }

    // Add the Instructions Completed Event
    retval = PAPI_add_event(EventSet, PAPI_TOT_INS);
    if (retval != PAPI_OK) {
        fprintf(stderr, "PAPI add event error!\n");
        exit(1);
    }

    // Encryption
    uint64_t start = get_time_us();

    // Start counting events in the Event Set
    retval = PAPI_start(EventSet);
    if (retval != PAPI_OK) {
        fprintf(stderr, "PAPI start error!\n");
        exit(1);
    }

    ECB_encrypt_Bitslice(PlainText, CipherText, RoundKey, SAND128_ROUNDS, DATA_SIZE);

// Stop counting events in the Event Set
    retval = PAPI_stop(EventSet, values);
    if (retval != PAPI_OK) {
        fprintf(stderr, "PAPI stop error!\n");
        exit(1);
    }

    uint64_t end = get_time_us();
    uint64_t encryption_time = end - start;

    // Calculate throughput
    double seconds = (double)encryption_time / 1000000.0; // Conversion from microseconds to seconds
    double encryption_throughput = (double)DATA_SIZE / seconds;  // Bytes per secord
    encryption_throughput /= (1024*1024); // MB per secord

    //Calculate latency
    size_t total_blocks = 131072;
    double total_cycles = values[0];
    double total_instructions = values[1];
    double cycles_per_block = total_cycles / total_blocks;

    //Calculate efficiency
    double code_size_kB = 3.4 + 1.2;   // Code size in kB
    double throughput_kbps = encryption_throughput * 8192; // Conversion factor: 1 MB = 8192 kb
    double efficiency = throughput_kbps / code_size_kB;

    //Calculate Code_Size×Cycle_Count/Block_Size
    double s_metric = total_cycles * code_size_kB / 128;

    // Print first 64 bytes of ciphertext
    PrintData("Ciphertext (first 64 bytes)", (uint8_t*)CipherText, 64);

    // Print results
    printf("\n\n\tSAND-128 only encryption\n\n");
    printf("Exec. Time for 2MB plaintext:\t %" PRIu64 " ms\n", encryption_time);
    printf("Throughput:\t\t\t %.2f MB/s\n", encryption_throughput);
    printf("Throughput:\t\t\t %.2f kb/s\n", throughput_kbps);
    printf("Total Cycles:\t\t\t %.f\n", total_cycles);
    printf("Latency (Cycles per block):\t %.f\n", cycles_per_block);
    printf("Total Instructions:\t\t %.f\n", total_instructions);
    printf("Efficiency:\t\t\t %.2f\n", efficiency);
    printf("Code_Size*Cycle_Count/Block_Size:%.f\n", s_metric);
    free(PlainText);
    free(CipherText);

    return EXIT_SUCCESS;
}
