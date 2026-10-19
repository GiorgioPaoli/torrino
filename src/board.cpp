#include <iostream>
#include "board.h"
#include "attacks.h"
#include "zobrist.h"

// chiama dentro di se updateOccupancy e syncMailboxFromBitboards
void Board::initBoard(){
    // azzero tutto 
    for (int c = 0; c < COLOR_NB; c++){
        for (int pt = 0; pt < PIECE_TYPE_NB; pt++){
            pieceBB[c][pt] = 0ULL;
        }
    }
    
    // pedoni
    pieceBB[WHITE][PAWN] = RANK_2;
    pieceBB[BLACK][PAWN] = RANK_7;
    
    // cavalli
    pieceBB[WHITE][KNIGHT] = SQ_BB(B1) | SQ_BB(G1);
    pieceBB[BLACK][KNIGHT] = SQ_BB(B8) | SQ_BB(G8);
    
    // alfieri
    pieceBB[WHITE][BISHOP] = SQ_BB(C1) | SQ_BB(F1);
    pieceBB[BLACK][BISHOP] = SQ_BB(C8) | SQ_BB(F8);
    
    // torri
    pieceBB[WHITE][ROOK] = SQ_BB(A1) | SQ_BB(H1);
    pieceBB[BLACK][ROOK] = SQ_BB(A8) | SQ_BB(H8);
    
    // regine
    pieceBB[WHITE][QUEEN] = SQ_BB(D1);
    pieceBB[BLACK][QUEEN] = SQ_BB(D8);
 
    // re
    pieceBB[WHITE][KING] = SQ_BB(E1);
    pieceBB[BLACK][KING] = SQ_BB(E8);
 
    updateOccupancy();
    syncMailboxFromBitboards();

    sideToMove = WHITE;
    enPassantSquare = -1;
    castlingRights = 0b1111;
    halfMoveClock = 0;
    hash = computeHash();
}

void Board::clearBoard(){
    for (int c = 0; c < COLOR_NB; c++){
        for (int pt = 0; pt < PIECE_TYPE_NB; pt++){
            pieceBB[c][pt] = 0ULL;
        }
        occupiedByColor[c] = 0ULL;
    }
    occupiedAll = 0ULL;

    for (int sq = 0; sq < 64; sq++) {
        board[sq] = NO_PIECE;
    }

    sideToMove = WHITE;
    enPassantSquare = -1;
    castlingRights = 0;
    halfMoveClock = 0;
}

void Board::updateOccupancy(){
    occupiedByColor[WHITE] = 0ULL;
    occupiedByColor[BLACK] = 0ULL;
    for (int pt = 0; pt < PIECE_TYPE_NB; pt++){
        occupiedByColor[WHITE] |= pieceBB[WHITE][pt];
        occupiedByColor[BLACK] |= pieceBB[BLACK][pt];
    }
    occupiedAll = occupiedByColor[WHITE] | occupiedByColor[BLACK];
}

void Board::syncMailboxFromBitboards(){
    for (int sq = 0; sq < 64; sq++) board[sq] = NO_PIECE;

    for (int c = 0; c < COLOR_NB; c++) {
        for (int pt = 0; pt < PIECE_TYPE_NB; pt++) {
            uint64_t bb = pieceBB[c][pt];
            while (bb) {
                int sq = __builtin_ctzll(bb);
                board[sq] = makePiece((Color)c, (PieceType)pt);
                bb &= bb - 1;
            }
        }
    }
}

// --- MOVE FUNCTIONS ---

