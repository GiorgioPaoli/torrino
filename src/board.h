#pragma once
#include <cstdint>

enum Color {WHITE, BLACK, COLOR_NB};                            // traduce in 0, 1, 2
enum Piece {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PIECE_NB}; // traduce in 0, 1, 2, 3, 4, 5, 6

class Board {
public:
    uint64_t whitePawns;
    uint64_t whiteKnights;
    uint64_t whiteBishops;
    uint64_t whiteRooks;
    uint64_t whiteQueen;
    uint64_t whiteKing;

    uint64_t blackPawns;
    uint64_t blackKnights;
    uint64_t blackBishops;
    uint64_t blackRooks;
    uint64_t blackQueen;
    uint64_t blackKing;
    
    uint64_t allWhitePieces;
    uint64_t allBlackPieces;
    uint64_t allPieces;

    int sideToMove;             // 0 per bianco, 1 per nero
    int enPassantSquare;        // indice da 0 a 63, -1 se no c'è
    int castlingRights;         // usa i primi 4 bit:   0 0 0 0
                                // in ordine per bianco e nero:
                                // (black) long, short, (white) long, short

    int halfMoveClock;          // contantore per la regola delle 50 mosse


    void initBoard();
    bool isOccupied();
    bool isWhitePiece();
    bool isBlackPiece();
    

};