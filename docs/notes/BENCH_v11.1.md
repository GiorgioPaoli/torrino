# Speed test: v10 vs v11.1 (magic bitboards)

Date: 2026-10-23. Raw data: `docs/data/bench_v10_v11.1.csv`.

## What changed in v11.1
Sliding-piece attacks (bishop, rook, queen) use magic bitboards (one table lookup) instead of walking the rays
square by square. Used in move generation, `isSquareAttacked` and the king-attack term of the evaluation.
The search itself is unchanged.

## Method
- Engines: `bin/engine_v10.exe` and `bin/engine_v11.1.exe`, single thread.
- Machine: Intel i5 10th gen laptop (4 cores / 8 threads), 8 GB RAM, Windows 11, nothing else running
  (no Lichess bot, no cutechess).
- 6 standard test positions (start, kiwipete, pos3-pos6 from the Chess Programming Wiki), `go depth 6` and `go depth 7`.
- 10 runs per engine and position; the two engines alternate run by run, so background noise affects both equally.
- The **fastest** of the 10 runs is reported (the least disturbed by the operating system); medians are in the CSV.
- Time = wall clock from `go` to `bestmove`.

## Correctness
- Node counts and best moves are **identical** for v10 and v11.1 in all 12 position/depth combinations:
  the new attack functions change the speed, not the search.
- The magic lookups were compared with the old ray-loop functions on 38.4 million random occupancies (all 64 squares,
  rook, bishop and queen): 0 mismatches.
- Perft on the 6 positions gives the reference counts (checked on a test build of the same code).

## Results (fastest of 10 runs)

### Depth 7 (longer searches, the more reliable figure)

| Position | Nodes | v10 time (s) | v11.1 time (s) | v10 NPS | v11.1 NPS | Speed-up |
|---|---|---|---|---|---|---|
| start | 1,040,401 | 0.430 | 0.342 | 2.42 M | 3.04 M | +26% |
| kiwipete | 9,524,202 | 6.819 | 5.049 | 1.40 M | 1.89 M | +35% |
| pos3 | 825,324 | 0.287 | 0.194 | 2.88 M | 4.26 M | +48% |
| pos4 | 1,272,923 | 0.880 | 0.643 | 1.45 M | 1.98 M | +37% |
| pos5 | 6,004,982 | 2.488 | 1.829 | 2.41 M | 3.28 M | +36% |
| pos6 | 10,956,554 | 6.426 | 5.106 | 1.70 M | 2.15 M | +26% |
| **Total** | **29,624,386** | **17.33** | **13.16** | **1.71 M** | **2.25 M** | **+32%** |

### Depth 6

| Position | Nodes | v10 time (s) | v11.1 time (s) | v10 NPS | v11.1 NPS | Speed-up |
|---|---|---|---|---|---|---|
| start | 164,600 | 0.102 | 0.085 | 1.62 M | 1.94 M | +19% |
| kiwipete | 1,678,397 | 1.258 | 0.958 | 1.33 M | 1.75 M | +31% |
| pos3 | 184,986 | 0.061 | 0.042 | 3.03 M | 4.41 M | +45% |
| pos4 | 345,289 | 0.233 | 0.165 | 1.48 M | 2.09 M | +41% |
| pos5 | 691,630 | 0.411 | 0.317 | 1.68 M | 2.18 M | +30% |
| pos6 | 1,732,006 | 1.282 | 1.000 | 1.35 M | 1.73 M | +28% |
| **Total** | **4,796,908** | **3.35** | **2.57** | **1.43 M** | **1.87 M** | **+30%** |

Total NPS = total nodes / total time over the 6 positions.

## Reading the results
- **About +30% NPS overall** (+32% at depth 7, +30% at depth 6), with the same tree: v11.1 reaches the same depth
  in about 24% less time.
- This recovers most of the slowdown introduced by v10's evaluation (v9 to v10 was -32% NPS at depth 6 in the
  earlier benchmark): v11.1 at depth 6 runs at 1.87 M nodes/s, against 2.13 M for v9 and 1.43 M for v10.
- The gain is largest in pos3 (+45-48%), an endgame where rooks dominate, and smallest in the start position
  (+19-26%), where the sliders are still blocked by their own pawns and the ray loops were already short.
- Depth 6 runs are short (some under 0.1 s), so the depth 7 figures are the more reliable ones.
- Earlier estimate in the roadmap: 1.5-2.5x. The real gain is 1.3x: the slider attacks were only part of the
  cost per node (move generation and legality checks, evaluation, sorting all moves in quiescence).

## Next steps (v11)
- v11.2: captures-only move generation in quiescence (today it generates all moves and discards the quiet ones).
- v11.3: lazy move ordering (pick the best remaining move instead of sorting the whole list).
- Run the same test after each step, then a match v11 vs v10 to confirm no regression.