/*  
    non fa updateOccupancy e syncMailBoxFromBitboards perche sarebbero troppo lente
    aggiorna direttamente le singole BB con XOR
    Gestisce tutti i casi per ogni flag 
    
    Salva tutte le informazioni in &undo, in modo che in unmakeMove si puo ripristinare tutto,
    comprese tutte le flag e qualsiasi cattura / enPassant / promozione ecc
*/
void Board::makeMove(Move m, UndoInfo &undo){
    int from = m.fromSquare;
    int to = m.toSquare;
    int flags = m.flags;
    
    Piece movingPiece = board[from];
    Color us = sideToMove;
    Color them = (us == WHITE) ? BLACK : WHITE;
    
    PieceType pt = pieceTypeOf(movingPiece);

    undo.capturedPiece = board[to];
    undo.castlingRights = castlingRights;
    undo.enPassantSquare = enPassantSquare;
    undo.halfMoveClock = halfMoveClock;
    undo.hash = hash;

    // aggiorno la nostra BB
    uint64_t fromTo = SQ_BB(from) | SQ_BB(to);
    pieceBB[us][pt] ^= fromTo; // spegne il bit di from e accende quello in to
    // XOR for Zobrist hashing, removing the pieces that moved from the initial square
    hash ^= pieceKeys[movingPiece][from];
    // adding the movingPiece
    hash ^= pieceKeys[movingPiece][to];

    // aggiorno la BB avversaria -> CATTURA
    if (undo.capturedPiece != NO_PIECE){
        PieceType capturedPieceType = pieceTypeOf(undo.capturedPiece);
        pieceBB[them][capturedPieceType] ^= SQ_BB(to); // spegne il bit del pezzo catturato
        hash ^= pieceKeys[undo.capturedPiece][to];
    }

    // aggiorna mailBox
    board[from] = NO_PIECE;
    board[to] = movingPiece;
    
    // aggiorno la occupancy
    occupiedByColor[us] ^= SQ_BB(from) | SQ_BB(to);
    if (undo.capturedPiece != NO_PIECE){
        occupiedByColor[them] ^= SQ_BB(to);
    } 
    occupiedAll = occupiedByColor[WHITE] | occupiedByColor[BLACK];
    
    // FLAGS
    enPassantSquare = -1;
    
    // gestione DOUBLE_PAWN_PUSH
    if (flags == DOUBLE_PAWN_PUSH){
        enPassantSquare = (from + to) / 2;
    }
    
    switch (flags) {
        case EN_PASSANT: {
            /*  
                il pedone catturato NON è su to, è sulla stessa colonna di to,
                e su rank di from, la cattura avviene in diagonale.
                Il pedone va tolto e va spostato il pezzo da from a to
            */
            int capturedSq = (us == WHITE) ? (to - 8) : (to + 8);
            pieceBB[them][PAWN] ^= SQ_BB(capturedSq);
            hash ^= pieceKeys[makePiece(them, PAWN)][capturedSq];
            board[capturedSq] = NO_PIECE;
            occupiedByColor[them] ^= SQ_BB(capturedSq);
            occupiedAll = occupiedByColor[WHITE] | occupiedByColor[BLACK];
            break;
        }
        case CASTLE_KINGSIDE: {
            // torre da h1 -> f1, il re viene gia spostato
            int rookFrom = (us == WHITE) ? H1 : H8;
            int rookTo = (us == WHITE) ? F1 : F8;
            pieceBB[us][ROOK] ^= SQ_BB(rookFrom) | SQ_BB(rookTo); 
            hash ^= pieceKeys[makePiece(us, ROOK)][rookFrom];
            hash ^= pieceKeys[makePiece(us, ROOK)][rookTo];
            board[rookFrom] = NO_PIECE;
            board[rookTo] = makePiece(us, ROOK);
            occupiedByColor[us] ^= SQ_BB(rookFrom) | SQ_BB(rookTo);
            occupiedAll = occupiedByColor[WHITE] | occupiedByColor[BLACK];
            break;
        }
        case CASTLE_QUEENSIDE: {
            // torre da a1 -> d1, il re viene gia spostato
            int rookFrom = (us == WHITE) ? A1 : A8;
            int rookTo = (us == WHITE) ? D1 : D8;
            pieceBB[us][ROOK] ^= SQ_BB(rookFrom) | SQ_BB(rookTo);
            hash ^= pieceKeys[makePiece(us, ROOK)][rookFrom];
            hash ^= pieceKeys[makePiece(us, ROOK)][rookTo];
            board[rookFrom] = NO_PIECE;
            board[rookTo] = makePiece(us, ROOK);
            occupiedByColor[us] ^= SQ_BB(rookFrom) | SQ_BB(rookTo);
            occupiedAll = occupiedByColor[WHITE] | occupiedByColor[BLACK];
            break;
        }
        case PROMOTION_QUEEN: case PROMOTION_QUEEN_CAPTURE: {
            // cambia il pezzo su to da pedone a donna
            pieceBB[us][PAWN] ^= SQ_BB(to);
            pieceBB[us][QUEEN] ^= SQ_BB(to);
            hash ^= pieceKeys[movingPiece][to];
            hash ^= pieceKeys[makePiece(us, QUEEN)][to];
            board[to] = makePiece(us, QUEEN);
            break;
        }
        case PROMOTION_ROOK: case PROMOTION_ROOK_CAPTURE: {
            // cambia il pezzo su to da pedone a torre
            pieceBB[us][PAWN] ^= SQ_BB(to);
            pieceBB[us][ROOK] ^= SQ_BB(to);
            hash ^= pieceKeys[movingPiece][to];
            hash ^= pieceKeys[makePiece(us, ROOK)][to];
            board[to] = makePiece(us, ROOK);
            break;
        }
        case PROMOTION_BISHOP: case PROMOTION_BISHOP_CAPTURE: {
            // cambia il pezzo su to da pedone ad alfiere
            pieceBB[us][PAWN] ^= SQ_BB(to);
            pieceBB[us][BISHOP] ^= SQ_BB(to);
            hash ^= pieceKeys[movingPiece][to];
            hash ^= pieceKeys[makePiece(us, BISHOP)][to];
            board[to] = makePiece(us, BISHOP);
            break;
        }
        case PROMOTION_KNIGHT: case PROMOTION_KNIGHT_CAPTURE: {
            // cambia il pezzo su to da pedone a cavallo
            pieceBB[us][PAWN] ^= SQ_BB(to);
            pieceBB[us][KNIGHT] ^= SQ_BB(to);
            hash ^= pieceKeys[movingPiece][to];
            hash ^= pieceKeys[makePiece(us, KNIGHT)][to];
            board[to] = makePiece(us, KNIGHT);
            break;
        }
        default:
            break;
    }

    if (undo.capturedPiece != NO_PIECE || pt == PAWN){
        halfMoveClock = 0;
    }
    else
        halfMoveClock++;

    // aggiorna castlingRights se muove re o torre
    if (pt == KING){
        if (us == WHITE) {
            clearBitInt(castlingRights, 0);
            clearBitInt(castlingRights, 1);
        } else {
            clearBitInt(castlingRights, 2);
            clearBitInt(castlingRights, 3);
        }
    }
    if (pt == ROOK){
        if (from == H1) clearBitInt(castlingRights, 0);
        if (from == A1) clearBitInt(castlingRights, 1);
        if (from == H8) clearBitInt(castlingRights, 2);
        if (from == A8) clearBitInt(castlingRights, 3);
    }
    if (to == H1) clearBitInt(castlingRights, 0);
    if (to == A1) clearBitInt(castlingRights, 1);
    if (to == H8) clearBitInt(castlingRights, 2);
    if (to == A8) clearBitInt(castlingRights, 3);

    hash ^= castlingKeys[undo.castlingRights];
    hash ^= castlingKeys[castlingRights];
    if (undo.enPassantSquare != -1) {
        hash ^= enPassantKeys[fileOf(undo.enPassantSquare)];
    }
    if (enPassantSquare != -1) {
        hash ^= enPassantKeys[fileOf(enPassantSquare)];
    }

    sideToMove = them;

    hash ^= sideKey;
}

