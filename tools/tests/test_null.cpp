// Null move check: at every node of perft trees on 4 positions (one with an en passant square), makes and
// unmakes a null move and checks that the hash equals a full recomputation and that the state is restored.
// Build and run (from the repo root, Git Bash):
//   g++ -O2 -std=c++17 -Isrc tools/tests/test_null.cpp $(ls src/*.cpp | grep -v main.cpp) -o tools/tests/test_null.exe
//   ./tools/tests/test_null.exe
#include <iostream>
#include "board.h"
#include "movegen.h"
#include "attacks.h"
#include "zobrist.h"
#include "magics.h"

long long checks = 0, errors = 0;

void walk(Board &b, int depth) {
    Color side = b.sideToMove;
    int ep = b.enPassantSquare, halfMove = b.halfMoveClock, histCount = b.historyCount;
    uint64_t hash = b.hash;

    UndoInfo u;
    b.makeNullMove(u);
    if (b.hash != b.computeHash() || b.sideToMove == side || b.enPassantSquare != -1 || b.historyCount != histCount + 1)
        errors++;
    b.unmakeNullMove(u);
    if (b.sideToMove != side || b.enPassantSquare != ep || b.halfMoveClock != halfMove ||
        b.historyCount != histCount || b.hash != hash)
        errors++;
    checks++;

    if (depth == 0) return;
    MoveList list;
    generateLegalMoves(b, list);
    for (int i = 0; i < list.count; i++) {
        UndoInfo undo;
        b.makeMove(list.moves[i], undo);
        walk(b, depth - 1);
        b.unmakeMove(list.moves[i], undo);
    }
}

int main() {
    initZobrist(); initPawnAttacks(); initKnightAttacks(); initKingAttacks(); initSliderMasks(); initMagics();
    const char *fens[] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3",   // en passant possible
    };
    int depths[] = {3, 3, 4, 3};
    for (int i = 0; i < 4; i++) { Board b; b.loadFEN(fens[i]); walk(b, depths[i]); }
    std::cout << checks << " null-move round trips, " << errors << " errors\n";
    return errors != 0;
}
