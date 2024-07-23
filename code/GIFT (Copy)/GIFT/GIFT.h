#ifndef GIFT_H_
#define GIFT_H_

#define REARRANGE_RKEY_0(x) ({			\
	SWAPMOVE(x, x, 0x00550055, 9);		\
	SWAPMOVE(x, x, 0x000f000f, 12);		\
	SWAPMOVE(x, x, 0x00003333, 18);		\
	SWAPMOVE(x, x, 0x000000ff, 24);		\
})

#define REARRANGE_RKEY_1(x) ({			\
	SWAPMOVE(x, x, 0x11111111, 3);		\
	SWAPMOVE(x, x, 0x03030303, 6);		\
	SWAPMOVE(x, x, 0x000f000f, 12);		\
	SWAPMOVE(x, x, 0x000000ff, 24);		\
})

#define REARRANGE_RKEY_2(x) ({			\
	SWAPMOVE(x, x, 0x0000aaaa, 15);		\
	SWAPMOVE(x, x, 0x00003333, 18);		\
	SWAPMOVE(x, x, 0x0000f0f0, 12);		\
	SWAPMOVE(x, x, 0x000000ff, 24);		\
})

#define REARRANGE_RKEY_3(x) ({			\
	SWAPMOVE(x, x, 0x0a0a0a0a, 3);		\
	SWAPMOVE(x, x, 0x00cc00cc, 6);		\
	SWAPMOVE(x, x, 0x0000f0f0, 12);		\
	SWAPMOVE(x, x, 0x000000ff, 24);		\
})

#define KEY_UPDATE(x)											\
	(((x) >> 12) & 0x0000000f)	| (((x) & 0x00000fff) << 4) | 	\
	(((x) >> 2) & 0x3fff0000)	| (((x) & 0x00030000) << 14)

#define KEY_TRIPLE_UPDATE_0(x)									\
	(ROR((x) & 0x33333333, 24) 	| ROR((x) & 0xcccccccc, 16))

#define KEY_DOUBLE_UPDATE_1(x)									\
	((((x) >> 4) & 0x0f000f00)	| (((x) & 0x0f000f00) << 4) | 	\
	(((x) >> 6) & 0x00030003)	| (((x) & 0x003f003f) << 2))

#define KEY_TRIPLE_UPDATE_1(x)									\
	((((x) >> 6) & 0x03000300)	| (((x) & 0x3f003f00) << 2) | 	\
	(((x) >> 5) & 0x00070007)	| (((x) & 0x001f001f) << 3))

#define KEY_DOUBLE_UPDATE_2(x)									\
	(ROR((x) & 0xaaaaaaaa, 24)	| ROR((x) & 0x55555555, 16))

#define KEY_TRIPLE_UPDATE_2(x)									\
	(ROR((x) & 0x55555555, 24)	| ROR((x) & 0xaaaaaaaa, 20))

#define KEY_DOUBLE_UPDATE_3(x)									\
	((((x) >> 2) & 0x03030303)	| (((x) & 0x03030303) << 2) | 	\
	(((x) >> 1) & 0x70707070)	| (((x) & 0x10101010) << 3))

#define KEY_TRIPLE_UPDATE_3(x)									\
	((((x) >> 18) & 0x00003030)	| (((x) & 0x01010101) << 3) | 	\
	(((x) >> 14) & 0x0000c0c0)	| (((x) & 0x0000e0e0) << 15)|	\
	(((x) >> 1) & 0x07070707)	| (((x) & 0x00001010) << 19))

#define KEY_DOUBLE_UPDATE_4(x)									\
	((((x) >> 4)  & 0x0fff0000)	| (((x) & 0x000f0000) << 12) | 	\
	(((x) >> 8)  & 0x000000ff)	| (((x) & 0x000000ff) << 8))

#define KEY_TRIPLE_UPDATE_4(x)									\
	((((x) >> 6)  & 0x03ff0000)	| (((x) & 0x003f0000) << 10) |	\
	(((x) >> 4)  & 0x00000fff)	| (((x) & 0x0000000f) << 12))

