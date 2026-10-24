#include "tt.h"

TTentry TT[TT_SIZE];

void clearTT(){
    for (int i = 0; i < TT_SIZE; i++){
        TT[i].key = 0;
    }
}              

bool probeTT(uint64_t key, TTentry &out){
    int index = key & (TT_SIZE - 1);
    if (TT[index].key == key) {out = TT[index]; return true;}
    return false;
}   

void storeTT(uint64_t key, Move move, int score, int depth, TTFlags flag){
    TT[key & (TT_SIZE - 1)] = {key, move, score, depth, flag};
}

// mate scores count plies from the root; in the table they are stored as distance from the node,
// so an entry gives the right mate distance at whatever ply the position is found again
int scoreToTT(int score, int ply){
    if (score < -99000) return score - ply;   // mated
    if (score >  99000) return score + ply;   // mating
    return score;                             // normal score: unchanged
}

int scoreFromTT(int score, int ply){
    if (score < -99000) return score + ply;
    if (score >  99000) return score - ply;
    return score;
}