void Board::unmakeMove(Move m, const UndoInfo &undo){
    // restore dei campi semplici
    int to = m.toSquare;
    int from = m.fromSquare;
    int flags = m.flags;

    Piece movingPiece = board[to];
    Color us = (sideToMove == BLACK) ? WHITE : BLACK;
    Color them = (us == WHITE) ? BLACK : WHITE;

    PieceType pt = pieceTypeOf(movingPiece);

    Piece capturedPiece = undo.capturedPiece;     
    castlingRights = undo.castlingRights;
    enPassantSquare = undo.enPassantSquare;
    halfMoveClock = undo.halfMoveClock;
    hash = undo.hash;
    sideToMove = us;

    // aggiorna la loro BB
    uint64_t fromTo = SQ_BB(from) | SQ_BB(to);

    bool isPromotion = (flags >= PROMOTION_QUEEN && flags <= PROMOTION_KNIGHT_CAPTURE);
    if (!isPromotion) {
        pieceBB[us][pt] ^= fromTo;
    }

    board[to] = NO_PIECE;
    board[from] = movingPiece;

    // restore nostra BB -> CATTURA
    if (capturedPiece != NO_PIECE){
        PieceType capturedPieceType = pieceTypeOf(capturedPiece);
        pieceBB[them][capturedPieceType] ^= SQ_BB(m.toSquare); 
        board[to] = capturedPiece;
    }

    // restore Occupancy
    occupiedByColor[us] ^= SQ_BB(to) | SQ_BB(from);
    if (capturedPiece != NO_PIECE){
        occupiedByColor[them] ^= SQ_BB(to);
    }
    occupiedAll = occupiedByColor[WHITE] | occupiedByColor[BLACK];


    switch (flags) {
        case EN_PASSANT: {
            int capturedSq = (us == WHITE) ? (to - 8) : (to + 8);
            pieceBB[them][PAWN] ^= SQ_BB(capturedSq);
            board[capturedSq] = makePiece(them, PAWN);
            occupiedByColor[them] ^= SQ_BB(capturedSq);
            occupiedAll = occupiedByColor[WHITE] | occupiedByColor[BLACK];
            break;
        }
        case CASTLE_KINGSIDE: {
            int rookFrom = (us == WHITE) ? H1 : H8;
            int rookTo = (us == WHITE) ? F1 : F8;
            pieceBB[us][ROOK] ^= SQ_BB(rookFrom) | SQ_BB(rookTo); 
            board[rookTo] = NO_PIECE;
            board[rookFrom] = makePiece(us, ROOK);
            occupiedByColor[us] ^= SQ_BB(rookFrom) | SQ_BB(rookTo);
            occupiedAll = occupiedByColor[WHITE] | occupiedByColor[BLACK];
            break;
        }
        case CASTLE_QUEENSIDE: {
            int rookFrom = (us == WHITE) ? A1 : A8;
            int rookTo = (us == WHITE) ? D1 : D8;
            pieceBB[us][ROOK] ^= SQ_BB(rookFrom) | SQ_BB(rookTo);
            board[rookTo] = NO_PIECE;
            board[rookFrom] = makePiece(us, ROOK);
            occupiedByColor[us] ^= SQ_BB(rookFrom) | SQ_BB(rookTo);
            occupiedAll = occupiedByColor[WHITE] | occupiedByColor[BLACK];
            break;
        }
        case PROMOTION_QUEEN: case PROMOTION_QUEEN_CAPTURE: {
            PieceType promotedType = QUEEN;
            pieceBB[us][promotedType] ^= SQ_BB(to);
            pieceBB[us][PAWN] ^= SQ_BB(from);
            board[from] = makePiece(us, PAWN);
            break;
        }
        case PROMOTION_ROOK: case PROMOTION_ROOK_CAPTURE: {
            PieceType promotedType = ROOK;
            pieceBB[us][promotedType] ^= SQ_BB(to);
            pieceBB[us][PAWN] ^= SQ_BB(from);
            board[from] = makePiece(us, PAWN);
            break;
        }
        case PROMOTION_BISHOP: case PROMOTION_BISHOP_CAPTURE: {
            PieceType promotedType = BISHOP;
            pieceBB[us][promotedType] ^= SQ_BB(to);
            pieceBB[us][PAWN] ^= SQ_BB(from);
            board[from] = makePiece(us, PAWN);
            break;
        }
        case PROMOTION_KNIGHT: case PROMOTION_KNIGHT_CAPTURE: {
            PieceType promotedType = KNIGHT;
            pieceBB[us][promotedType] ^= SQ_BB(to);
            pieceBB[us][PAWN] ^= SQ_BB(from);
            board[from] = makePiece(us, PAWN);
            break;
        }
        default: break;
    }


}

