#include <iostream>
#include <chrono>
#include <cstdlib>
#include "search.h"
#include "movegen.h"
using namespace std::chrono;

long long nodesCount = 0;
int rootScore = 0;
// MAX_PLY = 64
Move killers[MAX_PLY][2];

bool stopSearch = false;
steady_clock::time_point deadline; 


int quiescence(Board &board, int alpha, int beta){
    nodesCount++;
    // questi due sono stop per il time management
    if ((nodesCount & 2047) == 0) {
        if (steady_clock::now() > deadline) stopSearch = true;
    }
    if (stopSearch) return 0;

    int standPat = evaluate(board);
    if (standPat >= beta) return standPat;
    if (standPat > alpha) alpha = standPat;

    int bestScore = standPat;
    Color us = board.sideToMove;

    MoveList list;
    generateCaptures(board, list);

    int scores[256];
    scoreMoves(board, list, scores, {-1, -1, QUIET}, {-1, -1, QUIET});

    for (int i = 0; i < list.count; i++){
        pickMove(list, scores, i);
        UndoInfo undo;
        board.makeMove(list.moves[i], undo);
        if (board.isInCheck(us)) {
            board.unmakeMove(list.moves[i], undo);
            continue;
        }
        
        int score = -quiescence(board, -beta, -alpha);
        board.unmakeMove(list.moves[i], undo);
        
        if (score > bestScore) bestScore = score;
        if (bestScore > alpha) alpha = bestScore;
        if (alpha >= beta) break;
    } 

    return bestScore;
}


int negamax(Board &board, int depth, int alpha, int beta, int ply){
    nodesCount++;
    // time management
    if ((nodesCount & 2047) == 0) {
        if (steady_clock::now() > deadline) stopSearch = true;
    }
    if (stopSearch) return 0;
    // safe stop for ply
    if (ply > MAX_PLY - 1) return evaluate(board);

    // checks for repetitions or HalfMoveClock (100 = 50 moves without captures or pawn moves)
    if (board.isRepetition() || board.halfMoveClock >= 100) return 0;

    // checks extension
    Color us = board.sideToMove;
    bool isInCheckVar = board.isInCheck(us);
    if (isInCheckVar) depth++;
    if (depth == 0) {
        return quiescence(board, alpha, beta);
    }


    /*
        Genera SOLO mosse Pseudo legali, controlla la legalita direttamente
        dentro il ciclo for, e alla fine se non ci sono mosse legali
        restituisce matto o patta
    */
    MoveList list;
    generatePseudoLegalMoves(board, list);

    int scores[256];
    scoreMoves(board, list, scores, killers[ply][0], killers[ply][1]);

    int bestScore = -INF;
    int legalMoves = 0;

    for (int i = 0; i < list.count; i++) {
        pickMove(list, scores, i);
        UndoInfo undo;
        board.makeMove(list.moves[i], undo);

        if (board.isInCheck(us)) {
            board.unmakeMove(list.moves[i], undo);
            continue;
        }
        
        legalMoves++;
        int score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
        board.unmakeMove(list.moves[i], undo);

        if (score > bestScore) bestScore = score;
        if (bestScore > alpha) alpha = bestScore;
        if (alpha >= beta) {
            if (isQuiet((Flags)list.moves[i].flags) && !isSameMove(killers[ply][0], list.moves[i])) {
                killers[ply][1] = killers[ply][0];
                killers[ply][0] = list.moves[i];
            }
            break;
        }
    }

    if (legalMoves == 0) {
        if (isInCheckVar) return MATE + ply;
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
        int score = -negamax(board, depth - 1, -beta, -alpha, 1);
        board.unmakeMove(list.moves[i], undo);

        if (stopSearch) break;      // this move wasnt fully searched, ignore its score

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

    rootScore = bestScore;

    return bestMove;
}

Move findBestMoveTime(Board &board, int softTime, int hardTime, int maxDepth){
    auto start = steady_clock::now();
    deadline = start + milliseconds(hardTime);
    stopSearch = false;
    nodesCount = 0;
    Move nullMove {-1, -1, QUIET};
    // resets the killers array at every new move
    for (int i = 0; i < MAX_PLY; i++) {
        for (int j = 0; j < 2; j++) {
            killers[i][j] = nullMove;
        }
    }

    Move bestMove {-1, -1, QUIET};

    MoveList list;
    generateLegalMoves(board, list);
    if (list.count == 1) {
        return Move(list.moves[0]);
    }

    long long lastIterTime = 0;
    long long prevIterTime = 0;
    double factor = 5;

    for (int depth = 1; depth <= maxDepth; depth++) {
        auto startIterTime = steady_clock::now();

        Move move = findBestMove(board, depth, bestMove);
        if (stopSearch) {
            if (!isSameMove(move, nullMove)) {
                bestMove = move;
            }
            break;
        }
        if (isSameMove(move, nullMove)) break;
        
        bestMove = move;
        
        auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
        
        // DEBUG per nodi, mossa, tempo e profondita
        long long nps = (elapsed > 0) ? (nodesCount * 1000 / elapsed) : 0;
        // check for mates in N
        int plies = 0;
        int N = 0;
        if (rootScore > 99000 || rootScore < -99000) {
            plies = 100000 - abs(rootScore);
            N = (rootScore > 0) ? (plies + 1)/2 : -(plies + 1)/2;
        }
        
        std::cout   << "info " 
                    << "depth " << depth;

        if (N != 0) std::cout << " score mate " << N;
        else        std::cout << " score cp " << rootScore; 

                    std::cout<< " nodes " << nodesCount
                    << " nps " << nps
                    << " time " << elapsed
                    << " pv " << moveToString(bestMove) 
                    << std::endl;

        prevIterTime = lastIterTime;
        lastIterTime = duration_cast<milliseconds>(steady_clock::now() - startIterTime).count();

        if (prevIterTime > 5) {
            factor = (double)lastIterTime / prevIterTime;
            if (factor > 10) factor = 10;
            if (factor < 2) factor = 2;
        } 
        
        if (elapsed + lastIterTime * factor > hardTime) break;
    }

    // std::cout << evaluate(board) << std::endl;
    return bestMove;
}




