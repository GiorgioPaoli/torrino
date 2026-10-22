# Torrino

**A UCI chess engine written from scratch in C++17 — bitboards, alpha-beta search, and a hand-crafted tapered evaluation, developed and measured version by version (≈ Stockfish `UCI_Elo` 2000 at 10+0.1).**

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey)
![Lichess bot](https://img.shields.io/badge/Lichess-TorrinoBOT-black)
<!-- TODO: add a license badge once LICENSE exists, e.g. ![License: MIT](https://img.shields.io/badge/license-MIT-green) -->


<!-- HERO: a GIF of a TorrinoBOT win. Lichess exports any game as a GIF:
     https://lichess1.org/game/export/gif/<GAME_ID>.gif  -> save as docs/images/hero.gif and uncomment. -->
<!-- <p align="center"><img src="docs/images/hero.gif" width="360" alt="TorrinoBOT winning a game on Lichess"></p> -->
<p align="center"><img src="docs/gifs/hero.gif" width="360" alt="TorrinoBOT winning a game on Lichess"></p>
<!-- > 🎞️ *Placeholder — GIF of a TorrinoBOT win on Lichess.* -->

Torrino plays online as **[TorrinoBOT](https://lichess.org/@/TorrinoBOT)** (bullet 1963, blitz 1751 as of October 22, 2026).
Every change is first verified for correctness and then measured in engine-vs-engine matches;
the complete log of changes and results is in **[VERSIONS.md](VERSIONS.md)**.

---

## Highlights

- **Measured, incremental development.** Each version was tested in a 100-game match against the previous one
  (cutechess-cli, balanced openings, colours swapped). Cumulative gain from v3 to v10: several hundred Elo;
  the latest step, **v10 vs v9: +135 ± 69 Elo** (95% CI, 62 wins, 25 losses, 13 draws).
- **Correctness by construction.** Move generation matches the reference **perft** counts on the 6 standard test
  positions; every evaluation change is checked for colour symmetry; speed optimisations are checked to leave the
  search unchanged (identical node counts at fixed depth).
- **Evaluation driven by real games.** Weaknesses found in Lichess games (king safety, passed pawns, unconverted
  endgames) were reproduced as test positions, fixed, and re-measured: v10 gained **+135 Elo** over v9 while searching
  *less* deep (5.2 vs 5.5 plies on average).
- **Strength against an external reference:** 55.5% (+38 ± 67 Elo) against Stockfish limited to `UCI_Elo` 2000
  in 100 games at 10+0.1.

---

## Table of contents
[Quick start](#quick-start) · [Architecture](#architecture) · [Technical deep-dive](#technical-deep-dive) ·
[Results](#results) · [Testing and validation](#testing-and-validation) · [Case studies](#case-studies) ·
[Limitations and future work](#limitations-and-future-work) · [What I learned](#what-i-learned) ·
[About](#about) · [References](#references)

---

## Quick start

Requirements: a C++17 compiler (developed with g++ / MinGW-w64 on Windows).

```bash
git clone https://github.com/GiorgioPl/chess-engine.git
cd chess-engine/src
g++ -O3 -march=native -flto -static -std=c++17 *.cpp -o ../bin/torrino.exe
../bin/torrino.exe
```
<!-- TODO: replace with a Makefile or CMakeLists.txt so the build is one command, and test it on a clean machine. -->

Example UCI session (input lines, then the engine's output):
```text
uci
position startpos moves e2e4 e7e5
go movetime 2000
info depth 7 score cp 35 nodes 2847000 nps 1300000 time 2190 pv e2e4
bestmove ...
```
Extra commands: `go depth N`, `eval` (static evaluation and game phase of the current position).
Works with any UCI GUI (cutechess, En Croissant, Arena) and with
[lichess-bot](https://github.com/lichess-bot-devs/lichess-bot).

---

## Architecture

| Module | Files | Role |
|---|---|---|
| Board representation | [`board`](src/board.cpp), [`types.h`](src/types.h), [`bitboard.h`](src/bitboard.h) | bitboards + mailbox; `makeMove`/`unmakeMove`; Zobrist key and position history |
| Attack tables | [`attacks`](src/attacks.cpp) | precomputed pawn/knight/king attacks; sliding-piece attacks (ray loops) |
| Move generation | [`movegen`](src/movegen.cpp) | pseudo-legal generation, legality check, move ordering (MVV-LVA, killers) |
| Search | [`search`](src/search.cpp) | negamax alpha-beta, quiescence, iterative deepening, time management |
| Evaluation | [`eval`](src/eval.cpp) | tapered PST, king safety, pawn structure, mop-up |
| Protocol | [`uci`](src/uci.cpp) | UCI loop, time-control parsing, `info` output |
| Testing | [`perft`](src/perft.cpp), [`tools/`](tools/) | perft; scripts for PGN statistics and charts |

<!-- OPTIONAL: an architecture diagram (e.g. Mermaid) can replace or complement this table. -->

---

## Technical deep-dive

Each subsection follows *what → why → result*. All Elo figures are 100-game cutechess matches with a 95% confidence
interval; see [VERSIONS.md](VERSIONS.md) for every match.

### Board representation and move generation
- **What:** 12 bitboards (one per piece type and colour) plus a 64-square mailbox; pseudo-legal generation followed by
  a legality check; incremental `makeMove`/`unmakeMove` (no board copies).
- **Why:** bitboards make attack and pawn-structure computations a few bitwise operations.
- **Result:** perft matches the reference counts on all 6 standard positions (table below); ~7–8 M nodes/s in perft
  (single thread). Sliding-piece attacks still use ray loops — the main speed bottleneck (see limitations).

### Search
| Technique | What it does | Measured effect |
|---|---|---|
| Alpha-beta (negamax) + iterative deepening | prunes provably irrelevant branches; reuses the previous best move | baseline (v2–v3) |
| **Quiescence search** | at the horizon, keeps searching captures until the position is quiet (stand-pat) | **+195 ± 50 Elo** (v4 vs v3) |
| **Killer moves** | tries quiet moves that caused a cutoff at the same ply first | **+59 ± 34 Elo**; 3.8× fewer nodes at fixed depth on 6 test positions (21× in the start position) |
| Check extension + mate scores | searches one ply deeper in check; reports `mate N` | +31 ± 39 Elo (not significant at 95%, LOS 94%) |
| **Zobrist hashing + repetition detection** | 64-bit incremental position keys; a repeated position scores as a draw | **+67 ± 48 Elo**; draw rate vs the previous version 67% → 51% |
| Time management (soft/hard limits) | ideal time vs deadline; does not start a depth that cannot finish | +60 ± 55 Elo (v8), +49 ± 66 (v9, not significant at 95%) |

### Evaluation (v10, developed in three measured steps)
| Step | What | Why (the weakness it fixes) | Elo vs previous step |
|---|---|---|---|
| v10.1 | **Tapered evaluation** (middlegame/endgame blend by game phase) + **mop-up** | won endgames were not converted (KR vs K drawn by the fifty-move rule) | **+123 ± 59** |
| v10.2 | **King safety**: castling rights, pawn shield, exposed king, attackers near the king | the engine opened its own king (case study 1) | **+53 ± 60** (LOS 95.8%) |
| v10.3 | **Pawn structure**: passed (rank-dependent), isolated, defended, doubled pawns | passed pawns ignored (case study 2) | **+100 ± 65** |
| **v10** | all three | — | **+135 ± 69 vs v9** (direct match) |

The sum of the three steps (+276) overestimates the direct result (+135): gains measured against the immediately
previous version do not add up linearly, so the direct match is the reported figure.

---

## Results

### Elo gained by each version

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/elo_per_version_dark.png">
  <img alt="Elo gained by each version against the previous one, with 95% intervals" src="docs/images/elo_per_version_light.png">
</picture>

Every version improved on the previous one; the largest single gains came from quiescence search (v4) and from the
evaluation rework (v10). v6 and v9 are positive but their intervals include zero.

### Search depth: stronger without searching deeper

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/depth_per_version_dark.png">
  <img alt="Average search depth per version in 10+0.1 games" src="docs/images/depth_per_version_light.png">
</picture>

Average depth stayed between 5.0 and 5.5 plies across versions, while strength increased by several hundred Elo:
most of the gains came from search *quality* (move ordering, quiescence, repetition handling) and evaluation, not from
depth. v10.2's king-safety terms cost about 0.3 plies (slower evaluation) and still gained +53 Elo.
Data: ~112,000 engine moves from the cutechess game records ([`tools/pgn_depth_stats.py`](tools/pgn_depth_stats.py)).

### Against Stockfish (limited strength)

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/vs_sf2000_dark.png">
  <img alt="Score against Stockfish UCI_Elo 2000, version by version" src="docs/images/vs_sf2000_light.png">
</picture>

Against an external engine the progress is smaller than in self-play: v6–v10 all score 53–56% against Stockfish 2000,
within each other's error margins. Limited Stockfish loses mainly through tactical errors, which deeper search
exploits better than a refined evaluation — this motivates the next versions (faster move generation, transposition table).

### Lichess

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="docs/images/lichess_rating_dark.png">
  <img alt="TorrinoBOT daily Lichess rating" src="docs/images/lichess_rating_light.png">
</picture>

Bullet rating rose from 1808 (v4) to 1963 (v10) in six days of play against other bots and humans.
Online ratings are noisy (opponent pool, network latency, time controls) and are reported as a complement to the
controlled matches above, not as a substitute.

### Node throughput (NPS) and time to depth

<!-- PLACEHOLDER: NPS and time-to-depth for every version (bin/engine_v*.exe) on the same positions,
     measured with nothing else running. Planned chart: docs/images/nps_per_version_*.png -->
> 📊 *Placeholder — NPS and time to reach depth 6 for each version, measured on the same positions on an idle machine.*

Current figure (v10, single thread, Intel i5 10th gen laptop): ~1.0–1.3 M nodes/s in the middlegame.

### Basic endgames: v9 vs v10.1

| Position (self-play, 0.2 s/move) | v9 | v10.1 |
|---|---|---|
| KR vs K, king in the centre | draw (fifty-move rule) | **mate in 14** |
| KR vs K, king on d5 | draw (fifty-move rule) | **mate in 14** |
| KQ vs K, king in the centre | mate in 10 | **mate in 12** |
| KQ vs K, king on c5 | draw (fifty-move rule) | **mate in 11** |

In the 100-game match v10.1 vs v9, **v9 drew 6 games while a queen or a rook up** (KQ vs K ×4, KR vs K ×1,
two bishops vs K ×1); v10.1 never failed to convert such an advantage.

---

## Testing and validation

| Check | Method | Status |
|---|---|---|
| Move generation | perft on the 6 standard positions (table below) | ✅ all match |
| Zobrist keys | incremental key compared with the key recomputed from scratch at every perft node | ✅ |
| Repetition history | history balanced after every `unmakeMove`; 8 repetition test cases | ✅ |
| En passant edge cases | perft cross-checked against [python-chess](https://python-chess.readthedocs.io/) | ✅ |
| Evaluation | colour-mirrored positions must evaluate identically | ✅ on every change |
| Speed optimisations | identical node count at fixed depth before/after | ✅ |
| Strength | 100-game matches vs the previous version + Stockfish reference | every version |
| Regression positions | critical positions from lost Lichess games re-run after each change | case studies below |
<!-- TODO (future): automated tests (e.g. GitHub Actions running perft), sanitizers under Linux/WSL. -->

| Perft position | Depth | Nodes |
|---|---|---|
| Start position | 5 | 4,865,609 |
| Kiwipete | 4 | 4,085,603 |
| Position 3 | 5 | 674,624 |
| Position 4 | 4 | 422,333 |
| Position 5 | 4 | 2,103,487 |
| Position 6 | 4 | 3,894,594 |

---

## Case studies

Weaknesses found in real games, reproduced as test positions, and fixed. Red arrow: move played; green arrow: a better move.

<table>
<tr>
<td width="50%" valign="top">

**1. Opening its own king** (Lichess, 3+1)<br>
A pawn up (Stockfish +4.2), v9 played **39.f4??** and later **41.f5??**, walking its king to h3: the evaluation had
no notion of king safety. Stockfish's evaluation dropped to +0.2 (green: Stockfish's Rg1, keeping the pawns in front of the king).<br>
**Fix:** v10.2 (king safety) — in the same position v10.2 plays Kg1.

</td>
<td width="50%"><img src="docs/images/case_king_safety.svg" alt="Position before 39.f4 in TorrinoBOT vs GroshBoss"></td>
</tr>
<tr>
<td width="50%" valign="top">

**2. Ignoring a passed pawn** (Lichess, 1+1, Torrino is Black)<br>
At +4.4, v10.1 moved its rook off the h-file (**27...Rf6?**) to win two pawns; White's h-pawn ran to h7 and the game
ended in a draw by perpetual check (green: Stockfish's a6, keeping the rook on the h-file).<br>
**Fix:** v10.3 (passed pawns) — the engine keeps the blockade (27...Nb6 at depth 8).

</td>
<td width="50%"><img src="docs/images/case_passed_pawn.svg" alt="Position before 27...Rf6 in maia9 vs TorrinoBOT"></td>
</tr>
<tr>
<td width="50%" valign="top">

**3. A queen up, no mate** (cutechess, v10.1 vs v9)<br>
v9 kept an extra queen for 192 moves and drew by the fifty-move rule: without an endgame plan, the king never
approached the edge.<br>
**Fix:** v10.1 (tapered evaluation + mop-up) — basic mates converted in 11–14 moves.

</td>
<td width="50%"><img src="docs/images/case_kq_vs_k.svg" alt="Final position of a KQ vs K game drawn by the fifty-move rule"></td>
</tr>
</table>

Other bugs found through testing: an uninitialised en passant square causing random crashes (found when a cutechess
match stopped on an engine crash); a time-management rule that ended 30+1 games with more time than at the start
(found by extracting thinking times from the game records).

---

## Limitations and future work

**Limitations**
- **Search depth** of about 5–6 plies at 10+0.1 and 7–9 in blitz: tactics beyond the horizon are missed.
- **No transposition table**: positions reached by different move orders are searched again
  (in KR vs K, depth 9 needs ~7 M nodes).
- **Sliding-piece attacks by ray loops**: ~1.0–1.3 M nodes/s in the middlegame.
- **No mobility / piece-activity terms**; hand-set evaluation weights (not tuned from data).
- Single-threaded; no opening book; no endgame tablebases.

**Next steps**
1. **Magic bitboards** — table-lookup sliding attacks (move generation, check detection, evaluation).
2. **Transposition table** — reuse search results across transpositions; better move ordering.
3. **Pruning and reductions** — history heuristic, null-move pruning, late move reductions (LMR).
4. **Mobility and piece activity** — cheap once attacks are table lookups.
5. **Evaluation tuning from data** — Texel tuning on positions from the engine's own games.

---

## What I learned

<!-- TODO (personal, 3-4 lines, in your own words). Possible directions, only if true for you:
     - why chained self-play Elo overestimates progress, and why a direct match is needed;
     - measuring before optimising (e.g. time management v9: the first attempt ended games with unused time);
     - correctness tools (perft, symmetry tests, identical node counts) caught bugs that games alone would not show;
     - a faster but dumber engine is not stronger: v10 searches less deep than v9 and wins 68.5% of games. -->
*Placeholder.*

---

## About

<!-- TODO (personal): who you are, why you built this engine, what you were aiming to learn.
     Be transparent about the development process and tools, e.g. an AI assistant (Claude) used as a tutor and
     code reviewer, for test/plot scripts, and for some functions written on request (name them). -->
*Placeholder.*

---

## References

- [Chess Programming Wiki](https://www.chessprogramming.org/) — algorithms, perft positions, evaluation ideas.
- [UCI protocol](https://www.shredderchess.com/chess-features/uci-universal-chess-interface.html).
- [cutechess](https://github.com/cutechess/cutechess) (matches), [lichess-bot](https://github.com/lichess-bot-devs/lichess-bot) (online play),
  [Stockfish](https://stockfishchess.org/) (reference opponent and game analysis), [python-chess](https://python-chess.readthedocs.io/) (cross-checks, board diagrams).

## License and contact

<!-- TODO: add a LICENSE file (e.g. MIT). -->
License: TODO · Author: Giorgio Paoli · Contact: TODO
