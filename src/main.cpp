#include <iostream>
#include <bitset>

#include "board.h"
#include "attacks.h"



int main(){
    Board board;

    // board.initBoard();
    board.clearBoard();

    setBit(board.pieceBB[WHITE][BISHOP], C1);
    setBit(board.pieceBB[BLACK][BISHOP], G8);
    setBit(board.pieceBB[WHITE][KNIGHT], F4);
    setBit(board.pieceBB[BLACK][BISHOP], B7);
    setBit(board.pieceBB[WHITE][ROOK], G1);
    setBit(board.pieceBB[BLACK][QUEEN], F2);

    board.updateOccupancy();
    board.syncMailboxFromBitboards();

    board.printBoard();

    initPawnAttacks();
    initKnightAttacks();
    initKingAttacks();

    // board.printBoardFromBB(bishopAttacksSlow(B7, board.occupiedAll));
    // board.printBoardFromBB(rookAttacksSlow(G1, board.occupiedAll));
    // board.printBoardFromBB(queenAttacksSlow(F2, board.occupiedAll));

    for (int c = 0; c < COLOR_NB; c++){
        std::cout << "Look up table attacchi Pedoni " << ((c == WHITE) ? "bianchi" : "neri") << std::endl;
        for (int sq = 0; sq < 64; sq++){
            std::cout << pawnAttacks[c][sq] << ", ";
        }
        std::cout << "\n\n";
    }

    std::cout << "Look up table cavalli: " << std::endl;
    for (int sq = 0; sq < 64; sq++){
        std::cout << knightAttacks[sq] << ", ";
    }
    
    

}