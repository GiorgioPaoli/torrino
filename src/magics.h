#pragma once
#include "attacks.h"
#include "magic_numbers.h"

/*
    Magic bitboards: sliding-piece attacks with one table lookup instead of walking the rays.
    The tables are filled once by initMagics(); the *Slow functions in attacks.cpp are only used for that.
*/

extern uint64_t rookMask[64];
extern uint64_t bishopMask[64];
extern int rookShift[64];
extern int bishopShift[64];
extern uint64_t rookTable[64][4096];
extern uint64_t bishopTable[64][512];

void initMagics();

inline uint64_t rookAttacks(int sq, uint64_t occupancy) {
    occupancy &= rookMask[sq];
    occupancy *= ROOK_MAGICS[sq];
    return rookTable[sq][occupancy >> rookShift[sq]];
}

inline uint64_t bishopAttacks(int sq, uint64_t occupancy) {
    occupancy &= bishopMask[sq];
    occupancy *= BISHOP_MAGICS[sq];
    return bishopTable[sq][occupancy >> bishopShift[sq]];
}

inline uint64_t queenAttacks(int sq, uint64_t occupancy) {
    return rookAttacks(sq, occupancy) | bishopAttacks(sq, occupancy);
}