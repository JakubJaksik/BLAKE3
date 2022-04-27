#include "output.h"

void  Output::chainingValue(unsigned int out[8]) {
    unsigned int out16[16];
    compressFunction(inputChainingValue, blockWords, counter,
        blockLen, flags, out16);
    memcpy(out, out16, 8 * 4);
}

void Output::rootBytes(void* out, unsigned int outLen) {
    unsigned __int8 *out_u8 = (unsigned __int8*)out;
    unsigned __int64 outputBlockCounter = 0;
    while (outLen > 0) {
        unsigned int words[16];
        compressFunction(inputChainingValue, blockWords, outputBlockCounter, blockLen, flags | ROOT, words);
        for (unsigned int word = 0; word < 16; word++) {
            for (int byte = 0; byte < 4; byte++) {
                if (outLen == 0) {
                    return;
                }
                *out_u8 = (unsigned __int8)(words[word] >> (8 * byte));
                out_u8++;
                outLen--;
            }
        }
        outputBlockCounter++;
    }
}