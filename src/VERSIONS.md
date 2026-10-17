# Versioni

## v4 — quiescence search
Commit: tag `v4`
Modifiche: quiescence search, eval con sviluppo dei pezzi, fix en passant

### Test locali (cutechess)
| Avversario | TC      | Partite | Risultato     | Elo        |
|------------|---------|---------|---------------|------------|
| v3fix      | 10+0.1  | 100     | 53-2-45       | +195 ± 50  |
| SF1500     | 10+0.1  | 100     | 78-19-3       | +235 ± 86  |
| SF1750     | 10+0.1  | 100     | 56-38-6       | +63 ± 68   |
| SF1900     | 10+0.1  | 100     | 51-39-10      | +42 ± 66   |
| SF1900     | 180+2   | 16      | 13-3-0        | +255 (?)   |

### Lichess (bot: TorrinoBOT)
Online dal: 2026-10-17  al: —
| Categoria | Rating iniziale | Rating finale | Partite |
|-----------|-----------------|---------------|---------|
| Bullet    | 1500?           |               |         |
| Blitz     | 1500?           |               |         |

Note: concurrency 2 (1 per umani), move_overhead 1000