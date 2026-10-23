#include <cstdlib>
#include "eval.h"
#include "bitboard.h"
#include "search.h"
#include "attacks.h"
#include "magics.h"

// restituisce una penalita per ogni doubled pawn
int doublePawnPenalty(Board &board){
    int whitePenalty = 0;
    int blackPenalty = 0;

    for (int f = 0; f < 8; f++) {
        int whiteOnFile = popcount(board.pieceBB[WHITE][PAWN] & FILE_MASKS[f]);
        int blackOnFile = popcount(board.pieceBB[BLACK][PAWN] & FILE_MASKS[f]);

        if (whiteOnFile > 1) whitePenalty += (whiteOnFile - 1) * DOUBLED_PAWN_PENALY;
        if (blackOnFile > 1) blackPenalty += (blackOnFile - 1) * DOUBLED_PAWN_PENALY;
    }

    return blackPenalty - whitePenalty;
}

uint64_t passedMask[COLOR_NB][64];
uint64_t adjacentFilesMask[8];

// builds the pawn structure masks, called once at startup
void initPawnMasks() {
    for (int f = 0; f < 8; f++) {
        adjacentFilesMask[f] = 0ULL;
        if (f > 0) adjacentFilesMask[f] |= FILE_MASKS[f - 1];
        if (f < 7) adjacentFilesMask[f] |= FILE_MASKS[f + 1];
    }

    for (int sq = 0; sq < 64; sq++) {
        int file = fileOf(sq);
        int rank = rankOf(sq);
        uint64_t files = FILE_MASKS[file] | adjacentFilesMask[file];

        // ranks in front of the pawn: above it for white, below it for black
        uint64_t aheadWhite = 0ULL;
        uint64_t aheadBlack = 0ULL;
        for (int r = rank + 1; r < 8; r++)  aheadWhite |= 0xFFULL << (8 * r);
        for (int r = rank - 1; r >= 0; r--) aheadBlack |= 0xFFULL << (8 * r);

        passedMask[WHITE][sq] = files & aheadWhite;
        passedMask[BLACK][sq] = files & aheadBlack;
    }
}

// passed, isolated, defended and doubled pawns; adds (white - black) to both mg and eg
void pawnStructure(Board &board, int &mg, int &eg) {
    uint64_t whitePawns = board.pieceBB[WHITE][PAWN];
    uint64_t blackPawns = board.pieceBB[BLACK][PAWN];

    // white pawns
    uint64_t bb = whitePawns;
    while (bb) {
        int sq = popLsb(bb);
        int relRank = rankOf(sq);                                   // 1 = starting rank ... 6 = 7th rank
        if ((passedMask[WHITE][sq] & blackPawns) == 0) {            // no black pawn can stop it
            mg += PASSED_PAWN_MG[relRank];
            eg += PASSED_PAWN_EG[relRank];
        }
        if ((adjacentFilesMask[fileOf(sq)] & whitePawns) == 0) {    // no white pawn on the adjacent files
            mg -= ISOLATED_PAWN_MG;
            eg -= ISOLATED_PAWN_EG;
        }
    }

    // black pawns: same logic, mirrored ranks and opposite signs
    bb = blackPawns;
    while (bb) {
        int sq = popLsb(bb);
        int relRank = 7 - rankOf(sq);
        if ((passedMask[BLACK][sq] & whitePawns) == 0) {
            mg -= PASSED_PAWN_MG[relRank];
            eg -= PASSED_PAWN_EG[relRank];
        }
        if ((adjacentFilesMask[fileOf(sq)] & blackPawns) == 0) {
            mg += ISOLATED_PAWN_MG;
            eg += ISOLATED_PAWN_EG;
        }
    }

    // defended pawns (pawn chains): pawns standing on a square attacked by a friendly pawn
    int whiteDefended = popcount(whitePawns & (shiftNorthEast(whitePawns) | shiftNorthWest(whitePawns)));
    int blackDefended = popcount(blackPawns & (shiftSouthEast(blackPawns) | shiftSouthWest(blackPawns)));
    mg += (whiteDefended - blackDefended) * DEFENDED_PAWN_MG;
    eg += (whiteDefended - blackDefended) * DEFENDED_PAWN_EG;

    // doubled pawns (already white - black)
    int doubled = doublePawnPenalty(board);
    mg += doubled;
    eg += doubled;
}


