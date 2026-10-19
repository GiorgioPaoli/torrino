#pragma once
#include <cstdint>

extern uint64_t pieceKeys[12][64];
extern uint64_t sideKey;
extern uint64_t castlingKeys[16];
extern uint64_t enPassantKeys[8];

void initZobrist();