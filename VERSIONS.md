# Versions

## v4 - quiescence search
Commit: tag `v4`
Changes: Added Quiescence search, eval with piece development, fixed en passant

### Local tests (cutechess)
| Opponent   | TC      | Games   | Results W-L-D | Elo        |
|------------|---------|---------|---------------|------------|
| v3fix      | 10+0.1  | 100     | 53-2-45       | +195 ± 50  |
| SF1500     | 10+0.1  | 100     | 78-19-3       | +235 ± 86  |
| SF1750     | 10+0.1  | 100     | 56-38-6       | +63 ± 68   |
| SF1900     | 10+0.1  | 100     | 51-39-10      | +42 ± 66   |
| SF1900     | 180+2   | 16      | 13-3-0        | +255 (?)   |

#### Conclusions
almost at the same level as Stockfish limited at 1900

### Lichess (bot: TorrinoBOT)
Online from: 17-10-2026  to: 18-10-2026
| Category  | Initial Rating  | Final Rating  | Games   | Results W-L-D |
|-----------|-----------------|---------------|---------|---------------|  
| Bullet    | 1500?           | 1869 (+369)   | 57      | 31 - 12 - 14  |
| Blitz     | 1500?           | 1663 (+163)   | 28      | 9 - 11 - 8    | 

Note: concurrency 3 (1 for humans), move_overhead 1000



## v5 - killer moves
Commit: tag `v5`
Changes: Added Killer Moves (quiet, 1 and 2)

### Local tests (cutechess)
| Opponent                  | TC        | Games | Results W-L-D | Elo                               |
|---------------------------|-----------|-------|---------------|-----------------------------------|
| v4                        | 10+0.1    | 100   | 22 - 5 - 73   | +59 ± 34                          |
| SF1900                    | 10+0.1    | 100   | 44 - 42 - 14  | +7 ± 63                           |
| v4 + SF1900 (round-robin) | 10+0.1    | 150   |               | v5 +45, v4 +10, SF1900 -56 (±55)  | conclusion: v5 > v4 > SF1900
| SF2000                    | 10+0.1    | 100   | 37 - 55 - 8   | -63.2 ± 67.11                     |

#### Conclusions
Better than v4, with +59 ± 34 Elo, not enough games to say anything about SF1900

### Lichess (bot: TorrinoBOT)
Online from: 18-10-2026  to: 19-10-2026
| Category  | Initial Rating  | Final Rating  | Games   | Results W-L-D |
|-----------|-----------------|---------------|---------|---------------|
| Bullet    | 1869            | 1848 (-21)    | 38      | 16 - 12 - 10  |
| Blitz     | 1663            | 1704 (+41)    | 25      | 9 - 6 - 10    |




## v6 - check extension
Commit: tag `v6`
Changes: Added check extensions for finding mates quicker and mate scores 

### Local tests (cutechess)
| Opponent                  | TC        | Games | Results W-L-D | Elo                               |
|---------------------------|-----------|-------|---------------|-----------------------------------|
| v5                        | 10+0.1    | 100   | 21 - 12 - 67  | +31 ± 39                          |
| SF2000                    | 10+0.1    | 100   | 50 - 43 - 7   | +24 ± 66                          |

#### Conclusions
Better than v5, with +31 ± 39 Elo, likelihood of superiority is 94.1%




## v7 - Zobrist hashing + repetition detection
Commit: tag `v7`
Changes:
- Zobrist hashing: 64-bit position key (pieces, side to move, castling rights,
  en passant file), updated incrementally in makeMove and restored in unmakeMove
- Key history in Board, updated by makeMove/unmakeMove
- Repetition detection in negamax (same side to move, bounded by halfMoveClock): score 0
- 50-move rule: score 0 after 100 plies without captures or pawn moves
- En passant square set only when a capture is actually possible (consistent keys)

### Local tests (cutechess)
| Opponent | TC     | Games | Result       | Elo       |
|----------|--------|-------|--------------|-----------|
| v6       | 10+0.1 | 100   | 34 - 15 - 51 | +67 ± 48  |
| SF2000   | 10+0.1 | 100   | 50 - 44 - 6  | +20 ± 67  | (5 wins by SF timeout: PC overloaded, concurrency = 4)

Notes: draw rate vs previous version dropped from 67% (v6 vs v5) to 51%.
Many remaining draws are v7 deliberately accepting repetitions in worse positions,
since v6 still repeats blindly.

### Lichess (bot: TorrinoBOT)
Online from: 19-10-2026   to: 20-10-2026
| Category | Starting rating | Final rating | Games | Results      |
|----------|-----------------|--------------|-------|--------------|
| Bullet   | 1848            | 1894 (+46)   | 61    | 26 - 22 - 13 |
| Blitz    | 1704            | 1748 (+44)   | 25    | 15 - 9 - 1   |

Notes: opening book off (same as previous versions), concurrency 2 (1 reserved for humans)