/*
    VERY IMPORTANT

    evaluate() gives a relative score to the side TO MOVE

    So > 0 is always relative to the side who's turn is to move, even if it's black to move.
    > 0 is a winning evaluation, < 0 is losing.
*/

// gives a total score based on which and how many pieces are left on the board, doesn't matter which side
int countPieces(Board &board){
    int knightValue = 1;
    int bishopValue = 1;
    int rookValue = 2;
    int queenValue = 4;
    int totalScore = 0;
    
    totalScore += popcount(board.pieceBB[WHITE][KNIGHT] | board.pieceBB[BLACK][KNIGHT])  * knightValue;
    totalScore += popcount(board.pieceBB[WHITE][BISHOP] | board.pieceBB[BLACK][BISHOP])  * bishopValue;
    totalScore += popcount(board.pieceBB[WHITE][ROOK]   | board.pieceBB[BLACK][ROOK])    * rookValue;
    totalScore += popcount(board.pieceBB[WHITE][QUEEN]  | board.pieceBB[BLACK][QUEEN])   * queenValue;

    return totalScore;
}

int centerDistance(int sq){
    int file = fileOf(sq);
    int rank = rankOf(sq);
    int fileDist = max(3 - file, file - 4);
    int rankDist = max(3 - rank, rank - 4);
    return fileDist + rankDist;
}

int kingDistance(int sq1, int sq2){
    int fileDiff = abs(fileOf(sq1) - fileOf(sq2));
    int rankDiff = abs(rankOf(sq1) - rankOf(sq2));
    return fileDiff + rankDiff;
}

// castling score for ONE side: bonus while it can still castle, penalty if the king moved without castling
static int castlingScore(Board &board, Color side, int kingSq) {
    int startSq = (side == WHITE) ? E1 : E8;

    if (kingSq == startSq) {
        // castling rights: bits 0-1 are white's (K, Q), bits 2-3 are black's (k, q)
        int firstBit = (side == WHITE) ? 0 : 2;
        int rights = getBitInt(board.castlingRights, firstBit) + getBitInt(board.castlingRights, firstBit + 1);
        return rights * CASTLING_RIGHT_BONUS;
    }

    // castled king (g/c) or one step further into the corner (h/b): safe, no penalty
    bool castled = (side == WHITE)
        ? (kingSq == G1 || kingSq == H1 || kingSq == C1 || kingSq == B1)
        : (kingSq == G8 || kingSq == H8 || kingSq == C8 || kingSq == B8);
    if (castled) return 0;

    // anywhere else (f1, d1, or off the back rank): the king moved and castling is lost
    return -KING_MOVED_PENALTY;
}

// white minus black
int castlingSafety(Board &board, int sqw, int sqb) {
    return castlingScore(board, WHITE, sqw) - castlingScore(board, BLACK, sqb);
}


// pawn shield in front of a king castled on a wing, for ONE side
int pawnShield(Board &board, Color side, int kingSq) {
    int homeRank = (side == WHITE) ? 0 : 7;
    if (rankOf(kingSq) != homeRank) return 0;            // only for a king on its back rank

    int file = fileOf(kingSq);
    int firstFile;
    if (file >= 5)      firstFile = 5;                    // kingside: files f, g, h
    else if (file <= 2) firstFile = 0;                    // queenside: files a, b, c
    else return 0;                                        // king in the centre: handled by castlingSafety

    uint64_t pawns    = board.pieceBB[side][PAWN];
    uint64_t rankNear = (side == WHITE) ? RANK_2 : RANK_7;  // pawn still on its starting square
    uint64_t rankFar  = (side == WHITE) ? RANK_3 : RANK_6;  // pawn pushed by one square

    int score = 0;
    for (int f = firstFile; f < firstFile + 3; f++) {
        if (pawns & FILE_MASKS[f] & rankNear)      score += SHIELD_RANK2_BONUS;
        else if (pawns & FILE_MASKS[f] & rankFar)  score += SHIELD_RANK3_BONUS;
        else                                       score -= SHIELD_MISSING_PENALTY;
    }
    return score;
}

