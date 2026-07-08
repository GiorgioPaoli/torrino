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

    // std::string fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
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