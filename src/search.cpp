#include <iostream>
#include <chrono>
#include "search.h"
#include "movegen.h"

long long nodesCount = 0;

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

int negamax(Board &board, int depth, int alpha, int beta){
    nodesCount++;
    if (depth == 0) {
        return evaluate(board);
    }
    
    MoveList list;
    generateLegalMoves(board, list);
    if (list.count == 0) {
        if (board.isInCheck(board.sideToMove)) {
            return MATE - depth;
        } else {
            return 0;
        }
    }

    // ordina le mosse mettendo prima le catture e promozioni, rende piu efficiente alpha beta pruning
    orderMoves(board, list);

    int bestScore = -INF;
    for (int i = 0; i < list.count; i++) {
        UndoInfo undo;
        board.makeMove(list.moves[i], undo);
        int score = -negamax(board, depth - 1, -beta, -alpha);
        board.unmakeMove(list.moves[i], undo);
        if (score > bestScore) bestScore = score;
        if (bestScore > alpha) alpha = bestScore;
        if (alpha >= beta) break; 
    }

    return bestScore;
}   

Move findBestMove(Board &board, int depth){
    // per debug, conta quante posizioni vede e starta il tempo, alla fine printa quanti nodi ha visto,
    // in quanto tempo, NPS e bestMove
    // nodesCount = 0;
    // auto start = std::chrono::steady_clock::now();

    MoveList list;
    generateLegalMoves(board, list);
    if (list.count == 0) {
        return Move{-1, -1, QUIET};
    }

    Move bestMove {-1, -1, QUIET};
    int bestScore = -INF;
    int alpha = -INF;
    int beta = INF;

    for (int i = 0; i < list.count; i++){
        UndoInfo undo;
        board.makeMove(list.moves[i], undo);
        int score = -negamax(board, depth - 1, -beta, -alpha);
        board.unmakeMove(list.moves[i], undo);

        if (score > bestScore) {
            bestScore = score;
            bestMove = list.moves[i];
        }
        if (bestScore > alpha) alpha = bestScore;
    }


    // print per DEBUG
    // auto end = std::chrono::steady_clock::now();
    // auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // std::cout << "info string nodi " << nodesCount
    //           << "  tempo: " << ms << " ms"
    //           << "  NPS: " << (ms > 0 ? nodesCount * 1000 / ms : 0) << "\n";


    return bestMove;
}




