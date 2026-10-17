#pragma once

#include <chrono>
#include "eval.h"

#define INF      1000000 // 1 mln
#define MATE     -100000 // 100k

extern long long nodesCount; 

extern int movesPlayed;

extern bool stopSearch;
extern std::chrono::steady_clock::time_point deadline; 

/*
    Utilizza Negamax, variante di minimax, non controlla di chi è il turno
    facendo un ramo per WHITE e uno per BLACK. Negamax utilizza un solo caso
    e sfrutta il fatto che max(a, b) = -min(-a, -b)

    !!!     TODO    !!!
    - scrivere alpha beta pruning
    - utilizzare move ordering per pruning
    - utilizzare zobrist hashing
    - utilizzare traspositions table

*/

/*
    Restituisce il punteggio di quella mossa a profondita depth
    Implementa alpha beta pruning e move orderibng
*/
int negamax(Board &board, int depth, int alpha, int beta);

Move findBestMove(Board &board, int depth, Move prevMove);
Move findBestMoveTime(Board &board, int timeForMove);

int quiescence(Board &board, int alpha, int beta);

constexpr int max(int a, int b) {return (a >= b) ? a : b;}
constexpr int min(int a, int b) {return (a <= b) ? a : b;}

