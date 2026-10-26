// Magic bitboards check: compares the table lookups (rookAttacks, bishopAttacks, queenAttacks) with the
// ray-loop functions they replaced, on every square and 200,000 random occupancies per square.
// Build and run (from the repo root, Git Bash):
//   g++ -O2 -std=c++17 -Isrc tools/tests/test_magics.cpp src/magics.cpp src/attacks.cpp -o tools/tests/test_magics.exe
//   ./tools/tests/test_magics.exe
#include <iostream>
#include <random>
#include "magics.h"

int main() {
    initMagics();
    std::mt19937_64 rng(42);
    long long checks = 0, mismatches = 0;
    for (int sq = 0; sq < 64; sq++) {
        for (int i = 0; i < 200000; i++) {
            uint64_t occ = rng();
            if (i % 3 == 1) occ &= rng();            // sparse boards
            if (i % 3 == 2) occ |= rng();            // dense boards
            if (rookAttacks(sq, occ)   != rookAttacksSlow(sq, occ))   mismatches++;
            if (bishopAttacks(sq, occ) != bishopAttacksSlow(sq, occ)) mismatches++;
            if (queenAttacks(sq, occ)  != queenAttacksSlow(sq, occ))  mismatches++;
            checks += 3;
        }
    }
    std::cout << checks << " checks, " << mismatches << " mismatches\n";
    return mismatches != 0;
}
