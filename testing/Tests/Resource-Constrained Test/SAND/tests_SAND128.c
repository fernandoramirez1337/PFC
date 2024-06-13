#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <inttypes.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
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
    uint64_t *NewPlainText = (uint64_t *)malloc(DATA_SIZE);
    uint64_t MasterKey[2];
    uint64_t RoundKey[SAND128_ROUNDS];

    if (!PlainText || !CipherText || !NewPlainText) {
        fprintf(stderr, "Memory allocation failed\n");
        free(PlainText);
        free(CipherText);
        free(NewPlainText);
        return EXIT_FAILURE;
    }

    int file = open("data.bin", O_RDONLY);
    if (file < 0) {
        fprintf(stderr, "Failed to open file\n");
        free(PlainText);
        free(CipherText);
        free(NewPlainText);
        return EXIT_FAILURE;
    }

    ssize_t bytesRead = read(file, PlainText, DATA_SIZE);
    if (bytesRead != DATA_SIZE) {
        fprintf(stderr, "Failed to read file\n");
        close(file);
        free(PlainText);
        free(CipherText);
        free(NewPlainText);
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

    struct rusage usage_before, usage_after;

    // Encryption
    getrusage(RUSAGE_SELF, &usage_before);
    uint64_t start = get_time_us();
    ECB_encrypt_Bitslice(PlainText, CipherText, RoundKey, SAND128_ROUNDS, DATA_SIZE);
    uint64_t end = get_time_us();
    uint64_t encryption_time = end - start;
    getrusage(RUSAGE_SELF, &usage_after);

    long encryption_memory = usage_after.ru_maxrss - usage_before.ru_maxrss;

    // Calculate throughput
    double encryption_throughput = (double)DATA_SIZE / encryption_time;  // MB/s

    // Print first 64 bytes of ciphertext
    PrintData("Ciphertext (first 64 bytes)", (uint8_t*)CipherText, 64);

    // Decryption
    KeySchedule(MasterKey, RoundKey, SAND128_ROUNDS, 1);

    getrusage(RUSAGE_SELF, &usage_before);
    start = get_time_us();
    ECB_decrypt_Bitslice(CipherText, NewPlainText, RoundKey, SAND128_ROUNDS, DATA_SIZE);
    end = get_time_us();
    uint64_t decryption_time = end - start;
    getrusage(RUSAGE_SELF, &usage_after);

    long decryption_memory = usage_after.ru_maxrss - usage_before.ru_maxrss;

    // Calculate throughput
    double decryption_throughput = (double)DATA_SIZE / decryption_time;  // MB/s

    // Print first 64 bytes of decrypted plaintext
    PrintData("Decrypted Plaintext (first 64 bytes)", (uint8_t*)NewPlainText, 64);

    // Check if the original plaintext matches the decrypted plaintext
    if (memcmp(PlainText, NewPlainText, DATA_SIZE)) {
        printf("Decryption failed: Plaintext does not match decrypted plaintext.\n");
        free(PlainText);
        free(CipherText);
        free(NewPlainText);
        return EXIT_FAILURE;
    }

    // Print results
    printf("Encryption Time for 2MB data: %" PRIu64 " microseconds\n", encryption_time);
    printf("Encryption Throughput: %.2f MB/s\n", encryption_throughput);
    printf("Encryption Memory Usage: %ld KB\n", encryption_memory);
    printf("Decryption Time for 2MB data: %" PRIu64 " microseconds\n", decryption_time);
    printf("Decryption Throughput: %.2f MB/s\n", decryption_throughput);
    printf("Decryption Memory Usage: %ld KB\n", decryption_memory);

    free(PlainText);
    free(CipherText);
    free(NewPlainText);

    return EXIT_SUCCESS;
}
