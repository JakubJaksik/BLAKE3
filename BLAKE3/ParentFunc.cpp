#include "ChunkState.h"

inline Output parentOutput(const unsigned int leftChildCV[8], const unsigned int rightChildCV[8],
						   const unsigned int keyWords[8],			unsigned int flags) {
	Output* ret = new Output();
	memcpy(ret->inputChainingValue, keyWords, sizeof(ret->inputChainingValue));
	memcpy(&ret->blockWords[0], leftChildCV, 8 * 4);
	memcpy(&ret->blockWords[8], rightChildCV, 8 * 4);
	ret->counter = 0;
	ret->blockLen = BLAKE3_BLOCK_LEN;
	ret->flags = PARENT | flags;
	return *ret;
}

inline void parentCV(const unsigned int leftChildCv[8], const unsigned int rightChildCv[8],
					 const unsigned int keyWords[8], unsigned int flags, unsigned int out[8]) {
	Output o = parentOutput(leftChildCv, rightChildCv, keyWords, flags);
	o.chainingValue(out);
}