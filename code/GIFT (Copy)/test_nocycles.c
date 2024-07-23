#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <inttypes.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include "GIFT/GIFT.h"

#define DATA_SIZE (1 * 1024 * 1024)  // 2 MB

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
    uint8_t *PlainText = (uint8_t *)malloc(DATA_SIZE);
    uint8_t *CipherText = (uint8_t *)malloc(DATA_SIZE);
    uint8_t MasterKey[16];

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

    memset(MasterKey, 0, sizeof(MasterKey));

    // Hexadecimal representation of the NIST key
    uint8_t NIST_key[] = {
        0x2b, 0x7e, 0x15, 0x16,
        0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88,
        0x09, 0xcf, 0x4f, 0x3c
    };

    // Copy the key values to MasterKey
    memcpy(MasterKey, NIST_key, sizeof(NIST_key));

    // Print key
    PrintData("Key", (uint8_t*)MasterKey, sizeof(MasterKey));

    // Print first 64 bytes of plaintext
    PrintData("Plaintext (first 64 bytes)", (uint8_t*)PlainText, 64);

    // Encryption
    uint64_t start = get_time_us();

    gift128_encrypt_ecb((uint8_t*)CipherText, (const uint8_t*)PlainText, DATA_SIZE, (const uint8_t*)MasterKey);

    uint64_t end = get_time_us();
    uint64_t encryption_time = end - start;

    // Calculate throughput
    double seconds = (double)encryption_time / 1000000.0; // Conversion from microseconds to seconds
    double encryption_throughput = (double)DATA_SIZE / seconds;  // Bytes per secord
    encryption_throughput /= (1024*1024); // MB per secord

    //Calculate efficiency
    double code_size_kB = 4.9 + 7.6;   // Code size in kB
    double throughput_kbps = encryption_throughput * 8192; // Conversion factor: 1 MB = 8192 kb
    double efficiency = throughput_kbps / code_size_kB;

    // Print first 64 bytes of ciphertext
    PrintData("Ciphertext (first 64 bytes)", (uint8_t*)CipherText, 64);

    // Print results
    printf("\n\n\tGIFT-128 only encryption\n\n");
    printf("Exec. Time for 2MB plaintext:\t %" PRIu64 " ms\n", encryption_time);
    printf("Throughput:\t\t\t %.2f MB/s\n", encryption_throughput);
    printf("Throughput:\t\t\t %.2f kb/s\n", throughput_kbps);
    printf("Efficiency:\t\t\t %.2f\n", efficiency);

    free(PlainText);
    free(CipherText);

    return EXIT_SUCCESS;
}
