#include "eval.h"
#include "bitboard.h"

// restituisce una penalita per ogni doubled pawn
int doublePawnPenalty(Board &board){
    int whitePenalty = 0;
    int blackPenalty = 0;

    for (int f = 0; f < 8; f++) {
        int whiteOnFile = popcount(board.pieceBB[WHITE][PAWN] & FILE_MASKS[f]);
        int blackOnFile = popcount(board.pieceBB[BLACK][PAWN] & FILE_MASKS[f]);

        if (whiteOnFile > 1) whitePenalty += (whiteOnFile - 1) * DOUBLED_PAWN_PENALY;
        if (blackOnFile > 1) blackPenalty += (blackOnFile - 1) * DOUBLED_PAWN_PENALY;
    }

    return blackPenalty - whitePenalty;
}

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
    
    score += doublePawnPenalty(board);

    return (board.sideToMove == WHITE) ? score : -score;
}   