## v8 - Time Management 
Commit: NO COMMIT, testing
Changes: 
- Changed the time management sistem, added a soft stop and a hard stop used as deadline
- Soft limit = timeLeft / 25 + 3/4 of the increment; hard limit = min(3 × soft, timeLeft / 4),
  never more than timeLeft - 100 ms
- Iterative deepening does not start a new depth after half of the soft limit,
  but finishes the current depth as long as it stays under the hard limit
- If there is only one legal move, it is played immediately without searching
- Iterative deepening stops when there are no legal moves (no more fake "h0h0" info lines)

### Local tests (cutechess)
| Opponent | TC     | Games | Results W-L-D | Elo       |
|----------|--------|-------|---------------|-----------|
| v7       | 10+0.5 | 100   | 40 - 23 - 37  | +60 ± 55  |
| SF2000   | 10+0.1 | 100   | 51 - 42 - 7   | +31 ± 67  | (3 wins by SF timeout)


#### Conclusions
Better than v7, with +60 ± 55 Elo, likelihood of superiority 98.4%.
Draw rate dropped again: 37% (v7 vs v6: 51%, v6 vs v5: 67%).
The type of draws changed: 17 by repetition (49 in v7 vs v6), 9 by the fifty-move rule
and 11 by insufficient material. More games reach endgames that neither engine knows
how to win, which is the motivation for v9 (endgame evaluation).




## v9 - Time Management 2
Commit: tag `v9`
Changes:
- Estimate of the next depth's cost: factor = time(depth d) / time(depth d-1), clamped
  between 2 and 10; a new depth starts only if it is expected to finish within the hard limit
- Interrupted depths are no longer discarded: if at least one root move was fully searched,
  the best move found so far is used (the first root move is always the previous best,
  so any other move can only replace it by scoring better)
- Soft limit = timeLeft / 20 + increment; hard limit = min(2 × soft, timeLeft / 4)

### Local tests (cutechess)
| Opponent         | TC   | Games | Results W-L-D | Elo       |
|------------------|------|-------|---------------|-----------|
| v8 (1st attempt) | 30+1 | 50    | 14 - 18 - 18  | -28 ± 78  |
| v8 (final)       | 30+1 | 50    | 15 - 8 - 27   | +49 ± 66  |

#### Time usage (from the PGN, 30+1)
| Engine           | Time/move, moves 1-30 | After move 30 | Avg depth | Clock left at end |
|------------------|-----------------------|---------------|-----------|-------------------|
| v8               | 1.96 s                | 1.09 s        | 7.12      | 13.8 s            |
| v9 (1st attempt) | 1.07 s                | 1.02 s        | 7.35      | 35.9 s            |
| v9 (final)       | 1.86 s                | 1.07 s        | 7.10      | 15.5 s            |

