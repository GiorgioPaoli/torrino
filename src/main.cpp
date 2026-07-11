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

    // std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    // board.initBoard();
    // board.clearBoard();
    // board.loadFEN(fen);
    
    // board.printBoard();
    
    // int depth = 5;

    // Move best = findBestMove(board, depth);
    // std::cout << "migliore: " << moveToString(best) << "\n";
    // std::cout << doublePawnPenalty(board);

    // gameloop(depth);
    
    uciLoop();
    
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