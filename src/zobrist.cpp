#include <random>
#include "zobrist.h"

uint64_t pieceKeys[12][64];
uint64_t sideKey;
uint64_t castlingKeys[16];
uint64_t enPassantKeys[8];

void initZobrist() {
    std::mt19937_64 rng(123456789ULL);

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            pieceKeys[i][j] = rng();
        }
    }

    sideKey = rng();

    for (int i = 0; i < 16; i++){
        castlingKeys[i] = rng();
    }

    for (int i = 0; i < 8; i++){
        enPassantKeys[i] = rng();
    }

}

