# Speed test: v10 vs v11.1 vs v11.2 (captures-only quiescence)

Date: 2026-10-23. Raw data: `docs/data/bench_v10_v11.2.csv`. Previous step: `BENCH_v11.1.md`.

## Method
Same as for v11.1: `bin/engine_v10.exe`, `engine_v11.1.exe`, `engine_v11.2.exe`, single thread, Intel i5 10th gen
laptop with nothing else running; 6 standard positions at `go depth 7`; 10 runs per engine and position with the
order of the three engines rotated every run; fastest run reported (medians in the CSV).

## Results (depth 7, fastest of 10 runs)

| Position | v10 time | v11.1 time | v11.2 time | v10 NPS | v11.1 NPS | v11.2 NPS | v11.2 vs v11.1 | v11.2 vs v10 |
|---|---|---|---|---|---|---|---|---|
| start | 0.443 s | 0.351 s | 0.288 s | 2.35 M | 2.96 M | 3.61 M | +22% | +54% |
| kiwipete | 6.647 s | 4.926 s | 3.884 s | 1.43 M | 1.93 M | 2.45 M | +27% | +71% |
| pos3 | 0.281 s | 0.190 s | 0.146 s | 2.94 M | 4.34 M | 5.64 M | +30% | +92% |
| pos4 | 0.875 s | 0.615 s | 0.531 s | 1.45 M | 2.07 M | 2.40 M | +16% | +65% |
| pos5 | 2.439 s | 1.798 s | 1.585 s | 2.46 M | 3.34 M | 3.79 M | +13% | +54% |
| pos6 | 6.401 s | 5.037 s | 3.899 s | 1.71 M | 2.18 M | 2.81 M | +29% | +64% |
| **Total** | **17.09 s** | **12.92 s** | **10.33 s** | **1.73 M** | **2.29 M** | **2.87 M** | **+25%** | **+65%** |

Total NPS = total nodes / total time over the 6 positions. v10 and v11.1 figures were re-measured in this run;
they agree with `BENCH_v11.1.md` within about 2%.

## Node counts
Best moves are identical for all three versions. v11.1 has exactly the same nodes as v10; v11.2 differs by less than
0.1% (for example kiwipete 9,523,360 vs 9,524,202, pos6 10,965,855 vs 10,956,554). The set of moves searched in
quiescence is the same: a test build that keeps the old generator but filters the moves *before* sorting gives exactly
the same nodes as v11.2. The difference comes only from the move sort, which can order captures with equal scores
differently when quiet moves are in the list.

## Reading the results
- v11.2 adds +25% NPS on top of v11.1; v10 → v11.2 is +65% NPS, and the same depth takes about 40% less time.
- The gain is largest in tactical middlegames (kiwipete, pos6, +27-29%), where most quiescence nodes have many
  quiet moves that were generated, scored and sorted for nothing.
- At depth 7, v11.2 runs at 2.87M nps, above v9 (about 2.1M at depth 6 in the earlier benchmark) while keeping
  the v10 evaluation.
