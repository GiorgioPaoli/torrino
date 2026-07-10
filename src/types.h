#pragma once
#include <cstdint>

// gli enum sono semplicemente un modo di assegnare ad un numero un nome
// nel nostro caso WHITE è 0, BLACK 1 ecc.
// in PieceType per esempio BISHOP è 2 ... PIECE_TYPE_NB è il totale, quindi 6

enum Color {WHITE, BLACK, COLOR_NB};                            // traduce in 0, 1, 2

enum PieceType {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    PIECE_TYPE_NB
};

enum Piece {
    W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
    NO_PIECE, PIECE_NB = 12
};


// a1 è 0 ... h8 è 63, SQUARE_NB 64 = totale
enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    SQUARE_NB
};


constexpr uint64_t RANK_1 = 0x00000000000000FFULL;
constexpr uint64_t RANK_2 = 0x000000000000FF00ULL;
constexpr uint64_t RANK_3 = 0x0000000000FF0000ULL;
constexpr uint64_t RANK_4 = 0x00000000FF000000ULL;
constexpr uint64_t RANK_5 = 0x000000FF00000000ULL;
constexpr uint64_t RANK_6 = 0x0000FF0000000000ULL;
constexpr uint64_t RANK_7 = 0x00FF000000000000ULL;
constexpr uint64_t RANK_8 = 0xFF00000000000000ULL;

constexpr uint64_t FILE_A = 0x0101010101010101ULL;
constexpr uint64_t FILE_B = 0x0202020202020202ULL;
constexpr uint64_t FILE_C = 0x0404040404040404ULL;
constexpr uint64_t FILE_D = 0x0808080808080808ULL;
constexpr uint64_t FILE_E = 0x1010101010101010ULL;
constexpr uint64_t FILE_F = 0x2020202020202020ULL;
constexpr uint64_t FILE_G = 0x4040404040404040ULL;
constexpr uint64_t FILE_H = 0x8080808080808080ULL;

constexpr uint64_t FILE_MASKS[8] = {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};

// --- FUNZIONI UTILI ---

constexpr Piece makePiece(Color c, PieceType pt) {return static_cast<Piece>(c * 6 + pt);}
constexpr PieceType pieceTypeOf(Piece p) {return static_cast<PieceType>(p % 6);}
constexpr Color colorOfPiece(Piece p) {return static_cast<Color>((p < 6) ? WHITE : BLACK);}
constexpr Color opposite(Color c) {return (c == WHITE) ? BLACK : WHITE;}

// bit corrispondente a una casa
constexpr uint64_t SQ_BB(int sq) { return 1ULL << sq; }

constexpr int fileOf(int sq) {return sq & 7;}

constexpr int rankOf(int sq) {return sq >> 3;}
