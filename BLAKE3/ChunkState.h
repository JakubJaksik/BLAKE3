#ifndef BLAKE3_CHUNK_STATE
#define BLAKE3_CHUNK_STATE

#include "Output.h"

// Klasa jednego chunka
class ChunkState
{
public:
	unsigned int chainingValue[8];
	unsigned __int64 chunkCounter;
	unsigned __int8	 block[BLAKE3_BLOCK_LEN];
	unsigned __int8  blockLen;
	unsigned __int8	 blocksCompressed;
	unsigned int flags;

	ChunkState(const unsigned int keyWords[8], unsigned __int64 chunkCounter, unsigned int flags);

	unsigned int len();

	inline unsigned int startFlag();

	void update(const void* input, unsigned int inputLen);
	
	Output getOutput();
};

#endif // !BLAKE3_CHUNK_STATE