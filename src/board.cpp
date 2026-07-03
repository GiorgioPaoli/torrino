#include <iostream>
#include "board.h"

void Board::initBoard(){
    // azzero tutto 
    for (int c = 0; c < COLOR_NB; c++){
        for (int pt = 0; pt < PIECE_TYPE_NB; pt++){
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
 
    updateOccupancy();
    syncMailboxFromBitboards();

    sideToMove = WHITE;
    // enPassant
    castlingRights = 0b1111;
    halfMoveClock = 0;
}

void Board::updateOccupancy(){
    occupiedByColor[WHITE] = 0ULL;
    occupiedByColor[BLACK] = 0ULL;
    for (int pt = 0; pt <= PIECE_TYPE_NB; pt++){
        occupiedByColor[WHITE] |= pieceBB[WHITE][pt];
        occupiedByColor[BLACK] |= pieceBB[BLACK][pt];
    }
    occupiedAll = occupiedByColor[WHITE] | occupiedByColor[BLACK];
}

void Board::syncMailboxFromBitboards(){
    for (int sq = 0; sq < 64; sq++) board[sq] = NO_PIECE;

    for (int c = 0; c < COLOR_NB; c++) {
        for (int pt = 0; pt < PIECE_TYPE_NB; pt++) {
            uint64_t bb = pieceBB[c][pt];
            while (bb) {
                int sq = __builtin_ctzll(bb);
                board[sq] = makePiece((Color)c, (PieceType)pt);
                bb &= bb - 1;
            }
        }
    }
}

void Board::printBoard(){
    const char* pieceChars = "PNBRQKpnbrqk.";  // maiuscolo bianco, minuscolo nero, '.' vuoto
    
    std::cout << '\n';
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << (rank + 1) << "  ";
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            std::cout << pieceChars[board[sq]] << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "\n   a b c d e f g h\n";

}