// penalty for a king that walked up the board while the enemy queen is still alive, for ONE side
int exposedKing(Board &board, Color side, int kingSq) {
    Color them = opposite(side);
    if (board.pieceBB[them][QUEEN] == 0) return 0;

    // how many ranks the king has advanced from its back rank (0 = back rank)
    int advanced = (side == WHITE) ? rankOf(kingSq) : 7 - rankOf(kingSq);
    if (advanced <= 1) return 0;                          // 1st and 2nd rank are fine

    return -EXPOSED_KING_PER_RANK * (advanced - 1);
}

// enemy pieces attacking the squares around the king of `side`; grows quadratically with the attack
int kingAttack(Board &board, Color side, int kingSq) {
    Color them = opposite(side);

    uint64_t enemyPieces = board.pieceBB[them][KNIGHT] | board.pieceBB[them][BISHOP]
                         | board.pieceBB[them][ROOK]   | board.pieceBB[them][QUEEN];
    if (popcount(enemyPieces) < 2) return 0;

    uint64_t zone = kingAttacks[kingSq];                  // the 8 squares around the king
    uint64_t occ  = board.occupiedAll;

    int attackerCount = 0;
    int attackWeight  = 0;

    uint64_t bb = board.pieceBB[them][KNIGHT];
    while (bb) {
        int sq = popLsb(bb);
        if (knightAttacks[sq] & zone) { attackerCount++; attackWeight += KNIGHT_ATTACK_WEIGHT; }
    }
    // sliders: cheap empty-board check first, slow exact check only if the piece is lined up with the zone
    bb = board.pieceBB[them][BISHOP];
    while (bb) {
        // first checks the EMPTY attacks, if the piece can attack the king, then it computes the function
        int sq = popLsb(bb);
        if ((bishopEmptyAttacks[sq] & zone) && (bishopAttacks(sq, occ) & zone)) {
            attackerCount++; attackWeight += BISHOP_ATTACK_WEIGHT;
        }
    }
    bb = board.pieceBB[them][ROOK];
    while (bb) {
        int sq = popLsb(bb);
        if ((rookEmptyAttacks[sq] & zone) && (rookAttacks(sq, occ) & zone)) {
            attackerCount++; attackWeight += ROOK_ATTACK_WEIGHT;
        }
    }
    bb = board.pieceBB[them][QUEEN];
    while (bb) {
        int sq = popLsb(bb);
        if ((queenEmptyAttacks[sq] & zone) && (queenAttacks(sq, occ) & zone)) {
            attackerCount++; attackWeight += QUEEN_ATTACK_WEIGHT;
        }
    }

    // a single attacker is rarely dangerous; with 2+ the danger grows quickly
    if (attackerCount < 2) return 0;
    return -(attackWeight * attackWeight);
}




