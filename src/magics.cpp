#include "magics.h"

uint64_t rookMask[64];
uint64_t bishopMask[64];
int rookShift[64];                  // how far to shift right after the multiply so that exactly that many bits are left as the index.
int bishopShift[64];
uint64_t rookTable[64][4096];
uint64_t bishopTable[64][512];

void initMagics() {
    for (int sq = 0; sq < 64; sq++) {
        rookMask[sq] = rookRelevantMask(sq);
        rookShift[sq] = 64 - popcount(rookMask[sq]);

        // every blocker pattern inside the mask goes to its own slot, holding the correct attacks
        uint64_t sub = 0;
        do {
            int idx = (int)((sub * ROOK_MAGICS[sq]) >> rookShift[sq]);
            rookTable[sq][idx] = rookAttacksSlow(sq, sub);
            sub = (sub - rookMask[sq]) & rookMask[sq];
        } while (sub);

        bishopMask[sq] = bishopRelevantMask(sq);
        bishopShift[sq] = 64 - popcount(bishopMask[sq]);

        sub = 0;
        do {
            int idx = (int)((sub * BISHOP_MAGICS[sq]) >> bishopShift[sq]);
            bishopTable[sq][idx] = bishopAttacksSlow(sq, sub);
            sub = (sub - bishopMask[sq]) & bishopMask[sq];
        } while (sub);
    }
}