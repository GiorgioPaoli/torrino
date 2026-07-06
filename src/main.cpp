#include <iostream>
#include <bitset>

#include "board.h"
#include "attacks.h"



int main(){
    Board board;

    board.initBoard();
    // board.clearBoard();

    // setBit(board.pieceBB[WHITE][BISHOP], C1);
    // setBit(board.pieceBB[BLACK][BISHOP], G8);
    // setBit(board.pieceBB[WHITE][KNIGHT], F4);
    // setBit(board.pieceBB[BLACK][BISHOP], B7);
    // setBit(board.pieceBB[WHITE][ROOK], G1);
    // setBit(board.pieceBB[BLACK][QUEEN], F2);

    board.updateOccupancy();
    board.syncMailboxFromBitboards();

    board.printBoard();

    initPawnAttacks();
    initKnightAttacks();
    initKingAttacks();

    Move move;
    move.fromSquare = E2;
    move.toSquare = E4;
    move.flags = DOUBLE_PAWN_PUSH;

    UndoInfo undo;

    board.makeMove(move, undo);
    
    move.fromSquare = B8;
    move.toSquare = C6;
    move.flags = QUIET;
    
    board.makeMove(move, undo);

    board.printBoard();
    // board.printBoardFromBB(bishopAttacksSlow(B7, board.occupiedAll));
    // board.printBoardFromBB(rookAttacksSlow(G1, board.occupiedAll));
    // board.printBoardFromBB(queenAttacksSlow(F2, board.occupiedAll));

    
    
    

}