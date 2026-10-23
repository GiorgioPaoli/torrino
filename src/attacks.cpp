#include "attacks.h"
#include <cstdlib>
#include <iostream>

uint64_t pawnAttacks[COLOR_NB][64];
uint64_t knightAttacks[64];
uint64_t kingAttacks[64];
uint64_t bishopEmptyAttacks[64];
uint64_t rookEmptyAttacks[64];
uint64_t queenEmptyAttacks[64];

void initPawnAttacks(){
    for (int sq = 0; sq < 64; sq++){
        uint64_t bb = SQ_BB(sq);
        pawnAttacks[WHITE][sq] = shiftNorthWest(bb) | shiftNorthEast(bb); 
        pawnAttacks[BLACK][sq] = shiftSouthWest(bb) | shiftSouthEast(bb);
    }
}

void initKnightAttacks(){
    for (int sq = 0; sq < 64; sq++){
        knightAttacks[sq] = knightAttacksFromSquare(sq);
    }
}

// +17, +15, +10, +6, -6, -10, -15, -17
uint64_t knightAttacksFromSquare(uint64_t sq){
    uint64_t attacks = 0ULL;
    int startFile = fileOf(sq);

    int offsets[8] = {17, 15, 10, 6, -6, -10, -15, -17};

    for (int off : offsets){
        int target = sq + off;
        if (target < 0 || target > 63) continue;

        int targetFile = fileOf(target);
        int fileDiff = abs(targetFile - startFile);
        
        if (fileDiff != 1 && fileDiff != 2) continue;

        attacks |= SQ_BB(target);
    }

    return attacks;
}

void initKingAttacks(){
    for (int sq = 0; sq < 64; sq++){
        kingAttacks[sq] = kingAttacksFromSquare(sq);
    }
}

// attacks of sliding pieces on an EMPTY board, used as a fast pre-filter:
// if a piece doesn't reach a square even on an empty board, it can't reach it with pieces in between
void initSliderMasks(){
    for (int sq = 0; sq < 64; sq++){
        bishopEmptyAttacks[sq] = bishopAttacksSlow(sq, 0ULL);
        rookEmptyAttacks[sq]   = rookAttacksSlow(sq, 0ULL);
        queenEmptyAttacks[sq]  = bishopEmptyAttacks[sq] | rookEmptyAttacks[sq];
    }
}

// +9, +8, +7, +1, -1, -7, -8, -9
uint64_t kingAttacksFromSquare(uint64_t sq){
    uint64_t attacks = 0ULL;
    int startFile = fileOf(sq);

    int offsets[8] = {9, 8, 7, 1, -1, -7, -8, -9};

    for (int off : offsets){
        int target = sq + off;
        if (target < 0 || target > 63) continue;

        int targetFile = fileOf(target);
        int fileDiff = abs(targetFile - startFile);
        
        if (fileDiff > 1) continue;

        attacks |= SQ_BB(target);
    }

    return attacks;
}

// 4 direzioni: +9 +7, -7, -9
uint64_t bishopAttacksSlow(int sq, uint64_t occupancy){
    uint64_t attacks = 0ULL;
    int startFile = fileOf(sq);

    int offsets[4] = {9, 7, -7, -9};

    for (int off : offsets){
        int currentSq = sq;
        int currentFile = startFile;
        for (int i = 0; i < 7; i++){
            int target = currentSq + off;
            currentSq += off;
            if (target < 0 || target > 63) break;
            
            int targetFile = fileOf(target);
            int fileDiff = abs(targetFile - currentFile);

            if (fileDiff != 1) break; 
            
            attacks |= SQ_BB(target);
            
            if (getBit(occupancy, target)) break;

            currentSq = target;
            currentFile = fileOf(target);
        }
    }

    return attacks;
}

// +8 -8 +1 -1
uint64_t rookAttacksSlow(int sq, uint64_t occupancy){
    uint64_t attacks = 0ULL;
    int startFile = fileOf(sq);

    int offsets[4] = {8, 1, -1, -8};

    for (int off : offsets){
        int currentSq = sq;
        int currentFile = startFile;
        for (int i = 0; i < 7; i++){
            int target = currentSq + off;
            currentSq += off;
            if (target < 0 || target > 63) break;
            
            // se si sposta di file
            if (abs(off) == 1){
                int targetFile = fileOf(target);
                int fileDiff = abs(targetFile - currentFile);
                if (fileDiff != 1) break;
                currentFile = targetFile;
            }

            attacks |= SQ_BB(target);

            if (getBit(occupancy, target)) break;

            currentSq = target;
        }
    }

    return attacks;
}

// OR con bishopAttacksSlow e rookAttacksSlow
uint64_t queenAttacksSlow(int sq, uint64_t occupancy){
    return bishopAttacksSlow(sq, occupancy) | rookAttacksSlow(sq, occupancy);
}

// rook: the rank and the file of sq, without sq and without the far ends of the lines
uint64_t rookRelevantMask(int sq){
    uint64_t rankPart = (0xFFULL << (8 * rankOf(sq))) & ~(FILE_A | FILE_H);
    uint64_t filePart = FILE_MASKS[fileOf(sq)] & ~(RANK_1 | RANK_8);
    return (rankPart | filePart) & ~SQ_BB(sq);
}

// bishop: its empty-board attacks without the four edges (a bishop ray always ends on an edge)
uint64_t bishopRelevantMask(int sq){
    return bishopAttacksSlow(sq, 0ULL) & ~(RANK_1 | RANK_8 | FILE_A | FILE_H);
}

