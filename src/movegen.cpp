#include "movegen.h"
#include "attacks.h"


void generatePawnMoves(Board &board, MoveList &list, Color us){
    // deve gestire EnPassant e Promozioni

}

// BLUEPRINT per i commenti
void generateKnightMoves(Board &board, MoveList &list, Color us){
    Color them = opposite(us);
    uint64_t knights = board.pieceBB[us][KNIGHT];

    while (knights){
        // prende il bit meno significativo della BB dei cavalli
        int from = popLsb(knights);

        // prendo le case attaccate e levo le sovrapposizioni con le nostre pedine 
        uint64_t attacks = knightAttacks[from];
        attacks &= ~board.occupiedByColor[us];

        while (attacks) {
            int to = popLsb(attacks);

            // gestione Flags, QUIET / CAPTURE
            Move move {from, to, (getBit(board.occupiedByColor[them], to)) ? CAPTURE : QUIET};

            list.add(move);
        }
    }
}

void generateBishopMoves(Board &board, MoveList &list, Color us){
    Color them = opposite(us);
    uint64_t bishops = board.pieceBB[us][BISHOP];

    while (bishops) {
        int from = popLsb(bishops);

        uint64_t attacks = bishopAttacksSlow(from, board.occupiedAll);
        attacks &= ~board.occupiedByColor[us];

        while (attacks) {
            int to = popLsb(attacks);

            Move move {from, to, (getBit(board.occupiedByColor[them], to)) ? CAPTURE : QUIET};
        
            list.add(move);
        }
    }
}

void generateRookMoves(Board &board, MoveList &list, Color us){
    Color them = opposite(us);
    uint64_t rooks = board.pieceBB[us][ROOK];

    while (rooks) {
        int from = popLsb(rooks);

        uint64_t attacks = rookAttacksSlow(from, board.occupiedAll);
        attacks &= ~board.occupiedByColor[us];

        while (attacks) {
            int to = popLsb(attacks);

            Move move {from, to, (getBit(board.occupiedByColor[them], to)) ? CAPTURE : QUIET};
        
            list.add(move);
        }
    }
}

void generateQueenMoves(Board &board, MoveList &list, Color us){
    Color them = opposite(us);
    uint64_t queens = board.pieceBB[us][QUEEN];

    while (queens) {
        int from = popLsb(queens);

        uint64_t attacks = queenAttacksSlow(from, board.occupiedAll);
        attacks &= ~board.occupiedByColor[us];

        while (attacks) {
            int to = popLsb(attacks);

            Move move {from, to, (getBit(board.occupiedByColor[them], to)) ? CAPTURE : QUIET};
        
            list.add(move);
        }
    }

}

/*
  
    possibile ottimizzazione con magic bitboards
                    |
                    |
                    V
*/
void generateKingMoves(Board &board, MoveList &list, Color us){
    // deve gestire castling controllando se la casella dove si trova, quella di mezzo
    // e quella di arrivo sono sotto attacco, e deve controllare le flags corrispondenti
    Color them = opposite(us);
    uint64_t king = board.pieceBB[us][KING];

    while (king) {
        int from = popLsb(king);
        
        Move move;

        uint64_t betweenKS = (us == WHITE) ? (SQ_BB(F1) | SQ_BB(G1)) : (SQ_BB(F8) | SQ_BB(G8));
        uint64_t betweenQS = (us == WHITE) ? (SQ_BB(D1) | SQ_BB(C1) | SQ_BB(B1)) : (SQ_BB(D8) | SQ_BB(C8) | SQ_BB(B8));

        /*
            Mosse per arrocchi sia bianchi che neri
        */
        // FLAGS per WHITE
        if (us == WHITE) {
            if (getBit(board.castlingRights, 0)) {
                if ((board.occupiedAll & betweenKS) == 0) {
                    if (!board.isSquareAttacked(E1, them)           // casa di partenza non sotto scacco
                        && !board.isSquareAttacked(F1, them)        // casa di passaggio non sotto scacco
                        && !board.isSquareAttacked(G1, them)) {     // casa di arrivo non sotto scacco
                            list.add({E1, G1, CASTLE_KINGSIDE});
                    }
                }
            }
            if (getBit(board.castlingRights, 1)) {
                if ((board.occupiedAll & betweenQS) == 0) {
                    if (!board.isSquareAttacked(E1, them)
                        && !board.isSquareAttacked(D1, them)
                        && !board.isSquareAttacked(C1, them)) {
                            list.add({E1, C1, CASTLE_QUEENSIDE});
                    }
                }
            }
        } 
        // FLAGS per BLACK
        else {
            if (getBit(board.castlingRights, 2)) {
                if ((board.occupiedAll & betweenKS) == 0) {
                    if (!board.isSquareAttacked(E8, us)           // casa di partenza non sotto scacco
                        && !board.isSquareAttacked(F8, us)        // casa di passaggio non sotto scacco
                        && !board.isSquareAttacked(G8, us)) {     // casa di arrivo non sotto scacco
                            list.add({E8, G8, CASTLE_KINGSIDE});
                    }
                }
            }
            if (getBit(board.castlingRights, 3)) {
                if ((board.occupiedAll & betweenQS) == 0) {
                    if (!board.isSquareAttacked(E8, us)
                        && !board.isSquareAttacked(D8, us)
                        && !board.isSquareAttacked(C8, us)) {
                            list.add({E8, C8, CASTLE_QUEENSIDE});
                    }
                }
            }
        }
        
        uint64_t attacks = kingAttacks[from];
        attacks &= ~board.occupiedByColor[us];
        while (attacks) {
            int to = popLsb(attacks);
            Move move {from, to, (getBit(board.occupiedByColor[them], to)) ? CAPTURE : QUIET};
            list.add(move);
        }

    }
}

void generatePseudoLegalMoves(Board &board, MoveList &list);
void generateLegalMoves(Board &board, MoveList &list);