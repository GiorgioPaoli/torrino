#include "perft.h"
#include <iostream>
#include <cstdlib>

uint64_t perft(Board &board, int depth){
    if (depth == 0) return 1;

    MoveList list;
    generateLegalMoves(board, list);
    
    uint64_t nodes = 0;
    for (int i = 0; i < list.count; i++){
        UndoInfo undo;
        uint64_t hash = board.hash;
        board.makeMove(list.moves[i], undo);
        if (board.hash != board.computeHash()) {
            std::cout   << moveToString(list.moves[i]) << std::endl;
            board.printBoard();
            std::exit(1);
        }
        nodes += perft(board, depth - 1);
        board.unmakeMove(list.moves[i], undo);
        if (board.hash != hash) {
            std::cout   << moveToString(list.moves[i]) << std::endl;
            board.printBoard();
            std::exit(1);
        }
    }

    return nodes;
}

uint64_t perftDivide(Board &board, int depth){
    MoveList list;
    generateLegalMoves(board, list);
    
    uint64_t total = 0;
    for (int i = 0; i < list.count; i++){
        UndoInfo undo;
        board.makeMove(list.moves[i], undo);
        uint64_t nodes = perft(board, depth - 1);
        board.unmakeMove(list.moves[i], undo);

        std::cout << moveToString(list.moves[i]) << ": " << nodes << '\n';
        total += nodes;
    }

    std::cout << '\n' << "Total nodes: " << total << '\n';
    return total;

}

