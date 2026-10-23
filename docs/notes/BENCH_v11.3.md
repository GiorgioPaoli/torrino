# Speed test: v10 vs v11.2 vs v11.3 (lazy move ordering)

Date: 2026-10-24. Raw data: `docs/data/bench_v10_v11.3.csv`. Previous steps: `BENCH_v11.1.md`, `BENCH_v11.2.md`.

Engines: `bin/engine_v10.exe`, `bin/engine_v11.exe` (= v11.2, the build that played the match vs v10),
`bin/engine_v11.3.exe`. Single thread, Intel i5 10th gen laptop, nothing else running.

## Test 1: time to depth 7
`go depth 7` on the 6 standard positions, 10 runs per engine and position, order of the three engines rotated
every run, fastest run reported (medians in the CSV).

| Position | v10 time | v11.2 time | v11.3 time | v10 NPS | v11.2 NPS | v11.3 NPS | v11.3 vs v11.2 | v11.3 vs v10 |
|---|---|---|---|---|---|---|---|---|
| start | 0.446 s | 0.308 s | 0.309 s | 2.33 M | 3.37 M | 3.36 M | 0% | +44% |
| kiwipete | 7.014 s | 4.190 s | 4.052 s | 1.36 M | 2.27 M | 2.35 M | +3% | +73% |
| pos3 | 0.323 s | 0.183 s | 0.178 s | 2.56 M | 4.50 M | 4.64 M | +3% | +81% |
| pos4 | 0.994 s | 0.613 s | 0.552 s | 1.28 M | 2.08 M | 2.31 M | +11% | +80% |
| pos5 | 2.396 s | 1.593 s | 1.542 s | 2.51 M | 3.77 M | 3.89 M | +3% | +55% |
| pos6 | 6.237 s | 3.775 s | 3.445 s | 1.76 M | 2.90 M | 3.18 M | +10% | +81% |
| **Total** | **17.41 s** | **10.66 s** | **10.08 s** | **1.70 M** | **2.78 M** | **2.94 M** | **+6%** | **+73%** |

Node counts and best moves of v11.3 are **exactly** equal to v11.2 on all 6 positions: lazy ordering searches the
moves in the same order, it only stops sorting when a cutoff ends the loop.
Using the medians instead of the fastest runs, v11.3 vs v11.2 is +7% in total (12.19 s → 11.35 s).

## Test 2: depth reached in a fixed time
`go movetime 3000` (3 seconds per move), 3 runs per engine and position, engines alternated.
Depth = last fully completed depth reported by the engine; best of the 3 runs.

| Position | v10 depth | v11.3 depth |
|---|---|---|
| start | 7 | **8** |
| kiwipete | 6 | 6 |
| pos3 | 8 | 8 |
| pos4 | 7 | **8** |
| pos5 | 7 | 7 |
| pos6 | 6 | 6 |

v11.3 completes one more ply in 2 of 6 positions and never less. In the other positions the next depth costs more
than the remaining time: the engine estimates the next iteration and does not start a depth it cannot finish
(kiwipete depth 7 takes about 4 s for v11.3, 7 s for v10). This test is coarse, because depth only grows in whole
plies; the time-to-depth figures above are the precise measure.

## Reading the results
- Lazy ordering adds about +6% NPS over v11.2: a small gain, as expected (sorting was a small part of the cost per
  node), and close to the run-to-run noise on single positions (start: 0%, pos4: +11%), but positive in total with
  both the fastest and the median runs.
- v10 → v11.3: +73% NPS with the same search; depth 7 takes 10.1 s instead of 17.4 s over the 6 positions (-42%).
- No match yet for v11.3. Its search is identical to v11.2 (same nodes, same moves), which beat v10 by
  +135 ± 64 Elo in 100 games at 10+0.1; being faster, v11.3 cannot be weaker than v11.2.
