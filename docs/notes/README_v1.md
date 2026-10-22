# Torrino - a UCI chess engine written from scratch in C++

<!-- TODO (optional): a banner image, e.g. a screenshot of the bot playing on Lichess
     or a board diagram. Save it as docs/images/banner.png and uncomment the next line. -->
<!-- ![Torrino](docs/images/banner.png) -->

**Torrino** is a classical chess engine (no neural network) written from scratch in C++.
It speaks the UCI protocol, so it can be used with any chess GUI, and it plays online on Lichess as
**[TorrinoBOT](https://lichess.org/@/TorrinoBOT)**.

The project was developed iteratively: every version adds one idea, is verified for correctness,
and is then **measured** in engine-vs-engine tournaments before being kept.
The full log of every change and every test is in **[VERSIONS.md](VERSIONS.md)**.

| | |
|---|---|
| **Language** | C++17, single-threaded, no external libraries |
| **Board** | bitboards + mailbox |
| **Search** | negamax alpha-beta, iterative deepening, quiescence search, check extension |
| **Evaluation** | hand-crafted, tapered (middlegame/endgame) |
| **Strength** | ≈ Stockfish `UCI_Elo` 2000 at 10+0.1 (55.5% in 100 games, v10) |
| **Lichess** | bullet ≈ TODO, blitz ≈ TODO (TorrinoBOT, v10) |

> **Status:** active development (current version: **v10**). Next steps: magic bitboards, transposition table.

---

## Table of contents

1. [Highlights](#highlights)
2. [Features](#features)
3. [Project structure](#project-structure)
4. [Building and running](#building-and-running)
5. [How it is tested](#how-it-is-tested)
6. [Results](#results)
7. [Case studies: bugs and weaknesses found in real games](#case-studies-bugs-and-weaknesses-found-in-real-games)
8. [Known limitations](#known-limitations)
9. [Roadmap](#roadmap)
10. [How this project was developed](#how-this-project-was-developed)
11. [References](#references)
12. [License](#license)

---

## Highlights

- **Measured progress**: every version is tested against the previous one in 100-game matches
  (cutechess-cli, balanced opening book, colours swapped). From v4 to v10 each step was a
  statistically significant improvement.
- **v10 vs v9: +135 ± 69 Elo** in a direct match (62 wins, 25 losses, 13 draws).
- **Correctness first**: move generation verified with perft on the 6 standard test positions;
  every evaluation change checked for colour symmetry; every speed optimisation checked to
  produce exactly the same search (same node count at fixed depth).
- **Weaknesses found in real games** on Lichess, analysed with Stockfish, and turned into
  regression tests (see [Case studies](#case-studies-bugs-and-weaknesses-found-in-real-games)).

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/elo_progression_dark.png">
  <img alt="Cumulative Elo progression from v3 to v10" src="docs/images/elo_progression_light.png">
</picture>

> The cumulative curve chains the version-vs-previous-version matches, so it is an estimate:
> errors add up along the chain (shaded band), and chained self-play results tend to overestimate
> the total gain.

---

## Features

### Board representation and move generation
- **Bitboards** (one 64-bit integer per piece type and colour) plus a mailbox array for fast lookups.
- Precomputed attack tables for pawns, knights and kings; sliding-piece attacks computed by ray loops
  (magic bitboards planned for v11).
- Pseudo-legal move generation with a legality check after each move; all special moves
  (castling, en passant, promotions).
- `makeMove` / `unmakeMove` with incremental updates (no board copies).
- Verified with **perft** (see [How it is tested](#how-it-is-tested)).

### Search
- **Negamax with alpha-beta pruning**.
- **Iterative deepening**, reusing the best move of the previous depth.
- **Quiescence search** (captures and queen promotions) with stand-pat, against the horizon effect.
- **Move ordering**: previous best move at the root, MVV-LVA for captures, **killer moves** for quiet moves.
- **Check extension**: positions in check are searched one ply deeper.
- **Mate scores** by distance from the root, reported as `score mate N`.
- **Zobrist hashing** with incremental key updates, used for **repetition detection** and the
  **fifty-move rule** (draw score inside the search).

### Evaluation (hand-crafted, tapered)
- Material + piece-square tables.
- **Tapered evaluation**: separate middlegame and endgame scores blended by the game phase
  (computed from the remaining pieces).
- **King safety** (middlegame): castling rights, pawn shield, exposed king, enemy pieces attacking
  the squares around the king.
- **Pawn structure**: passed pawns (bonus growing with the rank, larger in the endgame),
  isolated, doubled and defended pawns.
- **Endgames**: king centralisation and **mop-up** (drives the losing king to the corner to deliver mate).
- Development and early-queen penalties in the opening.

### Time management
- Soft limit (ideal time per move) and hard limit (deadline).
- Estimates the cost of the next depth from the previous iterations and does not start a depth
  that cannot finish; keeps the best move of an interrupted depth.
- Plays instantly when there is only one legal move.

### Protocol and tools
- **UCI** protocol: works with cutechess, En Croissant, Arena, and with
  [lichess-bot](https://github.com/lichess-bot-devs/lichess-bot).
- Extra commands: `go depth N`, `go movetime T`, `eval` (prints the static evaluation and the game phase).

---

## Project structure

| File | Content |
|---|---|
| `src/types.h`, `src/bitboard.h` | basic types (pieces, squares, colours) and bitboard helpers |
| `src/board.h/.cpp` | board state, FEN parsing, `makeMove`/`unmakeMove`, attack detection, Zobrist key and history |
| `src/attacks.h/.cpp` | precomputed attack tables and sliding-piece attacks |
| `src/movegen.h/.cpp` | move generation, move ordering (MVV-LVA, killers), move parsing/printing |
| `src/zobrist.h/.cpp` | random keys for Zobrist hashing |
| `src/search.h/.cpp` | negamax, quiescence search, iterative deepening, time management |
| `src/eval.h/.cpp` | evaluation: tapered PST, king safety, pawn structure, mop-up |
| `src/uci.h/.cpp` | UCI protocol loop |
| `src/perft.h/.cpp` | perft (move generation testing) |
| `src/gameloop.h/.cpp`, `src/main.cpp` | entry point and a simple terminal game loop |
| `VERSIONS.md` | full development log: changes and test results of every version |
| `pgn/` | Lichess games used to find weaknesses |

---

## Building and running

### Build
Requirements: a C++17 compiler (developed with g++ / MinGW on Windows).

From the `src` folder:

```bash
g++ -O3 -march=native -flto -static -std=c++17 *.cpp -o ../bin/torrino.exe
```

<!-- TODO: once there is a Makefile or CMakeLists.txt, replace this with a one-command build. -->

### Run in a terminal (UCI)
```text
uci
position startpos moves e2e4 e7e5
go movetime 2000
eval
quit
```
During the search the engine prints one line per completed depth, for example:
```text
info depth 7 score cp 35 nodes 2847000 nps 1300000 time 2190 pv e2e4
bestmove e2e4
```

### Use it with a GUI or on Lichess
- **GUI**: add the executable as a UCI engine in cutechess, En Croissant or Arena.
- **Lichess**: via [lichess-bot](https://github.com/lichess-bot-devs/lichess-bot) (`protocol: uci`).

---

## How it is tested

### 1. Correctness: perft
Perft counts all the leaf nodes of the move tree to a given depth; the numbers must match the
known values exactly. All six standard positions from the Chess Programming Wiki pass:

| Position | Depth | Nodes |
|---|---|---|
| Start position | 5 | 4,865,609 |
| Kiwipete | 4 | 4,085,603 |
| Position 3 | 5 | 674,624 |
| Position 4 | 4 | 422,333 |
| Position 5 | 4 | 2,103,487 |
| Position 6 | 4 | 3,894,594 |

Perft is also used to check the **incremental Zobrist key** (equal to the key recomputed from
scratch at every node) and the repetition history (balanced after every `unmakeMove`).
Move generation in tricky en passant positions was cross-checked against
[python-chess](https://python-chess.readthedocs.io/).

### 2. Correctness: evaluation symmetry
A position and its colour-mirrored copy must get exactly the same evaluation. Every evaluation
change was checked on a fixed set of positions (start position, kiwipete, middlegames, endgames).

### 3. Speed changes: same search, fewer seconds
Optimisations that must not change the search (e.g. the king-safety pre-filter in v10.2) are checked
by comparing the **node count at fixed depth** before and after: it must be identical.

### 4. Strength: engine-vs-engine matches
- Tool: [cutechess-cli](https://github.com/cutechess/cutechess).
- Each new version plays **100 games against the previous version** (time control 10+0.1),
  starting from a **balanced opening book**, each opening played **with both colours**.
- Result reported as Elo difference with 95% error margin and LOS (likelihood of superiority).
- External reference: **Stockfish with limited strength** (`UCI_Elo` 1500–2000).
- Additional checks on time management: thinking time per move and clock left at the end,
  extracted from the game records.

### 5. Real games: Lichess
TorrinoBOT plays rated games against other bots and humans. Lost or drawn games are analysed with
Stockfish; the critical positions become regression tests for the next version.

---

## Results

### Version by version (each version vs the previous one)

| Version | Main change | Opponent | TC | Games | W-L-D | Elo |
|---|---|---|---|---|---|---|
| v4 | quiescence search | v3 | 10+0.1 | 100 | 53-2-45 | +195 ± 50 |
| v5 | killer moves | v4 | 10+0.1 | 100 | 22-5-73 | +59 ± 34 |
| v6 | check extension, mate scores | v5 | 10+0.1 | 100 | 21-12-67 | +31 ± 39 |
| v7 | Zobrist hashing, repetition detection | v6 | 10+0.1 | 100 | 34-15-51 | +67 ± 48 |
| v8 | time management (soft/hard limit) | v7 | 10+0.5 | 100 | 40-23-37 | +60 ± 55 |
| v9 | time management 2 (next-depth estimate) | v8 | 30+1 | 50 | 15-8-27 | +49 ± 66 |
| v10 | evaluation rework (3 steps, see below) | v9 | 10+0.1 | 100 | 62-25-13 | +135 ± 69 |

v10 was developed in three steps, each tested against the previous one:

| Step | Change | W-L-D | Elo |
|---|---|---|---|
| v10.1 | tapered evaluation + mop-up | 51-17-32 vs v9 | +123 ± 59 |
| v10.2 | king safety | 45-30-25 vs v10.1 | +53 ± 60 |
| v10.3 | pawn structure | 56-28-16 vs v10.2 | +100 ± 65 |

> Note: the sum of the three steps (+276) is larger than the direct v10 vs v9 result (+135):
> gains measured against the immediately previous version tend to overestimate the total,
> so the direct match is the number reported above.

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/elo_per_version_dark.png">
  <img alt="Elo gained by each version against the previous one" src="docs/images/elo_per_version_light.png">
</picture>

### Against Stockfish (limited strength)

| Version | Opponent | Games | W-L-D | Score | Elo |
|---|---|---|---|---|---|
| v5 | SF `UCI_Elo` 2000 | 100 | 37-55-8 | 41.0% | −63 ± 67 |
| v6 | SF 2000 | 100 | 50-43-7 | 53.5% | +24 ± 66 |
| v7 | SF 2000 | 100 | 50-44-6 | 53.0% | +20 ± 67 |
| v8 | SF 2000 | 100 | 51-42-7 | 54.5% | +31 ± 67 |
| v10 | SF 2000 | 100 | 52-41-7 | 55.5% | +38 ± 67 |

Against limited Stockfish the progress is much smaller than in self-play: Stockfish limited mostly
loses through tactical mistakes, which a deeper search catches better than a better evaluation.
This motivates the next versions (faster move generation, transposition table).

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/vs_sf2000_dark.png">
  <img alt="Score against Stockfish UCI_Elo 2000, version by version" src="docs/images/vs_sf2000_light.png">
</picture>

### Lichess rating

| Version | Bullet (end of period) | Blitz (end of period) |
|---|---|---|
| v4 | 1869 | 1663 |
| v5 | 1848 | 1704 |
| v7 | 1894 | 1748 |
| v9 | 1948 | 1753 |
| v10 | TODO | TODO |

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/lichess_rating_dark.png">
  <img alt="TorrinoBOT Lichess rating at the end of each version's online period" src="docs/images/lichess_rating_light.png">
</picture>

<!-- Charts are generated by tools/make_charts.py from the data in VERSIONS.md:
     python tools/make_charts.py docs/images -->
<!-- TODO: add v10 to the LICHESS list in tools/make_charts.py once its rating is known, then re-run. -->

### Example: converting basic endgames (v9 vs v10.1)
Self-play from basic winning endgames, 0.2 s per move:

| Position | v9 | v10.1 |
|---|---|---|
| KR vs K, king in the centre | draw (fifty-move rule) | **mate in 14** |
| KR vs K, king on d5 | draw (fifty-move rule) | **mate in 14** |
| KQ vs K, king in the centre | mate in 10 | **mate in 12** |
| KQ vs K, king on c5 | draw (fifty-move rule) | **mate in 11** |

---

## Case studies: bugs and weaknesses found in real games

<!-- Each case: a board image of the critical position (e.g. exported from the Lichess analysis board),
     what went wrong, why, and what fixed it. Save images in docs/images/. -->

### 1. Pushing the pawns in front of its own king (fixed in v10.2)
In a 3+1 game the engine, a clear pawn up (Stockfish +4.2), played **39.f4??** and then **41.f5??**,
opening its own king, and later walked the king to h3. The evaluation had no notion of king safety.
After v10.2 (pawn shield, exposed king, attackers near the king) the engine plays Kg1 in the same position.

> 🖼️ *Image placeholder — position before 39.f4 (`3r2k1/1p3rpn/3p1p1p/P1pP3q/2N1P2P/2Q1PPR1/1PP3K1/7R w - - 5 39`).*
<!-- ![King safety case](docs/images/case_king_safety.png) -->

### 2. Ignoring a passed pawn (fixed in v10.3)
Winning by +4.4, the engine moved its rook away from the blockade of the opponent's h-pawn
(**27...Rf6?**) to win two pawns; the h-pawn ran to h7 and the game ended in a draw by perpetual check.
The evaluation did not value passed pawns. After v10.3 the engine keeps the blockade (27...Nb6 at depth 8).

> 🖼️ *Image placeholder — position before 27...Rf6 (`8/ppp2p2/7r/2kPp2P/P1n1P3/2K5/5P2/7R b - - 0 27`).*
<!-- ![Passed pawn case](docs/images/case_passed_pawn.png) -->

### 3. Drawing won positions by repetition (fixed in v7)
Before v7 the engine did not know that a repeated position is a draw: a rook up, it shuffled its pieces
until the opponent claimed threefold repetition. With Zobrist keys and a position history, a repetition
scores 0 inside the search, so a winning engine avoids it and a losing engine looks for it.

### 4. Other bugs found during testing
- **Uninitialised en passant square** causing random crashes (found when a cutechess match stopped
  on an engine crash; fixed before v4).
- **Time management** that ended 30+1 games with more time than at the start (measured from the game
  records, fixed in v9).

---

## Known limitations

- **Search depth**: about 7–9 plies in blitz; tactics beyond the horizon are missed.
- **No transposition table**: the same positions are searched many times (especially in endgames:
  depth 9 in KR vs K needs ~7 million nodes).
- **Slow sliding-piece attacks**: ray loops instead of magic bitboards; about 1 million nodes per second
  in the middlegame on a laptop CPU.
- **No mobility / piece activity** terms yet.
- **No long-term plans** in roughly equal endgames (it avoids repetition but can shuffle without progress).
- Single-threaded.

---

## Roadmap

- [ ] **v11 — Magic bitboards**: fast sliding-piece attacks (move generation, check detection, evaluation).
- [ ] **v12 — Transposition table**: reuse search results across transpositions.
- [ ] Mobility and piece activity (rooks on open files, bishop pair).
- [ ] History heuristic, null-move pruning, late move reductions, principal variation search.
- [ ] `bench` command for reproducible speed measurements.
- [ ] Opening book (Polyglot) with an `OwnBook` UCI option.
- [ ] Automatic tuning of the evaluation weights (Texel tuning).

---

## How this project was developed

<!-- TODO: write this section in your own words. Admission committees value honesty about the process.
     Suggested points:
     - why you started the project and what you wanted to learn;
     - what you designed and implemented yourself;
     - which tools you used and how: e.g. an AI assistant (Claude) used as a tutor for explanations,
       for code review, for writing test scripts and charts, and for some functions it wrote on request
       (state which ones honestly, e.g. parts of the v10.2 king-safety and v10.3 pawn-structure code);
     - the testing method: every change verified (perft, symmetry, node counts) and measured (tournaments);
     - what you learned (e.g. statistics of engine testing, debugging, version control). -->

*TODO — personal section.*

---

## References

- [Chess Programming Wiki](https://www.chessprogramming.org/) — algorithms, perft positions, evaluation ideas.
- [UCI protocol specification](https://www.shredderchess.com/chess-features/uci-universal-chess-interface.html).
- [cutechess](https://github.com/cutechess/cutechess) — engine-vs-engine matches.
- [lichess-bot](https://github.com/lichess-bot-devs/lichess-bot) — bridge between UCI engines and Lichess.
- [Stockfish](https://stockfishchess.org/) — used as a sparring partner and for game analysis.
- [python-chess](https://python-chess.readthedocs.io/) — reference move generator for cross-checks.

---

## License

<!-- TODO: add a LICENSE file (e.g. MIT) and update this line. -->
TODO — license not chosen yet.

## Author

Giorgio Paoli — <!-- TODO: contact / LinkedIn / university --> TODO
