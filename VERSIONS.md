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
Cumulative gain over v9 (sum of the three steps): ~ +276 Elo.
Mobility and piece activity postponed until after magic bitboards (v11): they need slider
attacks for every piece and would be too slow with the current attack functions.

### Lichess (bot: TorrinoBOT)
Online from: —   to: —
| Category | Starting rating | Final rating | Games | Results W-L-D |
|----------|-----------------|--------------|-------|---------------|
| Bullet   |                 |              |       |               |
| Blitz    |                 |              |       |               |

Note: v10.1 was also online on 2026-10-22 (rating before v10: bullet ____, blitz ____)
