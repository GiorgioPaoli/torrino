#include <iostream>
#include <bitset>
#include <chrono>
#include <iomanip>

#include "board.h"
#include "attacks.h"
#include "perft.h"
#include "gameloop.h"
#include "uci.h"


int main(){
    Board board;

    // std::string fen = "6k1/8/5R1K/8/8/8/8/8 w - - 0 1";
    // board.initBoard();
    // board.clearBoard();
    // board.loadFEN(fen);
    int depth = 4;

    // gameloop(depth);
    
    uciLoop();

    // board.updateOccupancy();
    // board.syncMailboxFromBitboards();

    // std::cout << '\n' << fen << std::endl;
    // board.printBoard();
    // std::cout << '\n';
    // board.printBoardFromBB(board.occupiedByColor[WHITE]);
    // board.printBoardFromBB(board.occupiedByColor[BLACK]);


    // initPawnAttacks(); initKnightAttacks(); initKingAttacks();
    // board.printBoard();
    // std::cout << "\nEvaluation: " << negamax(board, 3);

    // auto start = std::chrono::steady_clock::now();

    // uint64_t nodes = perft(board, depth);

    // auto end = std::chrono::steady_clock::now();
    // auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // std::cout << "Profondita: " << depth << "\n";
    // std::cout << "Nodi: " << nodes << "\n";
    // std::cout << "Tempo: " << ms << " ms\n";
    // std::cout << std::fixed << std::setprecision(2)
    //           << (nodes * 1000.0 / ms) / 1000000.0 << " mln nodi/secondo\n";
}