// Perft check: counts every position reachable at a given depth on the 6 standard test positions
// and compares the totals with the published reference values.
// Build and run (from the repo root, Git Bash):
//   g++ -O3 -std=c++17 -Isrc tools/tests/test_perft.cpp $(ls src/*.cpp | grep -v main.cpp) -o tools/tests/test_perft.exe
//   ./tools/tests/test_perft.exe
#include <iostream>
#include <chrono>
#include "board.h"
#include "perft.h"
#include "attacks.h"
#include "zobrist.h"
#include "magics.h"

int main() {
    initZobrist(); initPawnAttacks(); initKnightAttacks(); initKingAttacks(); initSliderMasks(); initMagics();
    struct P { const char *name; const char *fen; int depth; unsigned long long expected; } positions[] = {
        {"start",    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5, 4865609ULL},
        {"kiwipete", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4, 4085603ULL},
        {"pos3",     "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 5, 674624ULL},
        {"pos4",     "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 4, 422333ULL},
        {"pos5",     "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 4, 2103487ULL},
        {"pos6",     "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 4, 3894594ULL},
    };
    int failures = 0;
    for (auto &p : positions) {
        Board board; board.loadFEN(p.fen);
        auto t0 = std::chrono::steady_clock::now();
        unsigned long long nodes = perft(board, p.depth);
        double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
        bool ok = nodes == p.expected;
        if (!ok) failures++;
        std::cout << (ok ? "OK   " : "FAIL ") << p.name << " depth " << p.depth << ": " << nodes
                  << " (expected " << p.expected << ")  " << secs << " s\n";
    }
    std::cout << (failures ? "PERFT FAILED" : "all perft counts correct") << "\n";
    return failures != 0;
}
