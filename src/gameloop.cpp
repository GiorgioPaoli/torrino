#include <iostream>

#include "gameloop.h"
#include "attacks.h"
#include "movegen.h"

void gameloop(int depth){
    Board board;
    board.initBoard();
    board.updateOccupancy();
    board.syncMailboxFromBitboards();
    initPawnAttacks(); initKnightAttacks(); initKingAttacks();

    while (true) {
        board.printBoard();

        MoveList list;  // per generare le mosse legali e controllare se è finita la partita
        generateLegalMoves(board, list);
        if (list.count == 0) {
            if (board.isInCheck(board.sideToMove)) {
                std::cout << ((board.sideToMove == WHITE) ? "Nero " : "Bianco ") << "ha vinto. (scacco matto)\n";
            } else {
                std::cout << "Patta (stallo)\n";
            }
            break;
        }
        
        UndoInfo undo;
        if (board.sideToMove == WHITE) {
            std::string input;
            std::getline(std::cin, input);
            Move move = parseMove(board, input);
            if (move.fromSquare == -1) {
                std::cout << "Mossa non valida\n";
                continue;
            } else {
                board.makeMove(move, undo);
            }
        } else {
            Move move = findBestMove(board, depth);
            std::cout << "Il motore gioca: " << moveToString(move) << "\n";
            board.makeMove(move, undo);
        }
    }


}
