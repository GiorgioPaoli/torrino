#include <iostream>
#include <bitset>

#include "board.h"
#include "attacks.h"



int main(){
    Board board;

    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    // board.initBoard();
    // board.clearBoard();
    board.loadFEN(fen);

    board.updateOccupancy();
    board.syncMailboxFromBitboards();

    std::cout << '\n' << fen << std::endl;
    board.printBoard();

    initPawnAttacks();
    initKnightAttacks();
    initKingAttacks();

    // Move move {E1, G1, CASTLE_KINGSIDE};
    // UndoInfo undo;

    // board.makeMove(move, undo);

    // move = {B7, B5, DOUBLE_PAWN_PUSH};
    // board.makeMove(move, undo);

    // board.printBoard();

    // move = {C5, B6, EN_PASSANT};   
    // board.makeMove(move, undo);

    // move = {H7, H8, PROMOTION_KNIGHT};
    // board.sideToMove = WHITE;
    // board.makeMove(move, undo);

    // move = {D2, F1, CAPTURE};
    // board.makeMove(move, undo);    

    // board.printBoard();
}