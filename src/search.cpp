#include <iostream>
#include <chrono>
#include "search.h"
#include "movegen.h"
using namespace std::chrono;

long long nodesCount = 0;

bool stopSearch = false;
steady_clock::time_point deadline; 


int negamax(Board &board, int depth, int alpha, int beta){
    nodesCount++;
    // questi due sono stop per il time management
    if ((nodesCount & 2047) == 0) {
        if (steady_clock::now() > deadline) stopSearch = true;
    }
    if (stopSearch) return 0;
    
    if (depth == 0) {
        return evaluate(board);
    }

    Color us = board.sideToMove;

    /*
        Genera SOLO mosse Pseudo legali, controlla la legalita direttamente
        dentro il ciclo for, e alla fine se non ci sono mosse legali
        restituisce matto o patta
    */
    MoveList list;
    generatePseudoLegalMoves(board, list);

    // ordina le mosse mettendo prima le catture e promozioni, rende piu efficiente alpha beta pruning
    orderMoves(board, list);

    int bestScore = -INF;
    int legalMoves = 0;

    for (int i = 0; i < list.count; i++) {
        UndoInfo undo;
        board.makeMove(list.moves[i], undo);

        if (board.isInCheck(us)) {
            board.unmakeMove(list.moves[i], undo);
            continue;
        }
        
        legalMoves++;
        int score = -negamax(board, depth - 1, -beta, -alpha);
        board.unmakeMove(list.moves[i], undo);

        if (score > bestScore) bestScore = score;
        if (bestScore > alpha) alpha = bestScore;
        if (alpha >= beta) break; 
    }

    if (legalMoves == 0) {
        if (board.isInCheck(us)) return MATE - depth;
        else return 0;
    }

    return bestScore;
}   


Move findBestMove(Board &board, int depth, Move prevMove){
    // per debug, conta quante posizioni vede e starta il tempo, alla fine printa quanti nodi ha visto,
    // in quanto tempo, NPS e bestMove
    // nodesCount = 0;
    // auto start = std::chrono::steady_clock::now();
    

    MoveList list;
    generateLegalMoves(board, list);
    if (list.count == 0) {
        return Move{-1, -1, QUIET};
    }

    // mette la mossa migliore della search precedente in cima per ottimizzare alpha beta pruning
    if (prevMove.fromSquare != -1) {
        for (int i = 0; i < list.count; i++) {
            if (list.moves[i].fromSquare == prevMove.fromSquare &&
                list.moves[i].toSquare   == prevMove.toSquare &&
                list.moves[i].flags      == prevMove.flags) {
                Move tmp = list.moves[0];
                list.moves[0] = list.moves[i];
                list.moves[i] = tmp;
                break;
            }
        }
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

Move findBestMoveTime(Board &board, int timeForMove){
    auto start = steady_clock::now();
    deadline = start + milliseconds(timeForMove);
    stopSearch = false;
    nodesCount = 0;

    Move bestMove {-1, -1, QUIET};

    for (int depth = 1; depth < 64; depth++) {
        Move move = findBestMove(board, depth, bestMove);

        if (stopSearch) break;
        
        bestMove = move;
        
        // stop morbido, se ho gia consumato meta budget, non parto con depth + 1 
        auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
        
        // DEBUG per nodi, mossa, tempo e profondita
        // long long nps = (elapsed > 0) ? (nodesCount * 1000 / elapsed) : 0;
        // std::cout   << "info string depth: " << depth
        //             << ", nodi: " << nodesCount
        //             << ", tempo: " << elapsed << "ms" 
        //             << ", NPS: " << nps << "/s"
        //             << ", best move: " << moveToString(bestMove) 
        //             << std::endl; 
        

        if (elapsed * 2 > timeForMove) break;
    }

    // std::cout << evaluate(board) << std::endl;
    return bestMove;
}