#define ROR(x,y)											\
	(((x) >> (y)) | ((x) << (32 - (y))))
#define BYTE_ROR_2(x)										\
	((((x) >> 2) & 0x3f3f3f3f)	| (((x) & 0x03030303) << 6))
#define BYTE_ROR_4(x) 										\
	((((x) >> 4) & 0x0f0f0f0f)	| (((x) & 0x0f0f0f0f) << 4))
#define BYTE_ROR_6(x) 										\
	((((x) >> 6) & 0x03030303)	| (((x) & 0x3f3f3f3f) << 2))
#define HALF_ROR_4(x) 										\
	((((x) >> 4) & 0x0fff0fff)	| (((x) & 0x000f000f) << 12))
#define HALF_ROR_8(x) 										\
	((((x) >> 8) & 0x00ff00ff)	| (((x) & 0x00ff00ff) << 8))
#define HALF_ROR_12(x) 										\
	((((x) >> 12)& 0x000f000f)	| (((x) & 0x0fff0fff) << 4))
#define NIBBLE_ROR_1(x)										\
	((((x) >> 1) & 0x77777777) 	| (((x) & 0x11111111) << 3))
#define NIBBLE_ROR_2(x)										\
	((((x) >> 2) & 0x33333333) 	| (((x) & 0x33333333) << 2))
#define NIBBLE_ROR_3(x)										\
	((((x) >> 3) & 0x11111111) 	| (((x) & 0x77777777) << 1))

#define SWAPMOVE(a, b, mask, n)								\
	tmp = (b ^ (a >> n)) & mask;							\
	b ^= tmp;												\
	a ^= (tmp << n);

#define SBOX(s0, s1, s2, s3)								\
	s1 ^= s0 & s2;											\
	s0 ^= s1 & s3;											\
	s2 ^= s0 | s1;											\
	s3 ^= s2;												\
	s1 ^= s3;												\
	s3 ^= 0xffffffff;										\
	s2 ^= s0 & s1;

#define INV_SBOX(s0, s1, s2, s3)							\
	s2 ^= s3 & s1;											\
	s0 ^= 0xffffffff;										\
	s1 ^= s0;												\
	s0 ^= s2;												\
	s2 ^= s3 | s1;											\
	s3 ^= s1 & s0;											\
	s1 ^= s3 & s2;

#define QUINTUPLE_ROUND(state, rkey, rconst) ({				\
	SBOX(state[0], state[1], state[2], state[3]);			\
	state[3] = NIBBLE_ROR_1(state[3]);						\
	state[1] = NIBBLE_ROR_2(state[1]);						\
	state[2] = NIBBLE_ROR_3(state[2]);						\
	state[1] ^= (rkey)[0];									\
	state[2] ^= (rkey)[1];									\
	state[0] ^= (rconst)[0];								\
	SBOX(state[3], state[1], state[2], state[0]);			\
	state[0] = HALF_ROR_4(state[0]);						\
	state[1] = HALF_ROR_8(state[1]);						\
	state[2] = HALF_ROR_12(state[2]);						\
	state[1] ^= (rkey)[2];									\
	state[2] ^= (rkey)[3];									\
	state[3] ^= (rconst)[1];								\
	SBOX(state[0], state[1], state[2], state[3]);			\
	state[3] = ROR(state[3], 16);							\
	state[2] = ROR(state[2], 16);							\
	SWAPMOVE(state[1], state[1], 0x55555555, 1);			\
	SWAPMOVE(state[2], state[2], 0x00005555, 1);			\
	SWAPMOVE(state[3], state[3], 0x55550000, 1);			\
	state[1] ^= (rkey)[4];									\
	state[2] ^= (rkey)[5];									\
	state[0] ^= (rconst)[2];								\
	SBOX(state[3], state[1], state[2], state[0]);			\
	state[0] = BYTE_ROR_6(state[0]);						\
	state[1] = BYTE_ROR_4(state[1]);						\
	state[2] = BYTE_ROR_2(state[2]);						\
	state[1] ^= (rkey)[6];									\
	state[2] ^= (rkey)[7];									\
	state[3] ^= (rconst)[3];								\
	SBOX(state[0], state[1], state[2], state[3]);			\
	state[3] = ROR(state[3], 24);							\
	state[1] = ROR(state[1], 16);							\
	state[2] = ROR(state[2], 8);							\
	state[1] ^= (rkey)[8];									\
	state[2] ^= (rkey)[9];									\
	state[0] ^= (rconst)[4];								\
	state[0] ^= state[3];									\
	state[3] ^= state[0];									\
	state[0] ^= state[3];									\
})

