#pragma once
#include "bitboard.h"

/* 
    - File con tutti gli attacchi per ogni pezzo
    - Tabelle pre-calcolate di attacchi per ongi pezzo

*/

// questi tre sono per i Leaping Pieces, ovvero quelli che "saltano" o si muovono di uno
extern uint64_t pawnAttacks[COLOR_NB][64];
extern uint64_t knightAttacks[64];
extern uint64_t kingAttacks[64];

// Sliding Pieces
// --- DA FARE: MAGIC BITBOARDS ---

/*
    VENGONO CALCOLATE QUNADO SERVE, NON VENGONO INIZIALIZZATE PER ORA 
*/

// arrays for pieces attacks on an EMPTY board
extern uint64_t bishopEmptyAttacks[64];
extern uint64_t rookEmptyAttacks[64];
extern uint64_t queenEmptyAttacks[64];


// --- FUNZIONI INIZIALIZZAZIONE ---
void initPawnAttacks();
void initKnightAttacks();
uint64_t knightAttacksFromSquare(uint64_t sq);
void initKingAttacks();
uint64_t kingAttacksFromSquare(uint64_t sq);

void initSliderMasks();     // fills bishopAttacks/rookAttacks/queenAttacks with empty-board attacks

uint64_t bishopAttacksSlow(int sq, uint64_t occupancy);
uint64_t rookAttacksSlow(int sq, uint64_t occupancy);
uint64_t queenAttacksSlow(int sq, uint64_t occupancy);