uint64_t Board::computeHash() {
    uint64_t key = 0;
    for (int sq = 0; sq < 64; sq++) {
        if (board[sq] != NO_PIECE) {
            key ^= pieceKeys[board[sq]][sq];
        }
    }

    if (sideToMove == BLACK) key ^= sideKey;
    key ^= castlingKeys[castlingRights];
    if (enPassantSquare != -1) key ^= enPassantKeys[fileOf(enPassantSquare)];

    return key;
}

bool Board::isSquareAttacked(int square, Color bySide){
    Color opp = opposite(bySide);
    if(pawnAttacks[opp][square] & pieceBB[bySide][PAWN]) return true;
    if(knightAttacks[square] & pieceBB[bySide][KNIGHT]) return true;
    if(kingAttacks[square] & pieceBB[bySide][KING]) return true;
    if(bishopAttacksSlow(square, occupiedAll) & (pieceBB[bySide][BISHOP] | pieceBB[bySide][QUEEN])) return true;
    if(rookAttacksSlow(square, occupiedAll) & (pieceBB[bySide][ROOK] | pieceBB[bySide][QUEEN])) return true;
    return false;
}

bool Board::isInCheck(Color us){
    int kingSq = getLsbIndex(pieceBB[us][KING]);
    return isSquareAttacked(kingSq, opposite(us));
}



