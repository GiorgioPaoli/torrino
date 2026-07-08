#include "eval.h"
#include "bitboard.h"


int evaluate(Board &board) {
    int score = 0;
    for (int pt = 0; pt < PIECE_TYPE_NB; pt++) {
        score += popcount(board.pieceBB[WHITE][pt]) * PIECE_WEIGHTS[pt];
        score -= popcount(board.pieceBB[BLACK][pt]) * PIECE_WEIGHTS[pt];
    }   
    
    return (board.sideToMove == WHITE) ? score : -score;
}   