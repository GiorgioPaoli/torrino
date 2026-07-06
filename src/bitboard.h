#pragma once
#include "types.h"

constexpr void setBit(uint64_t &b, int i) {b |= 1ULL << i;}
constexpr void setBitInt(int &b, int i) {b |= 1 << i;}
constexpr void clearBit(uint64_t &b, int i) {b &= ~(1ULL << i);}
constexpr void clearBitInt(int &b, int i) {b &= ~(1 << i);}
constexpr bool getBit(uint64_t b, int i) {return (b >> i) & 1ULL;} // ritorna 0 = false o 1 = true


// returna il numero di bit accesi in bb
constexpr int popcount(uint64_t bb){
    return __builtin_popcountll(bb);
}

// returna il bit meno significativo (quello piu a destra)
constexpr int getLsbIndex(uint64_t bb){
    return __builtin_ctzll(bb);
}

// pop bit meno significativo e restituisce l'indice de l
constexpr int popLsb(uint64_t &bb){
    int index = getLsbIndex(bb);
    bb &= bb - 1;
    return index;
}


// hanno Wrap Around integrato, controllo che non esca dalla scacchiera
constexpr uint64_t shiftNorth(uint64_t bb) {return bb << 8;}
constexpr uint64_t shiftSouth(uint64_t bb) {return bb >> 8;} 
constexpr uint64_t shiftEast(uint64_t bb) {return (bb << 1) & ~FILE_A;} // serve la maschera con FILE_A altrimenti finirebbe da A a H 
constexpr uint64_t shiftWest(uint64_t bb) {return (bb >> 1) & ~FILE_H;} // viceversa 

constexpr uint64_t shiftNorthEast(uint64_t bb) {return shiftNorth(shiftEast(bb));}
constexpr uint64_t shiftNorthWest(uint64_t bb) {return shiftNorth(shiftWest(bb));}
constexpr uint64_t shiftSouthEast(uint64_t bb) {return shiftSouth(shiftEast(bb));}
constexpr uint64_t shiftSouthWest(uint64_t bb) {return shiftSouth(shiftWest(bb));}