// --- HELPER FUNCTIONS ---
void Board::printBoard(){
    const char* pieceChars = "PNBRQKpnbrqk.";  // maiuscolo bianco, minuscolo nero, '.' vuoto
    
    std::cout << '\n';
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << "  +---+---+---+---+---+---+---+---+\n";
        std::cout << (rank + 1) << " ";
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            char c = ((board[sq] == NO_PIECE) ? ' ' : pieceChars[board[sq]]);
            std::cout << "| " << c << " ";
        }
        std::cout << "|\n";
    }
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
    std::cout << "    a   b   c   d   e   f   g   h\n\n";

}

void Board::printBoardFromBB(uint64_t bb){
    std::cout << '\n';
    for (int rank = 7; rank >= 0; rank--){
        std::cout << (rank + 1) << "  ";
        for (int file = 0; file < 8; file++){
            int sq = rank * 8 + file;
            std::cout << (((bb >> sq) & 1ULL) ? 'x' : '.') << ' '; 
        }
        std::cout << '\n';
    }
    std::cout << "\n   a b c d e f g h\n";
}

// --- FEN FUNCTIONS ---
void Board::loadFEN(const std::string &fen){
    clearBoard();

    std::istringstream iss(fen);
    std::string placement, activeColor, castling, epSquare;
    int halfmove, fullmove;
    iss >> placement >> activeColor >> castling >> epSquare >> halfmove >> fullmove;
    
    // scrive tutto nella board, non c'è bisogno di chiamare occupancy e syncMailBox
    int rank = 7, file = 0;
    for (char c : placement){
        if (c == '/') {
            rank--;
            file = 0;
        }
        else if (isdigit(c)){
            file += c - '0';
        }
        else {
            int sq = rank * 8 + file;
            Color color = (isupper(c)) ? WHITE : BLACK;
            PieceType pt;

            switch (tolower(c)) {
                case 'p': pt = PAWN;    break;
                case 'n': pt = KNIGHT;  break;
                case 'b': pt = BISHOP;  break;
                case 'r': pt = ROOK;    break; 
                case 'q': pt = QUEEN;   break;
                case 'k': pt = KING;    break;
            }

            setBit(pieceBB[color][pt], sq);
            board[sq] = makePiece(color, pt);

            file++;
        }
    }

    // side to move
    sideToMove = (activeColor == "w") ? WHITE : BLACK;

    castlingRights = 0;
    for (char c : castling) {
        switch (c) {
            case 'K': setBitInt(castlingRights, 0); break;
            case 'Q': setBitInt(castlingRights, 1); break;
            case 'k': setBitInt(castlingRights, 2); break;
            case 'q': setBitInt(castlingRights, 3); break;
        }
    }

    //enPassant
    if (epSquare == "-") {
        enPassantSquare = -1;
    } else {
        int file = epSquare[0] - 'a';
        int rank = epSquare[1] - '1';
        enPassantSquare = rank * 8 + file;
    }

    halfMoveClock = halfmove;
    
    updateOccupancy();
    
    hash = computeHash();
}



