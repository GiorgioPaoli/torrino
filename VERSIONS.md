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
Online from: 2026-10-17  to: 2026-10-18
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
Online from: 2026-10-18  to: 2026-10-19
| Category  | Initial Rating  | Final Rating  | Games   | Results W-L-D |
|-----------|-----------------|---------------|---------|---------------|
| Bullet    | 1869            | 1848 (-21)    | 38      | 16 - 12 - 10  |
| Blitz     | 1663            | 1704 (+41)    | 25      | 9 - 6 - 10    |




## v6 - check extension
Commit: taag `v6`
Changes: Added check extensions for finding mates quicker and mate scores 

### Local tests (cutechess)
| Opponent                  | TC        | Games | Results W-L-D | Elo                               |
|---------------------------|-----------|-------|---------------|-----------------------------------|
| v5                        | 10+0.1    | 100   | 21 - 12 - 67  | +31 ± 39                          |
| SF2000                    | 10+0.1    | 100   | 50 - 43 - 7   | +24 ± 66                          |

#### Conclusions
Better than v5, with +31 ± 39 Elo, likely of superiority is 94.1%