#pragma once
#include <cstdint>

#include "types.h"



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
    
    // --- HELPER FUNCTIONS ---
    void printBoard(Board board[COLOR_NB][PIECE_NB]);

    // --- INLINE FUNCTIONS ---

    inline void initBoard(){
        // azzero tutto 
        for (int c = 0; c <= COLOR_NB; c++){
            for (int pt = 0; pt <= PIECE_TYPE_NB; pt++){
                pieceBB[c][pt] = 0ULL;
            }
        }

        // pedoni
        pieceBB[WHITE][PAWN] = RANK_2;
        pieceBB[BLACK][PAWN] = RANK_7;
        
        // cavalli
        pieceBB[WHITE][KNIGHT] = SQ_BB(B1) | SQ_BB(G1);
        pieceBB[BLACK][KNIGHT] = SQ_BB(B8) | SQ_BB(G8);

        // alfieri
        pieceBB[WHITE][BISHOP] = SQ_BB(C1) | SQ_BB(F1);
        pieceBB[BLACK][BISHOP] = SQ_BB(C8) | SQ_BB(F8);

        // torri
        pieceBB[WHITE][ROOK] = SQ_BB(A1) | SQ_BB(H1);
        pieceBB[BLACK][ROOK] = SQ_BB(A8) | SQ_BB(H8);

        // regine
        pieceBB[WHITE][QUEEN] = SQ_BB(D1);
        pieceBB[BLACK][QUEEN] = SQ_BB(D8);

        // re
        pieceBB[WHITE][KING] = SQ_BB(E1);
        pieceBB[BLACK][KING] = SQ_BB(E8);

    }

};