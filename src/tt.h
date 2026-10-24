#pragma once
#include "move.h"
#include <cstdint>

enum TTFlags {TT_EXACT, TT_LOWER, TT_UPPER};

constexpr int TT_SIZE = 1 << 20;  // 1.048.576 entries 

struct TTentry {
    uint64_t key;
    Move move;
    int score;
    int depth;
    TTFlags flag;
};

extern TTentry TT[];

void clearTT();                 // clears the table
bool probeTT(uint64_t key, TTentry &out);       
void storeTT(uint64_t key, Move move, int score, int depth, TTFlags flag);
int scoreToTT(int score, int ply);      // root-based score -> score relative to this node (for storing)
int scoreFromTT(int score, int ply);    // stored score -> root-based score at this ply (after reading)

