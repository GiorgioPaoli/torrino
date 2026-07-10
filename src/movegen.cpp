#include "movegen.h"
#include "attacks.h"

// ORDINE IMPORTANTE !!!! Aggiunge prima la promozione di regina
void generatePawnMoves(Board &board, MoveList &list, Color us){
    // deve gestire EnPassant e Promozioni
    Color them = opposite(us);
    uint64_t pawns = board.pieceBB[us][PAWN];

    int pushDir = (us == WHITE) ? +8 : -8;
    int startRank = (us == WHITE) ? 1 : 6;
    int promotionRank = (us == WHITE) ? 7 : 0;

    while (pawns) {
        int from = popLsb(pawns);
        int to = from + pushDir;
        if (!getBit(board.occupiedAll, to)) {
            if (rankOf(to) == promotionRank) {
                list.add({from, to, PROMOTION_QUEEN});
                list.add({from, to, PROMOTION_ROOK});
                list.add({from, to, PROMOTION_KNIGHT});
                list.add({from, to, PROMOTION_BISHOP});
            } else {
                list.add({from, to, QUIET});
            }

            if (rankOf(from) == startRank) {
                int to2 = from + 2*pushDir;
                if(!getBit(board.occupiedAll, to2)) {
                    list.add({from, to2, DOUBLE_PAWN_PUSH});
                }
            }
        }
        // per catture
        uint64_t captures = pawnAttacks[us][from] & board.occupiedByColor[them];
        while (captures) {
            int to = popLsb(captures);
            if (rankOf(to) == promotionRank) {
                list.add({from, to, PROMOTION_QUEEN_CAPTURE});
                list.add({from, to, PROMOTION_ROOK_CAPTURE});
                list.add({from, to, PROMOTION_BISHOP_CAPTURE});
                list.add({from, to, PROMOTION_KNIGHT_CAPTURE});
            } else {
                list.add({from, to, CAPTURE});
            }
        }
        
        // enPassant
        if (board.enPassantSquare != -1 && getBit(pawnAttacks[us][from], board.enPassantSquare)) {
            list.add({from, board.enPassantSquare, EN_PASSANT});
        }
    }
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

        // BB delle case tra il re e le torri
        uint64_t betweenKS = (us == WHITE) ? (SQ_BB(F1) | SQ_BB(G1)) : (SQ_BB(F8) | SQ_BB(G8));
        uint64_t betweenQS = (us == WHITE) ? (SQ_BB(D1) | SQ_BB(C1) | SQ_BB(B1)) : (SQ_BB(D8) | SQ_BB(C8) | SQ_BB(B8));

        /*
            Mosse per arrocchi sia bianchi che neri
            Controllo se posso fare uno specifico arrocco
            Controllo se tutte le case sono libere tra il re e la torre    
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
                    if (!board.isSquareAttacked(E8, them)           // casa di partenza non sotto scacco
                        && !board.isSquareAttacked(F8, them)        // casa di passaggio non sotto scacco
                        && !board.isSquareAttacked(G8, them)) {     // casa di arrivo non sotto scacco
                            list.add({E8, G8, CASTLE_KINGSIDE});
                    }
                }
            }
            if (getBit(board.castlingRights, 3)) {
                if ((board.occupiedAll & betweenQS) == 0) {
                    if (!board.isSquareAttacked(E8, them)
                        && !board.isSquareAttacked(D8, them)
                        && !board.isSquareAttacked(C8, them)) {
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

void generatePseudoLegalMoves(Board &board, MoveList &list){
    generatePawnMoves(board, list, board.sideToMove);
    generateKnightMoves(board, list, board.sideToMove);
    generateBishopMoves(board, list, board.sideToMove);
    generateRookMoves(board, list, board.sideToMove);
    generateQueenMoves(board, list, board.sideToMove);
    generateKingMoves(board, list, board.sideToMove);
}

void generateLegalMoves(Board &board, MoveList &list){
    MoveList pseudo;
    generatePseudoLegalMoves(board, pseudo);

    Color us = board.sideToMove;
    Color them = (us == WHITE) ? BLACK : WHITE;

    for (int i = 0; i < pseudo.count; i++){
        UndoInfo undo;
        board.makeMove(pseudo.moves[i], undo);

        // controllo se dopo la mossa fatta il re è sotto scacco, se si la scarto perché illegale
        int kingSq = getLsbIndex(board.pieceBB[us][KING]);
        if (!board.isSquareAttacked(kingSq, them)) {
            list.add(pseudo.moves[i]);
        }
        
        board.unmakeMove(pseudo.moves[i], undo);
    }
}

// MOVE ORDERING E SCORE MOVE
// da uno score alla mossa per il move-ordering
int scoreMove(Board &board, Move m){
    // catture: MVV-LVA (vittima grossa, attaccante piccolo)
    if (isCapture((Flags)m.flags)) {
        int attacker = PIECE_WEIGHTS[pieceTypeOf(board.board[m.fromSquare])];
        int victim;
        if (m.flags == EN_PASSANT) {
            victim = PIECE_WEIGHTS[PAWN];
        } else {
            victim = PIECE_WEIGHTS[pieceTypeOf(board.board[m.toSquare])];
        }

        return 10000 + victim * 10 - attacker;
    }

    if (m.flags == PROMOTION_QUEEN) {
        return 9000;
    }    

    return 0;
}

// ordina le mosse con selection sort
void orderMoves(Board &board, MoveList &list){
    for (int i = 0; i < list.count; i++) {
        int best = i;
        for (int j = i + 1; j < list.count; j++) {
            if (scoreMove(board, list.moves[j]) > scoreMove(board, list.moves[best])) {
                best = j;
            }
        }

        Move tmp = list.moves[i];
        list.moves[i] = list.moves[best];
        list.moves[best] = tmp;
    }
}




std::string moveToString(Move m){
    std::string s;
    s += char('a' + fileOf(m.fromSquare));
    s += char('1' + rankOf(m.fromSquare));
    s += char('a' + fileOf(m.toSquare));
    s += char('1' + rankOf(m.toSquare));

    switch (m.flags) {
        case PROMOTION_QUEEN: case PROMOTION_QUEEN_CAPTURE:     s += 'q'; break;
        case PROMOTION_ROOK: case PROMOTION_ROOK_CAPTURE:       s += 'r'; break;
        case PROMOTION_KNIGHT: case PROMOTION_KNIGHT_CAPTURE:   s += 'n'; break;
        case PROMOTION_BISHOP: case PROMOTION_BISHOP_CAPTURE:   s += 'b'; break;
        default: break;
    }
    return s;
}

// genera tutte le mosse possibili nella posizione, poi ne controlla una per una
// a quella passata come stringa, se è uguale restituisce la mossa
Move parseMove(Board &board, const std::string &s){
    MoveList list;
    generateLegalMoves(board, list);
    
    for (int i = 0; i < list.count; i++) {
        if (moveToString(list.moves[i]) == s) {
            return list.moves[i];
        }
    }
    return Move{-1, -1, QUIET};
}