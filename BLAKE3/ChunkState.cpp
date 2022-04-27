#include "ChunkState.h"

ChunkState::ChunkState(const unsigned int keyWords[8], unsigned __int64 chunkCounter, unsigned int flags) {
	memcpy(chainingValue, keyWords, sizeof(chainingValue));
	this->chunkCounter = chunkCounter;
	// Utworzenie 64 bajtowego bloku wype³nionego zerami
	memset(block, 0, sizeof(block));
	// Inicjalizacja atrybutów chunk'a
	this->blockLen = 0;
	this->blocksCompressed = 0;
	this->flags = flags;
}

unsigned int ChunkState::len() {
	return BLAKE3_BLOCK_LEN * (unsigned int)blocksCompressed +
		(unsigned int)blockLen;
}

inline unsigned int ChunkState::startFlag() {
	if (blocksCompressed == 0) {
		return CHUNK_START;
	}
	else {
		return 0;
	}
}

void ChunkState::update(const void *input, unsigned int inputLen) {
	const unsigned __int8 *input_u8 = (const unsigned __int8 *)input;
	while (inputLen > 0) {
		// Je¿eli blok jest pe³ny skompresuj i wyczyœæ
		// Skoro inputLen > 0 to na wejœciu czekaj¹ dane
		// Czyli nie jest to koniec chunk'a
		if (blockLen == BLAKE3_BLOCK_LEN) {		// TODO Ogarn¹æ ifa
			unsigned int blockWords[16];
			littleEndianBytes2Words(block, BLAKE3_BLOCK_LEN, blockWords);
			unsigned int out16[16];
			compressFunction(chainingValue, blockWords, chunkCounter,
				BLAKE3_BLOCK_LEN, flags | startFlag(), out16);
			memcpy(chainingValue, out16, sizeof(chainingValue));
			blocksCompressed++;
			memset(block, 0, sizeof(block));
			blockLen = 0;
		}
		// Ustalenie ile bajtów wejœcia siê zmieœci w bloku
		unsigned int want = BLAKE3_BLOCK_LEN - (unsigned int)blockLen;
		unsigned int take = want;
		if (inputLen < want) {
			take = inputLen;
		}
		// Wpisanie do bloku ustalonej iloœci bajtów
		memcpy(&this->block[(unsigned int)blockLen], input_u8, take);
		blockLen += (unsigned __int8)take;
		input_u8 += take;
		inputLen -= take;
	}
}

Output ChunkState::getOutput() {
	Output* ret = new Output();
	memcpy(ret->inputChainingValue, chainingValue, sizeof(ret->inputChainingValue));
	littleEndianBytes2Words(block, sizeof(block), ret->blockWords);
	ret->counter = chunkCounter;
	ret->blockLen = (unsigned int)blockLen;
	ret->flags = flags | startFlag() | CHUNK_END;
	return *ret;
}