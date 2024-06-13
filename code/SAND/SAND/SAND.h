#ifndef SAND_H
#define SAND_H

#include <stdio.h>
#include <stdint.h>

typedef uint64_t u64;

#define SAND128_ROUNDS 54
#define SAND128_BLOCK_SIZE 16 // 16 * 8 = 128 bits
#define SAND128_KEY_SIZE 16 // 16 * 8 = 128 bits

#define SWAP(x,y) ((x) ^= (y) , (y) ^= (x) , (x) ^= (y))
// General SAND-128

u64 ROTL(u64 x, int shift);
u64 P(u64 x);
u64 A16x3(u64 x);
void KeySchedule(const u64 MasterKey[], u64 RoundKey[], int Round, int Dec);
// Bitslice Implementation

u64 G0(u64 x);
u64 G1(u64 x);
void Bitslice_Round(const u64 PlainText[], u64 CipherText[], const u64 RoundKey[], int Round);
// Synthetic S-box Implementation

u64 S(u64 x);
void SSB_Round(const u64 PlainText[], u64 CipherText[], const u64 RoundKey[], int Round);

void ECB_encrypt_Bitslice(const u64 PlainText[], u64 CipherText[], const u64 RoundKey[], int Round, size_t data_size);
void ECB_decrypt_Bitslice(const u64 PlainText[], u64 CipherText[], const u64 RoundKey[], int Round, size_t data_size);
void ECB_encrypt_SSB(const u64 PlainText[], u64 CipherText[], const u64 RoundKey[], int Round, size_t data_size);
void ECB_decrypt_SSB(const u64 PlainText[], u64 CipherText[], const u64 RoundKey[], int Round, size_t data_size);

#endif /* SAND_H */
