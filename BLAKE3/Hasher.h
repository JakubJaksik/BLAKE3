#ifndef BLAKE3_HASHER
#define BLAKE3_HASHER

#include "ParentFunc.cpp"

class Hasher
{
	ChunkState* chunkState;
	unsigned int keyWords[8];
	unsigned int cvStack[8 * 54];
	unsigned __int8  cvStackLen;
	unsigned int flags;

public:
	Hasher();

	inline void initInternal(const unsigned int keyWords[8], unsigned int flags);

	inline void pushStack(const unsigned int cv[8]);

	inline const unsigned int* popStack();

	inline void addChunkCV(unsigned int newCv[8], unsigned __int64 totalChunks);

	void update(const void* input, unsigned int inputLen);

	void finalize(void* out, unsigned int outLen);
};

#endif // !BLAKE3_HASHER