#include "eval.h"
#include "bitboard.h"


int evaluate(Board &board) {
    int score = 0;
    for (int pt = 0; pt < PIECE_TYPE_NB; pt++) {
        uint64_t wbb = board.pieceBB[WHITE][pt];
        uint64_t bbb = board.pieceBB[BLACK][pt];
        while (wbb) {
            int sq = popLsb(wbb);
            score += PIECE_WEIGHTS[pt] + PST[pt][sq ^ 56];  // importante, questo inverte la tabella
        } 
        while (bbb) {
            int sq = popLsb(bbb);
            score -= PIECE_WEIGHTS[pt] + PST[pt][sq];
        }
    }
    
    return (board.sideToMove == WHITE) ? score : -score;
}   