#define INV_QUINTUPLE_ROUND(state, rkey, rconst) ({			\
	state[0] ^= state[3];									\
	state[3] ^= state[0];									\
	state[0] ^= state[3];									\
	state[1] ^= (rkey)[8];									\
	state[2] ^= (rkey)[9];									\
	state[0] ^= (rconst)[4];								\
	state[3] = ROR(state[3], 8);							\
	state[1] = ROR(state[1], 16);							\
	state[2] = ROR(state[2], 24);							\
	INV_SBOX(state[3], state[1], state[2], state[0]);		\
	state[1] ^= (rkey)[6];									\
	state[2] ^= (rkey)[7];									\
	state[3] ^= (rconst)[3];								\
	state[0] = BYTE_ROR_2(state[0]);						\
	state[1] = BYTE_ROR_4(state[1]);						\
	state[2] = BYTE_ROR_6(state[2]);						\
	INV_SBOX(state[0], state[1], state[2], state[3]);		\
	state[1] ^= (rkey)[4];									\
	state[2] ^= (rkey)[5];									\
	state[0] ^= (rconst)[2];								\
	SWAPMOVE(state[3], state[3], 0x55550000, 1);			\
	SWAPMOVE(state[1], state[1], 0x55555555, 1);			\
	SWAPMOVE(state[2], state[2], 0x00005555, 1);			\
	state[3] = ROR(state[3], 16);							\
	state[2] = ROR(state[2], 16);							\
	INV_SBOX(state[3], state[1], state[2], state[0]);		\
	state[1] ^= (rkey)[2];									\
	state[2] ^= (rkey)[3];									\
	state[3] ^= (rconst)[1];								\
	state[0] = HALF_ROR_12(state[0]);						\
	state[1] = HALF_ROR_8(state[1]);						\
	state[2] = HALF_ROR_4(state[2]);						\
	INV_SBOX(state[0], state[1], state[2], state[3]);		\
	state[1] ^= (rkey)[0];									\
	state[2] ^= (rkey)[1];									\
	state[0] ^= (rconst)[0];								\
	state[3] = NIBBLE_ROR_3(state[3]);						\
	state[1] = NIBBLE_ROR_2(state[1]);						\
	state[2] = NIBBLE_ROR_1(state[2]);						\
	INV_SBOX(state[3], state[1], state[2], state[0]);		\
})

#define U32BIG(x)											\
  ((((x) & 0x000000FF) << 24) | (((x) & 0x0000FF00) << 8) | \
   (((x) & 0x00FF0000) >> 8) | (((x) & 0xFF000000) >> 24))

#define U8BIG(x, y)											\
	(x)[0] = (y) >> 24; 									\
	(x)[1] = ((y) >> 16) & 0xff; 							\
	(x)[2] = ((y) >> 8) & 0xff; 							\
	(x)[3] = (y) & 0xff;

#define BLOCK_SIZE	16
#define KEY_SIZE	16

typedef unsigned char u8;
typedef unsigned int u32;

int gift128_encrypt_ecb(u8* ctext, const u8* ptext, u32 ptext_len, const u8* key);
int gift128_decrypt_ecb(u8* ptext, const u8* ctext, u32 ctext_len, const u8* key);

#endif