#### Conclusions
The 1st attempt (stop if the next depth didn't fit in the soft limit, soft = timeLeft/25 + 3/4 inc)
was too conservative: it ended games with more time than it started with.
The final version uses about the same time as v8 but without wasting interrupted depths:
+49 ± 66 Elo, likelihood of superiority 92.8%, no losses on time.
Known issue: at 3+1 the formula is too front-loaded (10-15 s per move in the middlegame).
 - To be adjusted (soft = timeLeft/30 + inc, hard ≤ timeLeft/6).


### Lichess (bot: TorrinoBOT)
Online from: 20-10-2026   to: 21-10-2026
| Category | Starting rating | Final rating | Games | Results W-L-D |
|----------|-----------------|--------------|-------|---------------|
| Bullet   | 1894            | 1948 (+54)   | 63    | 33 - 21 - 9   |
| Blitz    | 1748            | 1753 (+5)    | 26    | 9 - 12 - 5    |

Notes: opening book off (same as previous versions), concurrency 2 (1 reserved for humans)



## v10 - Evaluation rework
Commit: tag `v10`
Goal: fix weaknesses seen in Lichess games (won endgames not converted, king safety ignored,
passed pawns not pushed or not stopped). Developed in three steps, each tested against the previous one.

### Steps overview
| Step  | Changes                              | vs    | TC     | Games | Results W-L-D | Elo       |
|-------|--------------------------------------|-------|--------|-------|---------------|-----------|
| v10.1 | Tapered eval (game phase) + mop-up   | v9    | 10+0.1 | 100   | 51 - 17 - 32  | +123 ± 59 |
| v10.2 | King safety                          | v10.1 | 10+0.1 | 100   | 45 - 30 - 25  | +53 ± 60  |
| v10.3 | Pawn structure                       | v10.2 | 10+0.1 | 100   | 56 - 28 - 16  | +100 ± 65 |
| v10   | Final version                        | v9    | 10+0.1 | 100   | 62 - 25 - 13  | +135 ± 69 |
| v10   | Final version                        | SF2000| 10+0.1 | 100   | 52 - 41 - 7   | +38 ± 67  |


### v10.1 - Tapered evaluation + mop-up
- Game phase from remaining pieces (N=1, B=1, R=2, Q=4; 24 = opening, 0 = endgame);
  middlegame and endgame scores blended by phase
- King: MG table in the middlegame, EG table (centralisation) in the endgame
- Development and early-queen penalties: middlegame only
- Mop-up (endgame, piece-material advantage ≥ 400): push the losing king to the corner,
  bring the kings together

| Opponent | TC     | Games | Results W-L-D | Elo       |
|----------|--------|-------|---------------|-----------|
| v9       | 10+0.1 | 100   | 51 - 17 - 32  | +123 ± 59 |

Evaluation unchanged in the opening/middlegame, symmetric on all test positions.
Remaining draws vs v9: 13 by the fifty-move rule, 11 by insufficient material, 8 by repetition.
Known limit: aimless shuffling in roughly equal endgames (no plan beyond the search depth).

### v10.2 - King safety (middlegame only)
- Castling: bonus per castling right while the king is on its start square,
  penalty if the king moved without castling (castled = g/h or b/c file on the back rank)
- Pawn shield: for a king castled on a wing, bonus for each pawn on the 2nd/3rd rank of the
  three files in front of it, penalty for each missing one
- Exposed king: penalty per rank beyond the 2nd while the enemy queen is on the board
- King attack: enemy pieces attacking the 8 squares around the king (N=2, B=2, R=3, Q=5);
  with 2+ attackers, penalty = weight²

Checks:
- Start position still 0, symmetric on all test positions
- Castled king vs king on f1 without castling: 60 cp apart (was 20 in v10.1)
- GroshBoss position before 39.f4: v10.1 plays f3f4 (the game blunder), v10.2 plays Kg1
- Speed: ~30% fewer nodes per second (kiwipete 1.42M → 0.99M nps), same node count per depth

| Opponent | TC     | Games | Results W-L-D | Elo       |
|----------|--------|-------|---------------|-----------|
| v10.1    | 10+0.1 | 100   | 45 - 30 - 25  | +53 ± 60  |

Result: +53 ± 60 Elo vs v10.1 (LOS 95.8%) despite ~30% lower NPS.
The new funcionts in v10.2 call many tames the slow attack tables, and they are computed every time 
they're called, making the NPS ~30% slower, but the loss is balanced by the better search.

### v10.3 - Pawn structure
- Passed pawns: bonus by relative rank, much larger in the endgame
  (mg 5 → 70, eg 10 → 150 from the 2nd to the 7th rank); precomputed masks (initPawnMasks)
- Isolated pawns: penalty (mg 10, eg 15)
- Defended pawns (pawn chains): bonus per pawn defended by another pawn (mg 5, eg 8)
- Doubled pawns moved into the same function (pawnStructure)

| Opponent | TC     | Games | Results W-L-D | Elo       |
|----------|--------|-------|---------------|-----------|
| v10.2    | 10+0.1 | 100   | 56 - 28 - 16  | +100 ± 65 |


### Conclusions
Direct result: v10 vs v9 = +135 ± 69 (the sum of the three steps, +276, overestimates the
total: each step was measured against its predecessor). Against SF2000: 55.5% (+38 ± 67),
similar to v6-v8: the evaluation gains show mostly against similar engines; against Stockfish
limited, which loses mostly through tactical errors, search depth matters more (v11, v12).
Mobility and piece activity postponed until after magic bitboards (v11): they need slider
attacks for every piece and would be too slow with the current attack functions.

### Lichess (bot: TorrinoBOT)
Online from: 22-10-2026   to: 23-10-2026
| Category | Starting rating | Final rating | Games | Results W-L-D |
|----------|-----------------|--------------|-------|---------------|
| Bullet   | 1948            | 2007 (+57)   | 81    | 45 - 30 - 16  |
| Blitz    | 1753            | 1771 (+18)   | 31    | 15 - 13 - 4   |



## v11 - Speed
Commit: branch `v11-speed`; tag `v11` (= v11.3)
Goal: more nodes per second without changing the search, to recover the speed lost in v10
(v9 → v10: -32% NPS) and reach deeper in the same time. Every step must leave the search tree
unchanged (same nodes and same best move at fixed depth). Developed in steps, like v10.

### Steps overview
| Step  | Changes                                     | NPS     | vs previous step | vs v10 | Depth 7 time | Match vs v10             |
|-------|---------------------------------------------|---------|------------------|--------|--------------|--------------------------|
| v10   | (baseline)                                  | 1.77 M  | -                | -      | 16.8 s       | -                        |
| v11.1 | Magic bitboards for sliding-piece attacks   | 2.33 M  | +32%             | +32%   | 12.7 s       | -                        |
| v11.2 | Captures-only move generation in quiescence | 2.86 M  | +23%             | +62%   | 10.4 s       | 57 - 20 - 23, +135 ± 64  |
| v11.3 | Lazy move ordering                          | 3.10 M  | +8%              | +76%   | 9.6 s        | 54 - 24 - 22, +108 ± 63  |

Speed: one run with all four engines (2026-10-24), `go depth 7` on the 6 standard positions (start, kiwipete,
pos3-pos6), fastest of 10 runs, the order of the engines rotated every run, nothing else running;
NPS = total nodes / total time, depth 7 time = sum over the 6 positions. With median times instead of the
fastest runs: v10 1.66M, v11.1 2.17M, v11.2 2.70M, v11.3 2.87M nps (v11.3 vs v10 +73%).
Node counts and best moves in the same run: v11.1 = v10 and v11.3 = v11.2 exactly; v11.2 within 0.1% of v11.1.
Per-position table: `docs/notes/BENCH_v11.md`; raw data: `docs/data/bench_v11_all.csv` (earlier per-step runs
in `BENCH_v11.1.md`, `BENCH_v11.2.md`, `BENCH_v11.3.md`).
v11.1 has the same search as v10 and v11.3 the same search as v11.2, so they were checked with node counts and
speed tests instead of matches.


### v11.1 - Magic bitboards
- Bishop, rook and queen attacks are one table lookup instead of walking the rays square by square:
  `occupancy & mask`, multiply by a magic number, shift, read the precomputed attacks
- Relevant masks per square (rookRelevantMask, bishopRelevantMask): the lines of the piece without the board
  edges (rook up to 12 squares, bishop up to 9)
- Magic numbers found once by a separate program (`tools/find_magics.cpp`, random search with a fixed seed) and
  hardcoded in `src/magic_numbers.h`; tables filled at startup by initMagics(). The old ray loops (*Slow) are kept
  only to fill the tables and as a reference for tests
- Used everywhere: move generation, isSquareAttacked, king attack in the evaluation

Checks: new lookups vs old ray loops on 38.4 million random occupancies (all squares, rook/bishop/queen),
0 mismatches; perft correct on the 6 positions; same nodes and best moves as v10 at depth 6 and 7.

Result: +32% NPS. Largest gain in rook endgames (pos3 +54%), smallest in the start position (+27%), where the
sliders are blocked by their own pawns. Less than the 1.5-2.5x expected: slider attacks were only part of the
cost per node.


### v11.2 - Captures-only move generation in quiescence
- Quiescence generated every pseudo-legal move, scored and sorted all of them, then skipped the quiet ones.
  Now generateCaptures() produces only the moves it searches: captures, en passant, queen promotions
- No duplicated code: each piece generator takes a `capturesOnly` flag (pieces target only enemy pieces, pawns
  skip pushes and under-promotions, the king skips castling); generatePseudoLegalMoves() passes `false`

Checks:
- Perft correct on the 6 positions; same best moves as v11.1, nodes within 0.1% (kiwipete depth 7: 9,523,360
  vs 9,524,202). A test build filtering moves before sorting gives exactly v11.2's nodes: same move set, only
  the order of equal-score captures in the sort changed
- Bug found while testing: the four piece generators used `occupiedByColor[us]` instead of `[them]` in
  captures-only mode, so quiescence saw only pawn and king captures (kiwipete depth 1: +155 Qxf6 instead of +90).
  The node count exposed it at once (6.95M instead of 9.52M)

Result: +23% NPS over v11.1, largest in tactical middlegames and endgames (kiwipete +23%, pos3 and pos6 +27%).

#### Local tests (cutechess)
| Opponent | TC     | Games | Results W-L-D | Elo        |
|----------|--------|-------|---------------|------------|
| v10      | 10+0.1 | 100   | 57 - 20 - 23  | +135 ± 64  |

LOS 100%. As White 29 - 7 - 14, as Black 28 - 13 - 9. No losses on time.
Draws: 12 by repetition, 9 by insufficient material, 2 by the fifty-move rule.

Average depth in the match (from the PGN, `tools/pgn_depth_stats.py`):
| Engine | Moves 10-40 | All moves | Time/move |
|--------|-------------|-----------|-----------|
| v10    | 5.07        | 5.55      | 0.236 s   |
| v11.2  | 5.41        | 5.86      | 0.233 s   |

About +0.3 plies at the same time per move, as expected from the tree growing ~5.7x per ply
(log 1.65 / log 5.7 ≈ 0.3). The Elo gain is larger than the +30-70 expected for 1.65x speed; the interval is
wide, so the true value may be lower, but it is clearly positive. Plausible reason: with the same knowledge,
v10's evaluation profits a lot from 0.3 extra plies at 10+0.1.


### v11.3 - Lazy move ordering
- Before: orderMoves() sorted the whole move list (selection sort) before the loop, at every node, although
  most nodes stop after the first or second move (beta cutoff)
- Now: scoreMoves() scores every move once, and pickMove() runs one pass of the same selection sort at the start
  of each loop step. The sorting stops when the loop stops. Used in negamax and quiescence, not at the root

Checks: same moves in the same order as v11.2, so node counts and best moves are exactly equal on all 6 positions
(kiwipete depth 7: 9,523,360, e2a6); perft not affected.

Result: +8% NPS over v11.2 (+6% with median times): small, as sorting was a small part of the cost per node,
and close to the noise on single positions (start +2%, pos3 -1%, kiwipete +11%).

Depth reached in a fixed time (`go movetime 3000`, best of 3 runs):
| Engine | start | kiwipete | pos3 | pos4 | pos5 | pos6 |
|--------|-------|----------|------|------|------|------|
| v10    | 7     | 6        | 8    | 7    | 7    | 6    |
| v11.3  | 8     | 6        | 8    | 8    | 7    | 6    |

One more ply in 2 of 6 positions, never fewer; in the others the next ply still does not fit in 3 s
(kiwipete depth 7: ~3.6 s for v11.3, ~6.6 s for v10).

#### Local tests (cutechess)
| Opponent | TC     | Games | Results W-L-D | Elo        |
|----------|--------|-------|---------------|------------|
| v10      | 10+0.1 | 100   | 54 - 24 - 22  | +108 ± 63  |

LOS 100%. As White 31 - 9 - 10, as Black 23 - 15 - 12. No losses on time.
Draws: 11 by repetition, 7 by insufficient material, 3 by the fifty-move rule, 1 by stalemate.

Average depth in the match (from the PGN, `tools/pgn_depth_stats.py`):
| Engine | Moves 10-40 | All moves | Time/move |
|--------|-------------|-----------|-----------|
| v10    | 5.13        | 5.47      | 0.222 s   |
| v11.3  | 5.45        | 5.80      | 0.221 s   |

+108 ± 63 is lower than the +135 ± 64 of v11.2 vs v10, but the two results are well inside each other's
margins (the difference, 27 Elo, is less than half of either interval), so it is noise, not a regression:
v11.3 searches exactly the same tree as v11.2 and is 8% faster (benchmark above), so it should not play weaker
at the same time per move. 8% more speed is worth only about 0.05 plies, too little to see in 100 games:
the average depth gain over v10 is the same as v11.2's (+0.32 vs +0.34 plies in moves 10-40).


### Conclusions
+76% NPS since v10 with the same evaluation and the same search: depth 7 on the 6 positions takes 9.6 s
instead of 16.8 s (-43%), and v11 is now faster than v9 while keeping the full v10 evaluation.
Strength vs v10 (100 games each at 10+0.1): v11.2 +135 ± 64, v11.3 +108 ± 63. Since v11.2 and v11.3 search
the same tree and v11.3 is faster, the two matches measure the same thing; together (200 games, 111 - 44 - 45)
they give about **+121 ± 45 Elo for v11 over v10**, coming entirely from speed (+0.3 plies at the same time).
Final v11 = v11.3 (`bin/engine_v11.exe`): the fastest build, with the strength confirmed by its own match.


### Lichess (bot: TorrinoBOT)
Online from: 23-10-2026   to: 24-10-2026
| Category | Starting rating | Final rating | Games | Results W-L-D |
|----------|-----------------|--------------|-------|---------------|
| Bullet   | 2007            | 2037 (+30)   | 76    | - - -         |
| Blitz    | 1771            | 1824 (+53)   | 40    | - - -         |


## v12 - Transposition table
Commit: tag `v12`
Goal: remember positions already searched (Zobrist key → best move, score, depth, bound) so the same depth needs
far fewer nodes. Developed in two steps, v12.1 and v12.3 (there is no separate v12.2 build).


### v12.1 - TT move ordering
- New files `tt.h` / `tt.cpp`: table of 2^20 entries (key, best move, score, depth, flag), index = key & (size - 1),
  always-replace; probeTT() checks the full 64-bit key, storeTT() overwrites the slot, clearTT() at startup and
  on `ucinewgame`
- negamax probes the table after the repetition check and the check extension; the stored best move, if it is in
  the generated list, gets the highest ordering score (30000) and is searched first
- negamax tracks its best move and stores it at the end (never after an interrupted search); scores not used yet

Checks: perft correct; same best moves and scores as v11 at depth 7.
Result: -25% nodes and -26% time to depth 7, same NPS.


### v12.3 - TT cutoffs and mate scores
- Cutoffs: when the stored depth is at least the current depth, the stored score is used directly: exact scores
  are returned, lower bounds when ≥ beta, upper bounds when ≤ alpha. The flag is set when storing (fail low →
  upper, fail high → lower, else exact, using the original alpha). The probe moved before move generation, so a
  cutoff also skips generating and scoring the moves
- Mate scores: stored relative to the node instead of the root (scoreToTT / scoreFromTT), so a mate found again
  at another ply keeps the right distance

Checks: perft correct; same best moves and scores as v11 at depth 7; mate distances stable between depths.
Bugs found while testing: the TT move was read even when the probe failed (uninitialised memory), and after the
first fix it was used only from entries deep enough for a cutoff (kiwipete depth 7: 5.0M nodes instead of 3.65M).
Both fixed.
Result: -65% nodes and -63% time to depth 7 compared with v11; basic mates (KR vs K, KQ vs K) now found.


### Speed and mates
Speed (depth 7 on the 6 standard positions, fastest of 10 runs, two searches at a time; data in `docs/data/bench_v12.csv`):
| Position | v11 nodes  | v12.1 nodes | v12.3 nodes | v11 time | v12.1 time | v12.3 time | v12.3 vs v11 (time) |
|----------|------------|-------------|-------------|----------|------------|------------|---------------------|
| start    | 1,040,373  | 788,546     | 494,818     | 0.310 s  | 0.241 s    | 0.141 s    | -55%                |
| kiwipete | 9,523,360  | 7,119,182   | 3,653,450   | 3.931 s  | 2.924 s    | 1.523 s    | -61%                |
| pos3     | 825,186    | 220,933     | 121,410     | 0.150 s  | 0.046 s    | 0.027 s    | -82%                |
| pos4     | 1,272,695  | 1,128,129   | 632,291     | 0.513 s  | 0.433 s    | 0.247 s    | -52%                |
| pos5     | 6,004,952  | 5,150,074   | 1,919,909   | 1.667 s  | 1.451 s    | 0.552 s    | -67%                |
| pos6     | 10,965,855 | 7,741,257   | 3,644,392   | 3.862 s  | 2.657 s    | 1.362 s    | -65%                |
| **Total**| **29.6M**  | **22.1M (-25%)** | **10.5M (-65%)** | **10.43 s** | **7.75 s (-26%)** | **3.85 s (-63%)** | **-63%** |

NPS is almost unchanged (v11 2.84M, v12.1 2.86M, v12.3 2.72M): the gain comes from a smaller tree, not from faster nodes.
With median times: 11.16 s → 8.33 s → 4.13 s.

Mates (`go movetime`, Stockfish depth 30 as reference):
- King and rook vs king (`8/8/8/4k3/8/8/8/R3K3 w - - 0 1`): v11 reaches depth 10 in 20 s and sees no mate;
  v12.3 reaches depth 17 in 14 s (mate 19) and depth 18 in 35 s (mate 16; Stockfish reported mate 17)
- King and queen vs king (`8/8/8/4k3/8/8/8/3QK3 w - - 0 1`): v11 reaches depth 7 in 20 s, no mate; v12.3 finds
  mate 11 at depth 12 (3.6 s) and the correct mate 7 from depth 13 on, stable up to depth 16

#### Conclusions
The transposition table cuts the tree at depth 7 by 65% and the time by 63%, with the same moves and scores (2.7x faster to the same depth, roughly half a ply to one ply more in the same time).
The TT move alone (v12.1) gives -25%; the cutoffs add the rest.
In endgames the effect is much larger (pos3 -82%, and basic mates now found at depth 13-18, which v11 never reached).
Mate distances stay consistent between depths (no wrong or growing mate scores) thanks to the v12.3 conversion.

#### Local tests (cutechess)
| Opponent | TC     | Games | Results W-L-D | Elo        |
|----------|--------|-------|---------------|------------|
| v11      | 10+0.1 | 100   | 61 - 22 - 17  | +143 ±67   | 
| SF2000   | 10+0.1 | 100   | 68 - 26 - 6   | +155 ±73   |
| SF2100   | 10+0.1 | 100   | 61 - 34 - 5   | +96  ±69   |
 
Both LOS 100%, no losses on time. vs v11: as White 31 - 14 - 5, as Black 30 - 8 - 12.
vs SF2000: 71% (v10 scored 55.5% against the same opponent). vs SF2100: 63.5%, LOS 99.7%, no losses on time.

Average depth in the matches (from the PGNs, `tools/pgn_depth_stats.py`):
| Engine        | Moves 10-40 | All moves | Time/move |
|---------------|-------------|-----------|-----------|
| v11           | 5.70        | 6.08      | 0.235 s   |
| v12 (vs v11)  | 6.31        | 6.99      | 0.234 s   |
| v12 (vs SF)   | 6.25        | 7.22      | 0.242 s   |

v12 searches about 0.6 plies deeper in the middlegame and 0.9 plies deeper over the whole game at the same
time per move, in line with the benchmark (same depth in about 1/3 of the time).

Tested at 10+0.1 like the previous versions. The benefit of the transposition table may differ at longer time controls (more transpositions at higher depth, but smaller Elo per extra ply and a fuller table); not measured.


### Lichess (bot: TorrinoBOT)
Online from: 25-10-2026 to: 25-10-2026
| Category | Starting rating | Final rating | Games |
|----------|-----------------|--------------|-------|
| Bullet   | 2037            | 2010 (-27)   | 55    |
| Blitz    | 1824            | 1872 (+48)   | 17    |

Torrino lost Elo in Bullet, it's probably just noise since it only played 29 games compared the 76 games it played during v11.



## v13 - Pruning
Commit: tag `v13`
Goal: search fewer nodes for the same depth by skipping moves that almost certainly do not matter, so the engine
reaches deeper in the same time. Steps: v13.1 PVS, v13.2 null-move pruning, v13.3 late move reductions (LMR).
The match vs v12 will be played on the final v13; every step is checked with perft, node counts and benchmarks.


### v13.1 - Principal variation search (PVS)
- In negamax the first legal move is searched with the full window; the others with a zero window
  (alpha, alpha + 1) that only proves they are not better. If one scores above alpha (and below beta), it is
  searched again with the full window. The root search is unchanged

Checks: same best moves and scores as v12.3 at depth 7.
Result: -7% nodes, -10% time to depth 7.


### v13.2 - Null-move pruning
- Before generating moves, the side to move "passes" (makeNullMove: switch side, clear en passant, update the
  hash, treated as irreversible for repetitions) and the position is searched at depth - 1 - R (R = 2) with a zero
  window around beta. If the score is still ≥ beta, the node is cut (returns beta)
- Not used when in check, at depth < 3, right after another null move (new negamax parameter allowNullMove), or
  when the side to move has only king and pawns (zugzwang; hasNonPawnMaterial)

Checks: perft correct; makeNullMove/unmakeNullMove verified at 177,905 positions (hash equal to a full
recomputation, state restored exactly, 0 errors); same best moves and scores as v12.3 at depth 7.
Result: -65% nodes and -66% time to depth 7 compared with v12.3; 1-3 plies deeper in 3 seconds.


### v13.3 - Late move reductions (LMR)
- Moves late in the ordering are searched one ply shallower (R = 1) with a zero window; if one scores above alpha
  it is searched again at full depth, then (as in PVS) with the full window if it is really better
- Only quiet moves after the first 3 legal moves, at depth ≥ 3, not killers, not when in check and not moves that
  give check; captures, promotions, killers and checks are always searched at full depth

Checks: perft correct; same best moves as v12.3 on all 6 positions at depth 7 (same scores except pos5, +456
instead of +470).
Result: -53% nodes and -46% time to depth 7 compared with v13.2; NPS -12% (a check test per quiet move and the
re-searches), more than repaid by the smaller tree.


### Speed (depth 7 on the 6 standard positions, fastest of 10 runs, one run with the three engines; details in `docs/notes/BENCH_v13.md`)
| Position | v12.3 nodes | v13.2 nodes | v13.3 nodes | v12.3 time | v13.2 time | v13.3 time | v13.3 vs v12.3 (time) |
|----------|-------------|-------------|-------------|------------|------------|------------|-----------------------|
| start    | 494,818     | 229,765     | 73,871      | 0.153 s    | 0.069 s    | 0.026 s    | -83%                  |
| kiwipete | 3,653,450   | 1,828,821   | 880,693     | 1.501 s    | 0.718 s    | 0.399 s    | -73%                  |
| pos3     | 121,410     | 56,538      | 31,789      | 0.026 s    | 0.012 s    | 0.007 s    | -72%                  |
| pos4     | 632,291     | 342,361     | 181,879     | 0.259 s    | 0.136 s    | 0.079 s    | -69%                  |
| pos5     | 1,919,909   | 393,597     | 183,799     | 0.560 s    | 0.113 s    | 0.061 s    | -89%                  |
| pos6     | 3,644,392   | 842,827     | 396,571     | 1.248 s    | 0.302 s    | 0.157 s    | -87%                  |
| **Total**| **10.47M**  | **3.69M (-65%)** | **1.75M (-83%)** | **3.75 s** | **1.35 s (-64%)** | **0.73 s (-81%)** | **-81%** |

PVS alone (v13.1, measured in an earlier run): -7% nodes, -10% time compared with v12.3.

Depth reached in 3 seconds (`go movetime 3000`, start / kiwipete / pos3 / pos4 / pos5 / pos6):
v12.3 8 / 7 / 10 / 8 / 8 / 7, v13.2 10 / 8 / 13 / 10 / 9 / 8, v13.3 11 / 9 / 15 / 11 / 11 / 8.

Mates (`go movetime 20000`): king and rook vs king, first mate after 13.0 s (v12.3, mate 19), 6.9 s (v13.2, mate 22,
then mate 16 from depth 19) and 9.2 s (v13.3, mate 17); king and queen vs king, mate 7 at depth 13 for v12.3 and
v13.2 (about 4.3 s), mate 11 at depth 15 for v13.3 (4.2 s). With LMR the quiet mating moves are searched less deeply,
so the first mate found can be longer than the shortest one.

#### Conclusions
The three pruning steps cut the tree at depth 7 by 83% and the time by 81% compared with v12, with the same best
moves: in 3 seconds the engine now reaches 1 to 5 plies deeper (depth 8-15 instead of 7-10). Null-move pruning gives
the largest part (-64%), LMR roughly halves the rest, PVS adds a little and prepares the zero-window searches.
To check in the match vs v12: whether the reductions miss tactics often enough to cost games.


### v13.4 - Time management (Lichess test, no local match yet)
Problem: at 60+1 the bot ended games with about 25 s on the clock (time it could have used to search deeper), and
`go movetime` stopped early (a 20 s search stopped after 4.2 s). The soft limit passed by `uci.cpp` was never used:
the only stop rule was "do not start the next depth unless elapsed + last iteration time × growth factor fits under
the hard limit", with the growth factor clamped to 2-10.
History: v8 used "do not start a new depth after half of the soft limit", which overspent in the middlegame; v9
replaced it with the prediction, which turned out too cautious.

New rule, after each completed depth (findBestMoveTime):
- stop if elapsed ≥ soft limit (never overspend voluntarily)
- continue if elapsed + predicted time of the next depth ≤ hard limit
- continue if elapsed < 30% of the soft limit (still early: start it anyway, the hard deadline interrupts it and the
  best move of the partial depth is kept)
- otherwise stop
Growth factor clamped to 2-6 (with pruning, 10 was too pessimistic). With `go movetime` the soft limit is huge, so
the search runs to the full movetime.
Check on 5 positions with 4 clocks (25+1, 55+1, 5+1, 60+0): times and depths almost unchanged compared with v13.3:
with v13's pruning each depth costs only 2-4× the previous one, so the old prediction was rarely the limit.
The ~25 s left at the end comes from the formula soft = timeLeft / 30 + inc (with every move at full soft time,
60 × (29/30)^50 ≈ 11 s would be left after 50 moves; many moves use less). Change for the Lichess test:
soft = timeLeft / 20 + inc (≈ 4.6 s left after 50 moves at full soft time), hard = min(2 × soft, timeLeft / 6) unchanged.
To check: clock left at the end of 60+1 games and losses on time; then a 60+1 match vs v13.3 in cutechess.

#### Local tests (cutechess)
| Opponent | TC     | Games | Results W-L-D | Elo        |
|----------|--------|-------|---------------|------------|
| v12      | 10+0.1 | 100   | 52 - 32 - 16  | +70 ±64    |
| SF2000   | 10+0.1 | 100   | 78 - 19 - 3   | +235 ±86   |
| SF2100   | 10+0.1 | 100   | 73 - 24 - 3   | +186 ±79   |
| SF2200   | 10+0.1 | 100   | 62 - 34 - 4   | +100 ±71   |
| SF2300   | 10+0.1 | 100   | 58 - 34 - 8   | +85 ±68    |
| SF2400   | 10+0.1 | 100   | 48 - 44 - 8   | +14 ±66    |

vs v12: LOS 98.5%, no losses on time; as White 26 - 17 - 7, as Black 26 - 15 - 9.
vs SF2000: 79.5% (v12: 71%, v10: 55.5% against the same opponent), LOS 100%, no losses on time.
vs SF2100: 74.5% (v12: 63.5%), LOS 100%, no losses on time.
vs SF2200: 64%, LOS 99.8%, no losses on time.
vs SF2300: 62%, LOS 99.4%, no losses on time. 2200 and 2300 give almost the same score: at 10+0.1 the limited
Stockfish levels are compressed, so they are a rough reference scale, not calibrated ratings.
vs SF2400: 52%, LOS 66%: about equal. On this scale v13 scores about 50% around Stockfish UCI_Elo 2400 at 10+0.1.

Average depth in the match vs v12 (from the PGN, `tools/pgn_depth_stats.py`):
| Engine | Moves 10-40 | All moves | Time/move |
|--------|-------------|-----------|-----------|
| v12    | 6.25        | 6.95      | 0.247 s   |
| v13    | 8.43        | 9.01      | 0.256 s   |

v13 searches about 2.2 plies deeper in the middlegame at the same time per move. The Elo gain (+70 ± 64) is smaller
than the depth gain alone would suggest: the reductions (null move, LMR) make each ply less thorough, so a ply of
v13 is not worth a full ply of v12. Still clearly positive (LOS 98.5%).

### Lichess (bot: TorrinoBOT)
Online from: 25-10-2026 to: 
| Category | Starting rating | Final rating | Games |
|----------|-----------------|--------------|-------|
| Bullet   | 2010            |    |     |
| Blitz    | 1872            |    |     |

Sources:
- [Time Management - Chess Programming Wiki](https://chessprogramming.org/Time_Management): soft (optimum) limit checked
  after each iteration, hard (maximum) limit checked during the search
- [VeryChess 0.3.0 release - TalkChess](https://talkchess.com/viewtopic.php?p=995986): a soft limit used only to decide
  whether to start an iteration overspends 1.5-4×, since each iteration costs about twice the previous one
- [Time management and estimating search times - TalkChess](https://talkchess.com/viewtopic.php?p=894313): predictions
  from previous iterations oscillate (the TT is already filled); skipping iterations that will not finish is a compromise
- [Easy moves and best-move stability - TalkChess](https://talkchess.com/viewtopic.php?p=402413): spend less time when the
  best move is stable across iterations, more when it changes (possible next step)
- [Crafty time.c](https://sources.debian.org/src/crafty/23.4-6/time.c/): example implementation (easy moves stop at
  about 36% of the limit, the hard limit aborts the search)

