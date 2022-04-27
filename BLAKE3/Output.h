#ifndef BLAKE3_OUTPUT
#define BLAKE3_OUTPUT

#include "Functions.cpp"

class Output
{
public:
	unsigned int inputChainingValue[8];
	unsigned int blockWords[16];
	unsigned __int64 counter;
	unsigned int blockLen;
	unsigned int flags;

    void chainingValue(unsigned int out[8]);

	void rootBytes(void* out, unsigned int outLen);

};

#endif // !BLAKE3_OUTPUT