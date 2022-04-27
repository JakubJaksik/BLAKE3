#pragma once
#include <cassert>
#include <corecrt_memory.h>
#include <iostream>

const static unsigned int BLAKE3_OUT_LEN = 32;
const static unsigned int BLAKE3_KEY_LEN = 32;
const static unsigned int BLAKE3_BLOCK_LEN = 64;
const static unsigned int BLAKE3_CHUNK_LEN = 1024;

const static unsigned int CHUNK_START = 1 << 0;
const static unsigned int CHUNK_END = 1 << 1;
const static unsigned int PARENT = 1 << 2;
const static unsigned int ROOT = 1 << 3;

// Wektor inicjalizuj¹cy
static unsigned int IV[8] = {
	0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
	0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19,
};

static unsigned int PERMUTATION[16] = {
	2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8
};

inline unsigned int rotateRight(unsigned int x, int n) {
	return (x >> n) | (x << (32 - n));
}

// Funkcja mieszaj¹ca G. Miesza odpowiednie kolumny i przek¹tne
inline void G(unsigned int state[16], unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int mx, unsigned int my) {
	state[a] = state[a] + state[b] + mx;
	state[d] = rotateRight(state[d] ^ state[a], 16);
	state[c] = state[c] + state[d];
	state[b] = rotateRight(state[b] ^ state[c], 12);
	state[a] = state[a] + state[b] + my;
	state[d] = rotateRight(state[d] ^ state[a], 8);
	state[c] = state[c] + state[d];
	state[b] = rotateRight(state[b] ^ state[c], 7);
}

// Funkcja rundy. Wykonuje jedn¹ pe³n¹ rundê
inline void roundFunction(unsigned int state[16], unsigned int msg[16]) {
	// Mieszanie kolumn
	G(state, 0, 4, 8, 12, msg[0], msg[1]);
	G(state, 1, 5, 9, 13, msg[2], msg[3]);
	G(state, 2, 6, 10, 14, msg[4], msg[5]);
	G(state, 3, 7, 11, 15, msg[6], msg[7]);
	//
	  // Mieszanie przek¹tnych
	G(state, 0, 5, 10, 15, msg[8], msg[9]);
	G(state, 1, 6, 11, 12, msg[10], msg[11]);
	G(state, 2, 7, 8,  13, msg[12], msg[13]);
	G(state, 3, 4, 9,  14, msg[14], msg[15]);
}

// Funkcja permutuj¹ca 
inline void permute(unsigned int msg[16]) {
	unsigned int permutedMsg[16];
	for (unsigned int i = 0; i < 16; i++) {
		permutedMsg[i] = msg[PERMUTATION[i]];
	}memcpy(msg, permutedMsg, sizeof(permutedMsg));
}

// Funkcja kompresji
inline void compressFunction(const unsigned int chainingValue[8], const unsigned int blockWords[16],
	unsigned __int64 counter, unsigned int block_len,
	unsigned int flags, unsigned int out[16]) {
	// Tablica stanu wewnêtrznego
	unsigned int state[16] = {
		chainingValue[0], chainingValue[1], chainingValue[2], chainingValue[3],
		chainingValue[4], chainingValue[5], chainingValue[6], chainingValue[7],
		IV[0], IV[1], IV[2], IV[3],
		(unsigned int)counter, (unsigned int)(counter >> 32), block_len, flags
	};

	unsigned int block[16];
	memcpy(block, blockWords, sizeof(block));

	roundFunction(state, block); // runda 1
	permute(block);
	roundFunction(state, block); // runda 2
	permute(block);
	roundFunction(state, block); // runda 3
	permute(block);
	roundFunction(state, block); // runda 4
	permute(block);
	roundFunction(state, block); // runda 5
	permute(block);
	roundFunction(state, block); // runda 6
	permute(block);
	roundFunction(state, block); // runda 7

	for (unsigned int i = 0; i < 8; i++) {
		state[i] ^= state[i + 8];
		state[i + 8] ^= chainingValue[i];
	}

	memcpy(out, state, sizeof(state));
}

inline static void littleEndianBytes2Words(const void* bytes, unsigned int bytesLen, unsigned int* out) {
	assert(bytesLen % 4 == 0);
	const unsigned __int8* u8Ptr = (const unsigned __int8*)bytes;
	for (unsigned int i = 0; i < (bytesLen / 4); i++) {
		out[i]  = ((unsigned int)(*u8Ptr++));
		out[i] += ((unsigned int)(*u8Ptr++)) << 8;
		out[i] += ((unsigned int)(*u8Ptr++)) << 16;
		out[i] += ((unsigned int)(*u8Ptr++)) << 24;
	}
}

inline static void inverseWords(void *words, unsigned int wordLen) {
	unsigned int* wordsInt = (unsigned int*)words;
	for (int i = 0; i < wordLen/4; i++) {
		unsigned __int8* wordInt8 = (unsigned __int8*)(wordsInt + i);
		unsigned __int8 temp[4] = {wordInt8[0], wordInt8[1], wordInt8[2], wordInt8[3]};
		wordInt8[0] = temp[3];
		wordInt8[1] = temp[2];
		wordInt8[2] = temp[1];
		wordInt8[3] = temp[0];
	}
}