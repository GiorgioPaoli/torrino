#pragma once
#include "types.h"

enum Flags {
    QUIET, /*mossa semplice*/
    CAPTURE,
    DOUBLE_PAWN_PUSH,
    EN_PASSANT,
    CASTLE_KINGSIDE,
    CASTLE_QUEENSIDE,
    PROMOTION_QUEEN,
    PROMOTION_ROOK,
    PROMOTION_BISHOP,
    PROMOTION_KNIGHT,
    PROMOTION_QUEEN_CAPTURE,
    PROMOTION_ROOK_CAPTURE,
    PROMOTION_BISHOP_CAPTURE,
    PROMOTION_KNIGHT_CAPTURE,
    FLAGS_NB
};

struct Move {
    int fromSquare;
    int toSquare;
    int flags; // ci sono 14 flags, assegna da 0 a 13, entrano in un binario di 4 bit
};

struct MoveList {
    Move moves[256];
    int count = 0;

    void add(Move m){
        moves[count++] = m;
    }
};

struct UndoInfo {
    Piece capturedPiece;
    int castlingRights;
    int enPassantSquare;
    int halfMoveClock;
};

constexpr bool isCapture(Flags flag){
    return flag == CAPTURE 
        || flag == EN_PASSANT
        || flag == PROMOTION_QUEEN_CAPTURE
        || flag == PROMOTION_ROOK_CAPTURE
        || flag == PROMOTION_BISHOP_CAPTURE
        || flag == PROMOTION_KNIGHT_CAPTURE;
}

constexpr bool isSameMove(Move move1, Move move2){
    return move1.fromSquare == move2.fromSquare 
        && move1.toSquare == move2.toSquare
        && move1.flags == move2.flags; 
}

constexpr bool isQuiet(Flags flag){
    return flag == QUIET
        || flag == DOUBLE_PAWN_PUSH;
}

constexpr bool isPromotion(Flags flag){
    return flag >= PROMOTION_QUEEN 
        && flag <= PROMOTION_KNIGHT_CAPTURE;
} 


