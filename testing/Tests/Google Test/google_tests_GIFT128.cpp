#include <gtest/gtest.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <inttypes.h>
#include <sys/resource.h>  // For memory usage
#include "GIFT/GIFT.h"

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

class GiftTest : public ::testing::Test {
protected:
    uint8_t *PlainText;
    uint8_t *CipherText;
    uint8_t *NewPlainText;
    uint8_t MasterKey[16];

    virtual void SetUp() {
        PlainText = (uint8_t *)malloc(DATA_SIZE);
        CipherText = (uint8_t *)malloc(DATA_SIZE);
        NewPlainText = (uint8_t *)malloc(DATA_SIZE);
        memset(MasterKey, 0, sizeof(MasterKey));
    }

    virtual void TearDown() {
        free(PlainText);
        free(CipherText);
        free(NewPlainText);
    }

    void LoadData(const char* filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file");
        }
        file.read((char*)PlainText, DATA_SIZE);
        file.close();
    }

    void PrintData(const char* title, uint8_t* data, size_t length) {
        printf("%s:\n", title);
        for (size_t i = 0; i < length; i += 16) {
            phex(data + i);
        }
        printf("\n");
    }
};


TEST_F(GiftTest, TestLargeDataEncryptionAndDecryption) {
    LoadData("data.bin");

    // Print key
    PrintData("Key", (uint8_t*)MasterKey, sizeof(MasterKey));

    // Print first 64 bytes of plaintext
    PrintData("Plaintext (first 64 bytes)", (uint8_t*)PlainText, 64);

    struct rusage usage;

    // Encryption
    uint64_t start = get_time_us();
    gift128_encrypt_ecb((uint8_t*)CipherText, (const uint8_t*)PlainText, DATA_SIZE, (const uint8_t*)MasterKey);
    uint64_t end = get_time_us();
    uint64_t encryption_time = end - start;
    getrusage(RUSAGE_SELF, &usage);
    long encryption_memory = usage.ru_maxrss;  // Max resident set size

    // Calculate throughput
    double encryption_throughput = (double)DATA_SIZE / encryption_time;  // MB/s

    // Print first 64 bytes of ciphertext
    PrintData("Ciphertext (first 64 bytes)", (uint8_t*)CipherText, 64);

    // Decryption
    start = get_time_us();
    gift128_decrypt_ecb((uint8_t*)NewPlainText, (const uint8_t*)CipherText, DATA_SIZE, (const uint8_t*)MasterKey);
    end = get_time_us();
    uint64_t decryption_time = end - start;
    getrusage(RUSAGE_SELF, &usage);
    long decryption_memory = usage.ru_maxrss;  // Max resident set size

    // Calculate throughput
    double decryption_throughput = (double)DATA_SIZE / decryption_time;  // MB/s

    // Print first 64 bytes of decrypted plaintext
    PrintData("Decrypted Plaintext (first 64 bytes)", (uint8_t*)NewPlainText, 64);

    // Check if the original plaintext matches the decrypted plaintext
    EXPECT_EQ(0, memcmp(PlainText, NewPlainText, DATA_SIZE)) << "Decryption failed: Plaintext does not match decrypted plaintext.";

    // Print results
    printf("Encryption Time for 2MB data: %" PRIu64 " microseconds\n", encryption_time);
    printf("Encryption Throughput: %.2f MB/s\n", encryption_throughput);
    printf("Encryption Memory Usage: %ld KB\n", encryption_memory);
    printf("Decryption Time for 2MB data: %" PRIu64 " microseconds\n", decryption_time);
    printf("Decryption Throughput: %.2f MB/s\n", decryption_throughput);
    printf("Decryption Memory Usage: %ld KB\n", decryption_memory);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
