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
    IMPORTANTISSIMO

    Evaluation da un punteggio RELATIVO a chi deve muovere

    Quindi se una posizione è vantaggiosa per il bianco e tocca al bianco sara >= 0,
    ma se guardo la stessa posizione dal punto di vista del nero, mi dara < 0, perche negativo
    = svantaggioso, e positivo = vantaggioso, indipendentemente dal lato

*/

int evaluate(Board &board) {
    int score = 0;
    int piecesNotDevelopedWhite = 0;
    int piecesNotDevelopedBlack= 0;

    
    // penalita per pezzi non sviluppati
    if (board.board[B1] == W_KNIGHT) {score -= 30; piecesNotDevelopedWhite++;}
    if (board.board[F1] == W_BISHOP) {score -= 30; piecesNotDevelopedWhite++;}
    if (board.board[C1] == W_BISHOP) {score -= 30; piecesNotDevelopedWhite++;}
    if (board.board[G1] == W_KNIGHT) {score -= 30; piecesNotDevelopedWhite++;}
    
    if (board.board[B8] == B_KNIGHT) {score += 30; piecesNotDevelopedBlack++;}
    if (board.board[F8] == B_BISHOP) {score += 30; piecesNotDevelopedBlack++;}
    if (board.board[C8] == B_BISHOP) {score += 30; piecesNotDevelopedBlack++;}
    if (board.board[G8] == B_KNIGHT) {score += 30; piecesNotDevelopedBlack++;}
    
    // penalita per le regine
    if (piecesNotDevelopedWhite > 1) {
        if (popcount(board.pieceBB[WHITE][QUEEN]) != 0) {
            score -= (board.board[D1] == W_QUEEN) ? 0 : 80; 
        } 
    }
    if (piecesNotDevelopedBlack > 1) {
        if (popcount(board.pieceBB[BLACK][QUEEN]) != 0) {
            score += (board.board[D8] == B_QUEEN) ? 0 : 80;
        }
    }
    
    // conta i pezzi che ci sono e assegna il punteggio in base a PST
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