int evaluate(Board &board) {
    int mg = 0; // middlegame
    int eg = 0; // endgame
    int phase = 0;
    int piecesNotDevelopedWhite = 0;
    int piecesNotDevelopedBlack= 0;
    int whiteMaterial = 0;
    int blackMaterial = 0;

    phase = countPieces(board);
    if (phase > 24) phase = 24;

    // pawn structure: passed, isolated, defended and doubled pawns
    pawnStructure(board, mg, eg);
    
    // penalty for non developed pieces
    if (board.board[B1] == W_KNIGHT) {mg -= 30; piecesNotDevelopedWhite++;}
    if (board.board[F1] == W_BISHOP) {mg -= 30; piecesNotDevelopedWhite++;}
    if (board.board[C1] == W_BISHOP) {mg -= 30; piecesNotDevelopedWhite++;}
    if (board.board[G1] == W_KNIGHT) {mg -= 30; piecesNotDevelopedWhite++;}
    
    if (board.board[B8] == B_KNIGHT) {mg += 30; piecesNotDevelopedBlack++;}
    if (board.board[F8] == B_BISHOP) {mg += 30; piecesNotDevelopedBlack++;}
    if (board.board[C8] == B_BISHOP) {mg += 30; piecesNotDevelopedBlack++;}
    if (board.board[G8] == B_KNIGHT) {mg += 30; piecesNotDevelopedBlack++;}
    
    // penalty for moving queen too early
    if (piecesNotDevelopedWhite > 1) {
        if (popcount(board.pieceBB[WHITE][QUEEN]) != 0) {
            mg -= (board.board[D1] == W_QUEEN) ? 0 : 80; 
        } 
    }
    if (piecesNotDevelopedBlack > 1) {
        if (popcount(board.pieceBB[BLACK][QUEEN]) != 0) {
            mg += (board.board[D8] == B_QUEEN) ? 0 : 80;
        }
    }
    
    // counts pieces and assigns a score based on the PST, EXCEPT THE KING
    for (int pt = 0; pt < KING; pt++) {
        uint64_t wbb = board.pieceBB[WHITE][pt];
        uint64_t bbb = board.pieceBB[BLACK][pt];
        while (wbb) {
            int sq = popLsb(wbb);
            mg += PIECE_WEIGHTS[pt] + PST[pt][sq ^ 56];  // IMPORTANT, this flips the PST
            eg += PIECE_WEIGHTS[pt] + PST[pt][sq ^ 56];
            if (pt != PAWN) whiteMaterial += PIECE_WEIGHTS[pt];
        } 
        while (bbb) {
            int sq = popLsb(bbb);
            mg -= PIECE_WEIGHTS[pt] + PST[pt][sq];
            eg -= PIECE_WEIGHTS[pt] + PST[pt][sq];
            if (pt != PAWN) blackMaterial += PIECE_WEIGHTS[pt];
        }
    }
    
    // tapered evaluation for the king
    uint64_t wk = board.pieceBB[WHITE][KING];
    uint64_t bk = board.pieceBB[BLACK][KING];
    int sqw = popLsb(wk);
    int sqb = popLsb(bk);
    mg += MG_KING_PST[sqw ^ 56] - MG_KING_PST[sqb];
    eg += EG_KING_PST[sqw ^ 56] - EG_KING_PST[sqb];
    // Mop-up, pushing the enemy king to the corner based on piece material advantage
    int materialImbalance = whiteMaterial - blackMaterial;
    int bonus = 0;
    if (materialImbalance >= 400) {
        // centerDistance(losingKing) ... kingDistance(winningKing, losingKing)
        bonus = 10 * centerDistance(sqb) + 4 * (14 - kingDistance(sqw, sqb));
        eg += bonus;
    } else if (materialImbalance <= -400) {
        bonus = 10 * centerDistance(sqw) + 4 * (14 - kingDistance(sqb, sqw));
        eg -= bonus;
    } 
    
    // king safety: middlegame only, it fades out automatically as pieces come off
    if (phase >= KING_SAFETY_MIN_PHASE) {
        mg += castlingSafety(board, sqw, sqb);
        mg += pawnShield(board, WHITE, sqw)  - pawnShield(board, BLACK, sqb);
        mg += exposedKing(board, WHITE, sqw) - exposedKing(board, BLACK, sqb);
        mg += kingAttack(board, WHITE, sqw)  - kingAttack(board, BLACK, sqb);
    }

    int score = (mg * phase + eg * (24 - phase)) / 24;

    return (board.sideToMove == WHITE) ? score : -score;
}   
