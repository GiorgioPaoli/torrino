#include <iostream>
#include <bitset>

#include "board.h"
#include "attacks.h"



int main(){
    Board board;

    std::string fen = "5n2/4P2p/1r6/2B5/8/8/8/4K2R w K - 0 1";
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

    Move move1 {E7, F8, PROMOTION_QUEEN_CAPTURE};
    UndoInfo undo1;
    Move move2 {B6, D6, QUIET};
    UndoInfo undo2;
    Move move3 {E1, G1, CASTLE_KINGSIDE};
    UndoInfo undo3;
    Move move4 {H7, H5, DOUBLE_PAWN_PUSH};
    UndoInfo undo4;
    Move move5 {C5, D6, CAPTURE};
    UndoInfo undo5;
        

    board.makeMove(move1, undo1);   
    board.makeMove(move2, undo2);   
    board.makeMove(move3, undo3);   
    board.makeMove(move4, undo4);
    board.makeMove(move5, undo5);


    board.printBoard();
    
    board.unmakeMove(move5, undo5);
    board.unmakeMove(move4, undo4);
    board.unmakeMove(move3, undo3);
    board.unmakeMove(move2, undo2);
    board.unmakeMove(move1, undo1);

    board.printBoard();
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