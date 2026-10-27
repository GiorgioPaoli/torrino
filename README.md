# Torrino

**A UCI chess engine written from scratch in C++17: bitboards, alpha-beta search with a transposition table and pruning, and a hand-crafted tapered evaluation, developed and measured version by version (≈ Stockfish `UCI_Elo` 2400 at 10+0.1).**

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey)
![Lichess bot](https://img.shields.io/badge/Lichess-TorrinoBOT-black)
![License: MIT](https://img.shields.io/badge/license-MIT-green)

<!-- HERO GIF: must be the GIF of a whole GAME (animated), not of a single position.
     On the Lichess game page: "Share & export" -> GIF, or https://lichess1.org/game/export/gif/<GAME_ID>.gif
     Save it as docs/gifs/hero.gif (replacing the current image). -->
<p align="center"><img src="docs/gifs/hero.gif" width="360" alt="TorrinoBOT playing a game on Lichess"></p>

<p align="center">
Torrino plays bullet and blitz on Lichess as <a href="https://lichess.org/@/TorrinoBOT"><b>TorrinoBOT</b></a>
(bullet 2058, blitz 1897 on October 26, 2026).<br>
Every version is checked for correctness and measured in engine-vs-engine matches; the full log is in <a href="VERSIONS.md">VERSIONS.md</a>.
</p>

---

## Torrino in short

- **Written from scratch in C++17, no chess libraries:** bitboard board representation with magic bitboards for
  sliding pieces, legal move generation verified with perft.
- **Alpha-beta search:** negamax with iterative deepening, quiescence search, a transposition table, principal
  variation search, null-move pruning and late move reductions, plus move ordering by MVV-LVA and killer moves.
- **Hand-crafted evaluation:** material and piece-square tables blended between middlegame and endgame, king safety,
  pawn structure and endgame technique.
- **Measured, not guessed:** 13 versions, each tested in matches against the previous one and against Stockfish;
  every change and every result is logged in [VERSIONS.md](VERSIONS.md).

| | |
|---|---|
| **Strength** | about even with Stockfish limited to `UCI_Elo` 2400 at 10+0.1 (score 52% in 100 games: wins plus half the draws); 79.5% against `UCI_Elo` 2000 |
| **Search depth** | in 10+0.1 games, about 0.26 s per move: 8 plies on average in the middlegame, 9.6 in the endgame (10% of endgame moves reach 14 or more) |
| **Speed** | about 2.7 million positions per second on one laptop core; depth 7 in about 0.1 s per standard test position |
| **Online** | Lichess bullet 2058, blitz 1897 (October 26, 2026) |

---

## Table of contents

1. [Quick start](#quick-start)
2. [How the engine works](#how-the-engine-works)
   - [Board and move generation](#board-and-move-generation)
   - [Search](#search)
   - [Time management](#time-management)
   - [Evaluation](#evaluation)
3. [Version history](#version-history)
4. [Results](#results)
5. [Testing and validation](#testing-and-validation)
6. [Usage (UCI)](#usage-uci)
7. [Known weaknesses and next steps](#known-weaknesses-and-next-steps)
8. [What I learned](#what-i-learned)
9. [About](#about)
10. [References](#references)

---

## Quick start

```bash
git clone https://github.com/GiorgioPaoli/torrino.git
cd torrino/src
g++ -O3 -march=native -flto -static -std=c++17 *.cpp -o ../bin/torrino.exe
```
Requires a C++17 compiler (developed with g++ / MinGW-w64 on Windows). Then run `bin/torrino.exe` in any UCI
interface; the commands are listed in [Usage (UCI)](#usage-uci).
<!-- TODO: replace with a Makefile or CMakeLists.txt so the build is one command, and test it on a clean machine. -->

---

## How the engine works

> [!TIP]
> **Want the full picture?** This section is a short overview. The complete explanation, with step-by-step diagrams
> of the search, how each move is chosen and how positions are evaluated, is in **[How Torrino works](docs/ENGINE.md)**.

```mermaid
flowchart LR
    A["1. GUI sends<br/>position + clocks"] --> B["2. time budget<br/>soft: target<br/>hard: deadline"]
    B -->|"only one legal move:<br/>play it at once"| Z["6. bestmove"]
    B -->|otherwise| D["3. next depth<br/>1, 2, 3 ..."]
    D --> E["4. search every move<br/>negamax → quiescence<br/>→ evaluate"]
    E --> I{"5. time for<br/>another depth?"}
    I -->|yes| D
    I -->|no| Z
    E -.->|"hard deadline: stop,<br/>keep best move so far"| Z
```

The engine receives the position and the clocks, sets a time budget, and searches to depth 1, then 2, then 3
(**iterative deepening**), each time starting from the best move found so far. After each depth it decides whether
there is time for another one; if the deadline arrives in the middle of a depth, it plays the best move found so far.
[Step by step](docs/ENGINE.md#from-position-to-move).

### Board and move generation
The board is twelve 64-bit integers (**bitboards**), one per piece type and colour. Attacks of bishops, rooks and
queens come from **magic bitboards**, a single table lookup. Moves are generated, checked for legality, and made and
unmade incrementally, together with the position's **Zobrist key** (a 64-bit hash of the position).
[More](docs/ENGINE.md#board-and-move-generation).

### Search
**Negamax with alpha-beta**, which skips branches that cannot change the result, extended with: **quiescence search**
(captures are resolved before evaluating), a **transposition table** (results of positions already searched are
reused), **null-move pruning** and **late move reductions** (unpromising moves are searched less deeply),
**principal variation search** (moves after the first are only tested), and move ordering by the table move,
MVV-LVA captures and killer moves. [Diagrams and details](docs/ENGINE.md#search).

### Time management
A **soft** limit (the time to aim for) and a **hard** limit (a deadline the engine never exceeds), both computed from
the clock and the increment. After each depth the engine decides whether to start another one.
[More](docs/ENGINE.md#time-management).

### Evaluation
A hand-crafted score in centipawns, blended between middlegame and endgame weights by the game phase: material and
piece-square tables, development, **king safety**, **pawn structure** (passed, isolated, doubled, defended pawns) and
**mop-up** to convert won endgames. [Details](docs/ENGINE.md#evaluation).

**→ Read the full explanation, with diagrams: [How Torrino works](docs/ENGINE.md)**

---

## Version history

The engine was built in small steps; each version is a tag in the repository. How much each one gained is in
[Results](#results), the full test logs in [VERSIONS.md](VERSIONS.md).

| Version | What it added | Area |
|---|---|---|
| v1-v3 | First working engine: board, move generation, basic search, alpha-beta, iterative deepening | search |
| v4 | Quiescence search, development in the evaluation, en passant fix | search |
| v5 | Killer moves | search |
| v6 | Check extension, mate scores | search |
| v7 | Zobrist hashing, repetition detection, fifty-move rule | search |
| v8-v9 | Soft and hard time limits, next-depth estimate, keeping interrupted depths | time |
| v10 | Tapered evaluation and mop-up, king safety, pawn structure | evaluation |
| v11 | Magic bitboards, captures-only quiescence, lazy move ordering | speed |
| v12 | Transposition table (move ordering, cutoffs, mate scores) | search |
| v13 | Principal variation search, null-move pruning, late move reductions; new time management | search, time |

---

## Results

All local tests: 100-game matches at 10+0.1 (10 seconds plus 0.1 per move) in cutechess, with balanced openings and
colours swapped, on one laptop (Intel i5, single thread per engine). The results of every match, benchmark and
Lichess period, version by version, are in [VERSIONS.md](VERSIONS.md).

How to read the numbers:
- **Score** = (wins + half the draws) / games. 50% means equal strength; 79.5% means the engine took 79.5 points
  out of 100 games.
- **Elo difference** is the score converted to a rating gap, given with its uncertainty: **+70 ± 64** means the
  measured difference is +70, and with 95% confidence the true one lies between +6 and +134. 100 games give an
  uncertainty of about ± 60-70 Elo, which is why small gains are not always significant.
- In the charts, the vertical lines on bars and points show that uncertainty range.

### Strength across versions

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/elo_round_robin_dark.png">
  <img alt="Elo of every version from v4 to v13 on one scale, from a round-robin tournament, with 95% intervals" src="docs/images/elo_round_robin_light.png">
</picture>

All ten versions from v4 to v13 played each other in one round-robin tournament: 50 games per pair, 2250 games at
10+0.1 (6 lost on time). This puts every version on the same scale: v13 is about **576 Elo** stronger than v4. The
largest steps came from the time management (v8), the evaluation rework (v10), the speed work (v11), the
transposition table (v12) and the pruning (v13). Differences between neighbours below about 40 Elo are within the
uncertainty: v6 is level with v5, and v9 is not stronger than v8 at this time control (its time management change
was tested at 30+1). The matches of each version against the previous one, summed, give +735 for the same versions:
gains measured one step at a time tend to be overestimated. All results are in [VERSIONS.md](VERSIONS.md); the
comparison with Stockfish below is the external check.

### Against Stockfish

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/vs_stockfish_dark.png">
  <img alt="Score against Stockfish: all versions against UCI_Elo 2000, and v12 and v13 against UCI_Elo 2000 to 2400" src="docs/images/vs_stockfish_light.png">
</picture>

Against the same opponent (Stockfish limited to `UCI_Elo` 2000) the score (wins plus half the draws) rose from 41% (v5) to 79.5% (v13). From v6
to v10 it stayed around 53-56%: those versions improved the evaluation and the time management, but limited
Stockfish loses mainly through tactical errors, which only a deeper search exploits. The transposition table and the
pruning (v12, v13) changed that. Against stronger levels v13 scores 52% against `UCI_Elo` 2400. At 10+0.1 these
levels are compressed (2200 and 2300 gave almost the same score), so they are a reference scale, not a rating.

### Search depth

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/depth_per_version_dark.png">
  <img alt="Average search depth per version in 10+0.1 games" src="docs/images/depth_per_version_light.png">
</picture>

Up to v11 the depth reached in games stayed between 5 and 5.5 plies, while the strength kept rising: those gains came
from searching better and evaluating better, not deeper. v10's richer evaluation even cost some depth. The
transposition table and the pruning then added about 2.4 plies at the same time per move. Data: about 210,000
engine moves from the match records in [`docs/matches`](docs/matches).

### Speed

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/speed_per_version_dark.png">
  <img alt="Time to reach depth 7 per version, log scale" src="docs/images/speed_per_version_light.png">
</picture>

Time to reach depth 7 on the 6 standard test positions. v4 is slower than v3 because quiescence search resolves every
capture sequence (and plays much better for it); v10 is slower because of its evaluation. v11 made every node faster
(magic bitboards), while v12 and v13 search far fewer nodes: their speed in nodes per second stays about the same, but
the same depth takes about 3 and then 15 times less time than with v11. Data in [`docs/data/bench_all_versions.csv`](docs/data/bench_all_versions.csv).

**Time usage.** In local matches the engine uses about 0.25 s per move at 10+0.1. On Lichess it used to finish 60+1
games with about 25 s left on the clock, time it could have spent searching; v13 changes the stop rule and spends a
larger share per move.

**Endgames.** v9 drew won endgames by the fifty-move rule (6 games a queen or a rook up in one match); with mop-up,
v10.1 mated in 11-14 moves in all test positions. With the transposition table and pruning the engine now also sees
the mate. With king and rook against king (`8/8/8/4k3/8/8/8/R3K3 w - - 0 1`), v11 stopped at depth 10
(66 million nodes, 7.6 s) without seeing a mate; v13 reports mate in 17 at depth 20 (49 million nodes, 9.2 s). With king and
queen against king (`8/8/8/4k3/8/8/8/3QK3 w - - 0 1`), v11 stopped at depth 7 (3.0 s) without a mate, v13 reports mate in 11 at
depth 15 (20 million nodes, 4.2 s).

### Lichess

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/lichess_rating_dark.png">
  <img alt="TorrinoBOT daily Lichess rating" src="docs/images/lichess_rating_light.png">
</picture>

Bullet rose from 1808 (v4) to 2058, blitz from 1689 to 1897 (October 26, 2026). Online ratings are noisy (changing
opponents, many of them bots with opening books, network lag) and complement the matches above without replacing them.
Torrino plays without an opening book.

---

## Testing and validation

| Check | Method | Status |
|---|---|---|
| Move generation | perft (count of all positions reachable at a given depth) on the 6 standard test positions, compared with the published values | all match |
| Zobrist keys | incremental key compared with the key recomputed from scratch at every perft node | pass |
| Magic bitboards | lookups compared with the old ray loops on 38.4 million random occupancies | 0 mismatches |
| Null move | hash and state checked after make / unmake at 177,905 positions | 0 errors |
| Repetition history | history balanced after every `unmakeMove`; 8 repetition test cases | pass |
| En passant edge cases | perft cross-checked against [python-chess](https://python-chess.readthedocs.io/) | pass |
| Evaluation | colour-mirrored positions must evaluate identically | checked on every change |
| Speed optimisations | identical node counts and best moves at fixed depth before and after | checked on every change |
| Strength | matches against the previous version and against Stockfish; round-robin of v4-v13 | every version |

The perft, magic bitboard and null move checks can be rerun with the programs in [`tools/tests/`](tools/tests/)
(build and run commands at the top of each file).

---

## Usage (UCI)

Torrino speaks UCI (Universal Chess Interface), so it works with any UCI GUI, such as cutechess, En Croissant or
Arena, and with [lichess-bot](https://github.com/lichess-bot-devs/lichess-bot).

Commands: `uci`, `isready`, `ucinewgame`, `position startpos | fen <fen> [moves ...]`,
`go depth N | movetime T | wtime W btime B [winc WI binc BI]`, `quit`, plus `eval` (static evaluation, for debugging).

```text
position startpos moves e2e4 e7e5
go depth 6
info depth 6 score cp 0 nodes 416341 nps 1764156 time 236 pv b1c3
bestmove b1c3
```

---

## Known weaknesses and next steps

The search is now fairly complete; most of what Torrino lacks is chess knowledge in the evaluation, which is
written and tuned by hand:
- **Piece activity and coordination:** no mobility term, no rooks on open files or on the 7th rank, no rook behind a
  passed pawn, no batteries, no bishop pair, no knight outposts.
- **Opening play:** without an opening book and without a notion of tempo, it sometimes develops a piece and moves it
  again several times (a bishop check that a pawn simply blocks).
- **Technical limits:** single thread, no endgame tablebases, time management tuned mainly for bullet.

**Next step: a learned evaluation.** The same search with a small neural network as the evaluation (NNUE-style),
trained on positions evaluated by Stockfish, to compare a hand-crafted and a learned evaluation under identical
conditions. After that: tuning the evaluation weights from data, and an opening book for online play.

---

## What I learned

I started Torrino for fun: I liked the idea of building my own chess engine from scratch, and of the moment when it
would beat me. Along the way it became a lesson in algorithms, performance and testing.

- **Writing fast code, step by step.** From bitboards and bitwise operations to magic bitboards, then to the
  transposition table with Zobrist hashing and the pruning inside negamax. What surprised me most were the gains of
  the search algorithms: the transposition table, null-move pruning and late move reductions together made depth 7
  about 25 times faster than v10. I find Zobrist hashing especially elegant: a whole position becomes one 64-bit
  number, updated with a few XORs per move.
- **Algorithms depend on each other.** The transposition table only works because of Zobrist keys; late move
  reductions and null-move pruning are only safe because the move ordering (helped by the transposition table) puts
  the best moves first. Adding a technique often meant understanding the ones it relies on.
- **Correctness comes before speed.** One mistake in `unmakeMove` is enough to break the whole engine. I learned to
  prove the code correct with perft, node counts and targeted tests before trusting any result.
- **Measure, don't guess.** Every version was checked with perft, speed and depth benchmarks, and above all a match
  against the previous version: I wanted proof that it was better. 100 games still leave a large uncertainty that
  only shrinks with more games; that was a compromise with the time I had. One result that changed how I think about
  engines: from v5 to v11 the search depth stayed around 5-5.5 plies while the strength kept rising, thanks to search
  quality (move ordering, quiescence, repetition handling) and the evaluation. v10 even searched less deep than v9
  and still won clearly.
- **Real games show what to fix.** I looked at the bot's games on Lichess to find its weaknesses: for example it could
  not win king and rook against king, so I knew what to work on next. The same happened with king safety and passed
  pawns.
- **Time management is a compromise.** v8 spent too much time in the middlegame and ended blitz games short of time;
  from v9 on the engine went the other way and finished 60+1 games with 20 seconds or more left.

**What I would do differently.** Pay more attention to how the bot actually plays, and experiment much more with the
piece-square tables, positional play and the evaluation as a whole, ideally tuning the weights automatically from
data instead of by hand. That is where the next step goes: a neural-network evaluation (NNUE-style), trained on
positions instead of written by hand (see [Known weaknesses and next steps](#known-weaknesses-and-next-steps)).

---

## About

I'm Giorgio Paoli, a third-year student of Computer and Control Engineering (Ingegneria Informatica e Automatica) at
Sapienza University of Rome. I started Torrino at the end of my second year, alongside my studies, and developed it up
to the current version (v13, October 26, 2026): about three months in total, two of which with the project on hold.

The testing and measurement tools in [`tools/`](tools/) were not written by me: the scripts that measure the engine
(search depth from the game records, speed and nodes per second, charts), the correctness tests in
[`tools/tests/`](tools/tests/) (perft, null move, magic bitboards) and the program that finds the magic numbers
for the magic bitboards ([`tools/find_magics.cpp`](tools/find_magics.cpp)) were written by an AI assistant (Claude),
which I also used as a tutor while learning the algorithms.

---

## References

**Algorithms** ([Chess Programming Wiki](https://www.chessprogramming.org/)):
[Bitboards](https://www.chessprogramming.org/Bitboards) ·
[Magic Bitboards](https://www.chessprogramming.org/Magic_Bitboards) ·
[Perft Results](https://www.chessprogramming.org/Perft_Results) ·
[Alpha-Beta](https://www.chessprogramming.org/Alpha-Beta) ·
[Iterative Deepening](https://www.chessprogramming.org/Iterative_Deepening) ·
[Quiescence Search](https://www.chessprogramming.org/Quiescence_Search) ·
[MVV-LVA](https://www.chessprogramming.org/MVV-LVA) ·
[Killer Heuristic](https://www.chessprogramming.org/Killer_Heuristic) ·
[Check Extensions](https://www.chessprogramming.org/Check_Extensions) ·
[Zobrist Hashing](https://www.chessprogramming.org/Zobrist_Hashing) ·
[Transposition Table](https://www.chessprogramming.org/Transposition_Table) ·
[Principal Variation Search](https://www.chessprogramming.org/Principal_Variation_Search) ·
[Null Move Pruning](https://www.chessprogramming.org/Null_Move_Pruning) ·
[Late Move Reductions](https://www.chessprogramming.org/Late_Move_Reductions) ·
[Tapered Eval](https://www.chessprogramming.org/Tapered_Eval) ·
[Time Management](https://www.chessprogramming.org/Time_Management)

**Time management discussions:**
[VeryChess 0.3.0 release](https://talkchess.com/viewtopic.php?p=995986) ·
[Time management and estimating search times](https://talkchess.com/viewtopic.php?p=894313) ·
[Easy moves and best-move stability](https://talkchess.com/viewtopic.php?p=402413) ·
[Crafty time.c](https://sources.debian.org/src/crafty/23.4-6/time.c/)

**Protocol and tools:**
[UCI protocol](https://www.shredderchess.com/chess-features/uci-universal-chess-interface.html) ·
[cutechess](https://github.com/cutechess/cutechess) (matches) ·
[lichess-bot](https://github.com/lichess-bot-devs/lichess-bot) (online play) ·
[Stockfish](https://stockfishchess.org/) (reference opponent and game analysis) ·
[python-chess](https://python-chess.readthedocs.io/) (cross-checks, analysis scripts) ·
[Mermaid](https://mermaid.js.org/) (diagrams)

## License and contact

License: [MIT](LICENSE) · Author: Giorgio Paoli · Contact: [paoli.2205754@studenti.uniroma1.it](mailto:paoli.2205754@studenti.uniroma1.it)
