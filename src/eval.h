#pragma once

#include "board.h"

constexpr int PIECE_WEIGHTS[PIECE_TYPE_NB] = {
    100,    // PAWN
    320,    // KNIGHT
    330,    // BISHOP
    500,    // ROOK
    900,    // QUEEN
    0       // KING
};

// --- king safety (middlegame only) ---
constexpr int CASTLING_RIGHT_BONUS    = 10;     // per castling right still available (king on its start square)
constexpr int KING_MOVED_PENALTY      = 40;     // king left its start square without castling
constexpr int SHIELD_RANK2_BONUS      = 15;     // shield pawn on its starting rank
constexpr int SHIELD_RANK3_BONUS      = 8;      // shield pawn pushed by one square
constexpr int SHIELD_MISSING_PENALTY  = 20;     // no shield pawn on that file
constexpr int EXPOSED_KING_PER_RANK   = 30;     // per rank beyond the 2nd while the enemy queen is on the board
constexpr int KING_SAFETY_MIN_PHASE   = 6;        // below this phase king safety is skipped (its middlegame weight is tiny)

// weights of the pieces attacking the zone around the king
constexpr int KNIGHT_ATTACK_WEIGHT = 2;
constexpr int BISHOP_ATTACK_WEIGHT = 2;
constexpr int ROOK_ATTACK_WEIGHT   = 3;
constexpr int QUEEN_ATTACK_WEIGHT  = 5;

/*
    Piece Square Tables

    - in futuro bisognera utilizzare quelle con valori tuned, con valori Mid Game e End Game
*/  

constexpr int DOUBLED_PAWN_PENALY = 15;

// sprona il pedone ad andare al centro e avanti per promuovere
// mantiene valori positivi davanti agli arrocchi per proteggere il re 
constexpr int PAWN_PST[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

// il cavallo è incentivato ad andare al centro, in questo modo controlla piu case
constexpr int KNIGHT_PST[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

// l'alfiere evita lati e angoli, preferisce case come b3, c4, d5 ecc
constexpr int BISHOP_PST[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

// 
constexpr int ROOK_PST[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

// la regina è incentivata ad andare al centro per controllare piu case
constexpr int QUEEN_PST[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
     -10,  5,  5,  5,  5,  5,  0,-10,
     -10,  0,  5,  0,  0,  0,  0,-10,
     -20,-10,-10, -5, -5,-10,-10,-20
    };
    
// in mid game il re deve rimanere al coperto agli angoli della scacchiera
constexpr int MG_KING_PST[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

// in endgame il re deve stare al centro
constexpr int EG_KING_PST[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

constexpr const int *PST[PIECE_TYPE_NB] = {
    PAWN_PST, KNIGHT_PST, BISHOP_PST, ROOK_PST, QUEEN_PST, MG_KING_PST
};



int doublePawnPenalty(Board &board);
int countPieces(Board &board);
int centerDistance(int sq);
int kingDistance(int sq1, int sq2);
int castlingSafety(Board &board, int sqw, int sqb);             // small bonus per right to castle, king in no castling positoin -> small penalty
int pawnShield(Board &board, Color side, int kingSq);           // pawn shield, if the kings on first rank and its castled, for each own pawn on 2nd and 3rd rank bonus, elsewhere -> penalty
int exposedKing(Board &board, Color side, int kingSq);          // if the opponent still has a queen and the side king is not on 1st rank -> penalty that increase as ranks go on
int kingAttack(Board &board, Color side, int kingSq);



int evaluate(Board &board);