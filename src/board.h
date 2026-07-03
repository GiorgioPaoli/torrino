#pragma once
#include <cstdint>

#include "types.h"



class Board {
public:
    
    // caselle occupate dal colore e quelle di tutto
    uint64_t occupiedByColor[COLOR_NB];
    uint64_t occupiedAll;

    uint64_t pieceBB[COLOR_NB][PIECE_TYPE_NB];  // bitboards per ogni pezzo e ogni colore,
                                                // è semplicemente la somma di tutti quelli sopra q

    Piece board[64];                            // array in cui ci sono tutti i pezzi della scacchiera

    Color sideToMove;           
    int enPassantSquare;        // indice da 0 a 63, -1 se no c'è
    int castlingRights;         // usa i primi 4 bit:   0 0 0 0
                                // in ordine per bianco e nero:
                                // (black) long, short, (white) long, short

    int halfMoveClock;          // contantore per la regola delle 50 mosse


    void initBoard();
    bool isOccupied();
    bool isWhitePiece();
    bool isBlackPiece();
    
    void updateOccupancy();
    void syncMailboxFromBitboards();

    inline Piece makePiece(Color c, PieceType pt) {
        return static_cast<Piece>(c * 6 + pt);
    }

    // --- HELPER FUNCTIONS ---
    void printBoard();

    // --- INLINE FUNCTIONS ---

    
};