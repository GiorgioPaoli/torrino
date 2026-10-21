#include <cstdlib>
#include "eval.h"
#include "bitboard.h"
#include "search.h"

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

/*
    VERY IMPORTANT

    evaluate() gives a relative score to the side TO MOVE

    So > 0 is always relative to the side who's turn is to move, even if it's black to move.
    > 0 is a winning evaluation, < 0 is losing.
*/

// gives a total score based on which and how many pieces are left on the board, doesn't matter which side
int countPieces(Board &board){
    int knightValue = 1;
    int bishopValue = 1;
    int rookValue = 2;
    int queenValue = 4;
    int totalScore = 0;
    
    totalScore += popcount(board.pieceBB[WHITE][KNIGHT] | board.pieceBB[BLACK][KNIGHT])  * knightValue;
    totalScore += popcount(board.pieceBB[WHITE][BISHOP] | board.pieceBB[BLACK][BISHOP])  * bishopValue;
    totalScore += popcount(board.pieceBB[WHITE][ROOK]   | board.pieceBB[BLACK][ROOK])    * rookValue;
    totalScore += popcount(board.pieceBB[WHITE][QUEEN]  | board.pieceBB[BLACK][QUEEN])   * queenValue;

    return totalScore;
}

int centerDistance(int sq){
    int file = fileOf(sq);
    int rank = rankOf(sq);
    int fileDist = max(3 - file, file - 4);
    int rankDist = max(3 - rank, rank - 4);
    return fileDist + rankDist;
}

int kingDistance(int sq1, int sq2){
    int fileDiff = abs(fileOf(sq1) - fileOf(sq2));
    int rankDiff = abs(rankOf(sq1) - rankOf(sq2));
    return fileDiff + rankDiff;
}

int evaluate(Board &board) {
    int mg = 0; // middlegame
    int eg = 0; // endgame
    int phase = 0;
    int piecesNotDevelopedWhite = 0;
    int piecesNotDevelopedBlack= 0;
    int whiteMaterial = 0;
    int blackMaterial = 0;

    phase = countPieces(board);
    if (phase > 24) phase = 24;

    // penalty for doubled pawns = 15 per doubled pawn
    int doublePawns = doublePawnPenalty(board);
    mg += doublePawns;
    eg += doublePawns;
    
    // penalty for non developed pieces
    if (board.board[B1] == W_KNIGHT) {mg -= 30; piecesNotDevelopedWhite++;}
    if (board.board[F1] == W_BISHOP) {mg -= 30; piecesNotDevelopedWhite++;}
    if (board.board[C1] == W_BISHOP) {mg -= 30; piecesNotDevelopedWhite++;}
    if (board.board[G1] == W_KNIGHT) {mg -= 30; piecesNotDevelopedWhite++;}
    
    if (board.board[B8] == B_KNIGHT) {mg += 30; piecesNotDevelopedBlack++;}
    if (board.board[F8] == B_BISHOP) {mg += 30; piecesNotDevelopedBlack++;}
    if (board.board[C8] == B_BISHOP) {mg += 30; piecesNotDevelopedBlack++;}
    if (board.board[G8] == B_KNIGHT) {mg += 30; piecesNotDevelopedBlack++;}
    
    // penalty for moving queen too early
    if (piecesNotDevelopedWhite > 1) {
        if (popcount(board.pieceBB[WHITE][QUEEN]) != 0) {
            mg -= (board.board[D1] == W_QUEEN) ? 0 : 80; 
        } 
    }
    if (piecesNotDevelopedBlack > 1) {
        if (popcount(board.pieceBB[BLACK][QUEEN]) != 0) {
            mg += (board.board[D8] == B_QUEEN) ? 0 : 80;
        }
    }
    
    // counts pieces and assigns a score based on the PST, EXCEPT THE KING
    for (int pt = 0; pt < KING; pt++) {
        uint64_t wbb = board.pieceBB[WHITE][pt];
        uint64_t bbb = board.pieceBB[BLACK][pt];
        while (wbb) {
            int sq = popLsb(wbb);
            mg += PIECE_WEIGHTS[pt] + PST[pt][sq ^ 56];  // IMPORTANT, this flips the PST
            eg += PIECE_WEIGHTS[pt] + PST[pt][sq ^ 56];
            if (pt != PAWN) whiteMaterial += PIECE_WEIGHTS[pt];
        } 
        while (bbb) {
            int sq = popLsb(bbb);
            mg -= PIECE_WEIGHTS[pt] + PST[pt][sq];
            eg -= PIECE_WEIGHTS[pt] + PST[pt][sq];
            if (pt != PAWN) blackMaterial += PIECE_WEIGHTS[pt];
        }
    }
    
    // tapered evaluation for the king
    uint64_t wk = board.pieceBB[WHITE][KING];
    uint64_t bk = board.pieceBB[BLACK][KING];
    int sqw = popLsb(wk);
    int sqb = popLsb(bk);
    mg += MG_KING_PST[sqw ^ 56] - MG_KING_PST[sqb];
    eg += EG_KING_PST[sqw ^ 56] - EG_KING_PST[sqb];
    // Mop-up, pushing the enemy king to the corner based on piece material advantage
    int materialImbalance = whiteMaterial - blackMaterial;
    int bonus = 0;
    if (materialImbalance >= 400) {
        // centerDistance(losingKing) ... kingDistance(winningKing, losingKing)
        bonus = 10 * centerDistance(sqb) + 4 * (14 - kingDistance(sqw, sqb));
        eg += bonus;
    } else if (materialImbalance <= -400) {
        bonus = 10 * centerDistance(sqw) + 4 * (14 - kingDistance(sqb, sqw));
        eg -= bonus;
    } 
    
    int score = (mg * phase + eg * (24 - phase)) / 24;

    return (board.sideToMove == WHITE) ? score : -score;
}   
