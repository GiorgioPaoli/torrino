#include <iostream>
#include <bitset>
#include <chrono>
#include <iomanip>

#include "board.h"
#include "attacks.h"
#include "perft.h"
#include "gameloop.h"
#include "uci.h"
#include "magics.h"
#include "tt.h"
#include "zobrist.h"


int main(){
    Board board;
    // initZobrist();
    // initPawnAttacks();
    // initKnightAttacks();
    // initKingAttacks();
    // initSliderMasks();
    // initPawnMasks();
    // initMagics();
    // clearTT();

    // std::string fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
    // board.initBoard();
    // board.clearBoard();
    // board.loadFEN(fen);
    
    // board.printBoard();
    
    // std::cout << "eval: " << evaluate(board) << "\n";

    
    // std::string fen = "rnbqkbnr/pppppppp/8/8/8/2Q5/PPPPPPPP/RNB1KBNR b KQkq - 0 1";
    // board.loadFEN(fen);
    // board.printBoard();
    // std::cout << "evaluation: " << evaluate(board) << '\n';
    
    // perftDivide(board, 4);

    // int depth = 10;

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