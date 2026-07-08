#include "search.h"
#include "movegen.h"

/*
    Azioni:
    - non ci sono mosse in questa posizione? -> return 0 (patta)
    - depth == 0? -> return evaluate(position)
    max = -INF
    - per ogni mossa disponibile nella posizione:
        fai la mossa
        calcola il punteggio = -negamax(depth - 1)
        unkameMove
        se il punteggio è maggiore del massimo finora -> max = score
    return max
*/
int negamax(Board &board, int depth){
    if (depth == 0) {
        return evaluate(board);
    }
    MoveList list;
    generateLegalMoves(board, list);
    if (list.count == 0) {
        if (board.isInCheck(board.sideToMove)) {
            return MATE + depth;
        } else {
            return 0;
        }
    }

    int max = -INF;
    for (int i = 0; i < list.count; i++) {
        UndoInfo undo;
        board.makeMove(list.moves[i], undo);
        int score = -negamax(board, depth - 1);
        board.unmakeMove(list.moves[i], undo);
        if (score > max) {
            max = score;
        }
    }

    return max;
}   

Move findBestMove(Board &board, int depth){
    MoveList list;
    generateLegalMoves(board, list);
    if (list.count == 0) {
        return Move{-1, -1, QUIET};
    }

    Move bestMove {-1, -1, QUIET};
    int bestScore = -INF;

    for (int i = 0; i < list.count; i++){
        UndoInfo undo;
        board.makeMove(list.moves[i], undo);
        int score = -negamax(board, depth - 1);
        board.unmakeMove(list.moves[i], undo);
        if (score > bestScore) {
            bestScore = score;
            bestMove = list.moves[i];
        }
    }

    return bestMove;
}