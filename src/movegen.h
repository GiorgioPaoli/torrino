#include "board.h"
#include "move.h"

/*
    Generazione mosse Pseudo Legali per ogni pezzo e colore usando le tabelle
    già pronte in attacks.h. 
    Decido la flag da mettere in base allo stato della scacchiera e in base alla
    mossa (casella vuota -> QUIET, pedone su ultimo rank -> PROMOTION_*)

    Filtro legalità, per ogni mossa pseudo-legale: makeMove, isSquareAttacked(casa proprio re, colore avversario),
    se sotto scacco -> scarto la mossa

    RandomMove generation

    Perft
*/


void generatePawnMoves(Board &board, MoveList &list, Color us);
void generateKnightMoves(Board &board, MoveList &list, Color us);
void generateBishopMoves(Board &board, MoveList &list, Color us);
void generateRookMoves(Board &board, MoveList &list, Color us);
void generateQueenMoves(Board &board, MoveList &list, Color us);
void generateKingMoves(Board &board, MoveList &list, Color us);

void generatePseudoLegalMoves(Board &board, MoveList &list);
void generateLegalMoves(Board &board, MoveList &list);