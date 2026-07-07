#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <cctype>

#include "types.h"
#include "move.h"

class Board {
public:
    // da tenere a mente, bisogna tenerli in considerazione per eventuali utilizzi futuri
    uint64_t whitePawns;
    uint64_t whiteKnights;
    uint64_t whiteBishops;
    uint64_t whiteRooks;
    uint64_t whiteQueens;
    uint64_t whiteKing;
    
    uint64_t blackPawns;
    uint64_t blackKnights;
    uint64_t blackBishops;
    uint64_t blackRooks;
    uint64_t blackQueens;
    uint64_t blackKing;

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
    void clearBoard();
    bool isOccupied();
    bool isWhitePiece();
    bool isBlackPiece();
    
    void updateOccupancy();
    void syncMailboxFromBitboards();        // aggiunge tutte le pedine alla board[64]

    // --- MOVE FUNCTIONS ---
    void makeMove(Move m, UndoInfo &undo);
    void unmakeMove(Move m, const UndoInfo &undo);
    
    bool isSquareAttacked(int square, Color bySide);    // restituisce se square è attaccato dal colore bySide

    // --- HELPER FUNCTIONS ---
    void printBoard();
    void printBoardFromBB(uint64_t bb);

    // --- FEN FUNCTIONS ---
    void loadFEN(const std::string &fen);

    // --- INLINE FUNCTIONS ---
    
    
    
};