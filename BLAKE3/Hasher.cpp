#include "Hasher.h"
Hasher::Hasher() {
	initInternal(IV, 0);
}

// Inicjalizuje funkcjê skrótu
inline void Hasher::initInternal(const unsigned int keyWords[8], unsigned int flags) {
	// Utworzenie pierwszego chunk'a
	chunkState = new ChunkState(keyWords, 0, flags);
	memcpy(this->keyWords, keyWords, sizeof(this->keyWords));
	this->cvStackLen = 0;
	this->flags = flags;
}

inline void Hasher::pushStack(const unsigned int cv[8]) {
	memcpy(&this->cvStack[(size_t)this->cvStackLen * 8], cv, 8 * 4);
	this->cvStackLen++;
}

inline const unsigned int* Hasher::popStack() {
	this->cvStackLen--;
	return &this->cvStack[(unsigned int)this->cvStackLen * 8];
}

inline void Hasher::addChunkCV(unsigned int newCv[8], unsigned __int64 totalChunks) {
	while ((totalChunks & 1) == 0) {
		parentCV(this->popStack(), newCv, keyWords, flags, newCv);
		totalChunks >>= 1;
	}
	pushStack(newCv);
}


void Hasher::update(const void* input, unsigned int inputLen) {
	const unsigned __int8 *input_u8 = (const unsigned __int8 *)input;
	// Wczytywanie wejœcia
	while (inputLen > 0) {
		// Je¿eli chunk jest pe³ny 
		if (chunkState->len() == BLAKE3_CHUNK_LEN) {						// TODO Ogarn¹æ ifa
			Output chunkOutput = chunkState->getOutput();
			unsigned int chunkCV[8];
			chunkOutput.chainingValue(chunkCV);
			unsigned __int64 totalChunks = chunkState->chunkCounter + 1;
			addChunkCV(chunkCV, totalChunks);
			chunkState = new ChunkState (keyWords, totalChunks, flags);
		}
		// Wczytanie tyle bajtów wejœcia ile siê zmieœci w chunk'u
		unsigned int want = BLAKE3_CHUNK_LEN - chunkState->len();
		unsigned int take = want;
		if (inputLen < want) {
			take = inputLen;
		}
		chunkState->update(input_u8, take);
		// Przesuniêcie wskaŸnika na dane o iloœæ wczytanych bajtów i zmniejszenie d³ugoœci wejœcia
		input_u8 += take;
		inputLen -= take;
	}
}

void Hasher::finalize(void* out, unsigned int outLen) {
	Output currentOutput = chunkState->getOutput();
	unsigned int parentNodesRemaining = (unsigned int)cvStackLen;
	while (parentNodesRemaining > 0) {
		parentNodesRemaining--;
		unsigned int currentCV[8];
		currentOutput.chainingValue(currentCV);
		currentOutput = parentOutput(&this->cvStack[parentNodesRemaining * 8], currentCV, this->keyWords, this->flags);
	}
	currentOutput.rootBytes(out, outLen);
	inverseWords(out, outLen);
}


