# Versions

## v4 — quiescence search
Commit: tag `v4`
Changes: Added Quiescence search, eval with piece development, fixed en passant

### Local tests (cutechess)
| Opponeng   | TC      | Games   | Results       | Elo        |
|------------|---------|---------|---------------|------------|
| v3fix      | 10+0.1  | 100     | 53-2-45       | +195 ± 50  |
| SF1500     | 10+0.1  | 100     | 78-19-3       | +235 ± 86  |
| SF1750     | 10+0.1  | 100     | 56-38-6       | +63 ± 68   |
| SF1900     | 10+0.1  | 100     | 51-39-10      | +42 ± 66   |
| SF1900     | 180+2   | 16      | 13-3-0        | +255 (?)   |

### Lichess (bot: TorrinoBOT)
Online from: 2026-10-17  to: 2026-10-18
| Category  | Initial Rating  | Final Rating  | Games   |
|-----------|-----------------|---------------|---------|
| Bullet    | 1500?           |               |         |
| Blitz     | 1500?           |               |         |

Note: concurrency 3 (1 for humans), move_overhead 1000



## v5 - killer moves
Commit: tag `v5`
Changes: Added Killer Moves (quiet, 1 and 2)

### Local tests (cutechess)
| Opponeng   | TC      | Games   | Results       | Elo        |
|------------|---------|---------|---------------|------------|
| v4         | 10+0.1  | 100     | 22 - 5 - 73   | +59 ± 34   |
| SF1500     | 10+0.1  | 100     | | |
| SF1750     | 10+0.1  | 100     | | |
| SF1900     | 10+0.1  | 100     | 44 - 42 - 14  | 7 ± 63     |
| SF1900     | 180+2   | 16      | | |

### Lichess (bot: TorrinoBOT)
Online from: 2026-10-17  to: 2026-10-18
| Category  | Initial Rating  | Final Rating  | Games   |
|-----------|-----------------|---------------|---------|
| Bullet    | 1500?           |               |         |
| Blitz     | 1500?           |               |         |