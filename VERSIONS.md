# Versions

## v4 — quiescence search
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




## v7 — Zobrist hashing + repetition detection
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
Online from: 20-10-2026   to: - 21-10-2026
| Category | Starting rating | Final rating | Games | Results W-L-D |
|----------|-----------------|--------------|-------|---------------|
| Bullet   | 1894            | 1948 (+54)   | 63    | 33 - 21 - 9   |
| Blitz    | 1748            | 1753 (+5)    | 26    | 9 - 12 - 5    |

Notes: opening book off (same as previous versions), concurrency 2 (1 reserved for humans)

