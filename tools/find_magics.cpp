// Finds magic numbers for rooks and bishops and prints them as C++ arrays.
// Build (from the repo root):
//   g++ -O2 -std=c++17 -Isrc tools/find_magics.cpp src/attacks.cpp -o tools/find_magics.exe
#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include "attacks.h"

static std::mt19937_64 rng(20261023);

// random number with few set bits: much more likely to be a good magic
static uint64_t sparseRandom() {
    return rng() & rng() & rng();
}

static uint64_t findMagic(int sq, bool rook) {
    uint64_t mask = rook ? rookRelevantMask(sq) : bishopRelevantMask(sq);
    int bits = popcount(mask);
    int size = 1 << bits;

    // every blocker pattern inside the mask and the correct attacks for it
    std::vector<uint64_t> occ(size), ref(size);
    uint64_t sub = 0;
    for (int i = 0; i < size; i++) {
        occ[i] = sub;
        ref[i] = rook ? rookAttacksSlow(sq, sub) : bishopAttacksSlow(sq, sub);
        sub = (sub - mask) & mask;          // next subset of mask
    }

    std::vector<uint64_t> table(size);
    for (long long tries = 0; tries < 100000000LL; tries++) {
        uint64_t magic = sparseRandom();
        if (popcount((mask * magic) & 0xFF00000000000000ULL) < 6) continue;   // quick reject

        std::fill(table.begin(), table.end(), 0ULL);   // 0 = empty (a real attack set is never 0)
        bool ok = true;
        for (int i = 0; i < size && ok; i++) {
            int idx = (int)((occ[i] * magic) >> (64 - bits));
            if (table[idx] == 0) table[idx] = ref[i];
            else if (table[idx] != ref[i]) ok = false;   // two different attack sets share a slot
        }
        if (ok) return magic;
    }
    std::cerr << "no magic found for square " << sq << "\n";
    return 0;
}

static void printArray(const char *name, bool rook) {
    std::cout << "static constexpr uint64_t " << name << "[64] = {\n";
    for (int sq = 0; sq < 64; sq++) {
        uint64_t m = findMagic(sq, rook);
        std::cout << "    0x" << std::hex << m << std::dec << "ULL" << (sq < 63 ? "," : "") << "\n";
        std::cerr << name << " " << sq << " done\n";
    }
    std::cout << "};\n\n";
}

int main() {
    std::cout << "#pragma once\n#include <cstdint>\n\n";
    printArray("ROOK_MAGICS", true);
    printArray("BISHOP_MAGICS", false);
}