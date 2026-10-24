# Speed test: all v11 steps in one run (v10, v11.1, v11.2, v11.3)

Date: 2026-10-24. Raw data: `docs/data/bench_v11_all.csv`. Earlier per-step runs: `BENCH_v11.1.md`,
`BENCH_v11.2.md`, `BENCH_v11.3.md` (same method, fewer engines per run).

## Method
- Engines: `bin/engine_v10.exe` (v10), `bin/engine_v11.1.exe` (v11.1), `bin/engine_v11.2.exe` (v11.2, the build
  that played +135 ± 64 vs v10), `bin/engine_v11.exe` (v11.3 = final v11). Single thread.
- Machine: Intel i5 10th gen laptop (4 cores / 8 threads), 8 GB RAM, Windows 11, nothing else running
  (Lichess bot and cutechess stopped).
- `go depth 7` on the 6 standard positions (start, kiwipete, pos3-pos6 from the Chess Programming Wiki).
- 10 runs per engine and position; the order of the four engines is rotated every run, so background noise
  affects all of them equally. The fastest run is reported; medians are in the CSV.
- Time = wall clock from `go` to `bestmove`. NPS = nodes / time.

## Results (fastest of 10 runs)

| Position | v10 | v11.1 | v11.2 | v11.3 | v11.1 vs v10 | v11.2 vs v11.1 | v11.3 vs v11.2 | v11.3 vs v10 |
|---|---|---|---|---|---|---|---|---|
| start | 2.46 M (0.42 s) | 3.12 M (0.33 s) | 3.56 M (0.29 s) | 3.61 M (0.29 s) | +27% | +14% | +2% | +47% |
| kiwipete | 1.44 M (6.60 s) | 1.94 M (4.91 s) | 2.39 M (3.98 s) | 2.67 M (3.57 s) | +35% | +23% | +11% | +85% |
| pos3 | 2.88 M (0.29 s) | 4.43 M (0.19 s) | 5.61 M (0.15 s) | 5.53 M (0.15 s) | +54% | +27% | -1% | +92% |
| pos4 | 1.49 M (0.86 s) | 2.03 M (0.63 s) | 2.42 M (0.53 s) | 2.68 M (0.48 s) | +36% | +20% | +10% | +80% |
| pos5 | 2.51 M (2.40 s) | 3.39 M (1.77 s) | 3.81 M (1.58 s) | 3.90 M (1.54 s) | +35% | +12% | +2% | +56% |
| pos6 | 1.76 M (6.21 s) | 2.25 M (4.87 s) | 2.85 M (3.84 s) | 3.11 M (3.53 s) | +28% | +27% | +9% | +76% |
| **Total** | **1.77 M (16.78 s)** | **2.33 M (12.69 s)** | **2.86 M (10.36 s)** | **3.10 M (9.55 s)** | **+32%** | **+23%** | **+8%** | **+76%** |

Each cell: NPS (time to depth 7). Total = total nodes / total time over the 6 positions.

With median times instead of the fastest runs (totals): v10 1.66 M (17.84 s), v11.1 2.17 M (13.62 s),
v11.2 2.70 M (10.99 s), v11.3 2.87 M (10.33 s): v11.3 vs v10 +73%, v11.3 vs v11.2 +6%.

## Correctness in the same run
- Best moves identical for all four engines on all 6 positions.
- v11.1 nodes = v10 nodes exactly; v11.3 nodes = v11.2 nodes exactly.
- v11.2 nodes within 0.1% of v11.1 (for example kiwipete 9,523,360 vs 9,524,202): same move set in quiescence,
  only the order of equal-score captures in the sort differs (explained in `BENCH_v11.2.md`).
- This also confirms that the renamed files in `bin/` are the intended builds.

## Reading the results
- v10 → v11.3: +76% NPS with the same evaluation and the same search; depth 7 on the 6 positions takes 9.6 s
  instead of 16.8 s (-43%).
- The three steps contribute +32% (magic bitboards), +23% (captures-only quiescence) and +8% (lazy ordering).
- Lazy ordering helps most where many moves are searched before a cutoff (kiwipete +11%, pos4 +10%, pos6 +9%)
  and is within the noise elsewhere (start +2%, pos3 -1%, pos5 +2%).
- These numbers agree with the earlier per-step runs within a few percent (there: +32%, +25%, +6%).
