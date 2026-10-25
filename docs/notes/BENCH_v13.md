# Speed and depth tests: v12.3 vs v13.1 (PVS) vs v13.2 (null-move pruning)

Date: 2026-10-25. Raw data: `docs/data/bench_v13.csv`.
Engines: `bin/engine_v12.3.exe` (v12), `bin/engine_v13.1.exe`, `bin/engine_v13.2.exe`. Single thread per search,
Intel i5 10th gen laptop, Lichess bot stopped.

## Correctness
- Perft correct on the 6 standard positions (test build of the v13.2 code).
- Null move: makeNullMove/unmakeNullMove checked at 177,905 positions (perft trees of 4 positions, one with en
  passant): the hash after the null move equals a full recomputation, and side to move, en passant square,
  half-move clock, hash and history are restored exactly. 0 errors.
- Same best move and same score as v12.3 on all 6 positions at depth 7, for both v13.1 and v13.2.

## Test 1: time to depth 7
`go depth 7`, 10 runs per engine and position, two searches at a time, fastest run reported (medians in the CSV).

| Position | v12.3 nodes | v13.1 nodes | v13.2 nodes | v12.3 time | v13.1 time | v13.2 time | v13.2 vs v12.3 (time) |
|---|---|---|---|---|---|---|---|
| start | 494,818 | 485,720 | 229,765 | 0.165 s | 0.164 s | 0.070 s | -58% |
| kiwipete | 3,653,450 | 3,406,719 | 1,828,821 | 1.617 s | 1.433 s | 0.693 s | -57% |
| pos3 | 121,410 | 115,903 | 56,538 | 0.027 s | 0.024 s | 0.011 s | -57% |
| pos4 | 632,291 | 624,800 | 342,361 | 0.248 s | 0.245 s | 0.136 s | -45% |
| pos5 | 1,919,909 | 1,897,868 | 393,597 | 0.562 s | 0.559 s | 0.111 s | -80% |
| pos6 | 3,644,392 | 3,200,526 | 842,827 | 1.325 s | 1.114 s | 0.302 s | -77% |
| **Total** | **10.47M** | **9.73M (-7%)** | **3.69M (-65%)** | **3.94 s** | **3.54 s (-10%)** | **1.32 s (-66%)** | **-66%** |

NPS: v12.3 2.65M, v13.1 2.75M, v13.2 2.79M (unchanged: the gain is a smaller tree).
Medians (total): 4.52 s, 4.08 s, 1.69 s.

## Test 2: depth reached in 3 seconds
`go movetime 3000`, 3 runs per engine and position (identical in all runs).

| Position | v12.3 | v13.2 |
|---|---|---|
| start | 8 | **10** |
| kiwipete | 7 | **8** |
| pos3 | 10 | **13** |
| pos4 | 8 | **10** |
| pos5 | 8 | **9** |
| pos6 | 7 | **8** |

## Test 3: basic mates (`go movetime 20000`)
- King and rook vs king (`8/8/8/4k3/8/8/8/R3K3 w - - 0 1`): v12.3 first sees a mate at depth 17 after 13.0 s
  (mate 19); v13.2 at depth 18 after 6.9 s (mate 22), then mate 16 from depth 19 (9.2 s), stable up to depth 21.
- King and queen vs king (`8/8/8/4k3/8/8/8/3QK3 w - - 0 1`): both find mate 7 at depth 13 (4.1 s / 4.3 s) and keep
  it; in 20 s v13.2 reaches depth 20 against depth 16 for v12.3.

## Reading the results
- PVS (v13.1) alone: -7% nodes, -10% time. Small, as expected: it mainly prepares the zero-window searches used by LMR.
- Null-move pruning (v13.2): -65% nodes and -66% time to depth 7 compared with v12.3, with the same moves and scores
  on all 6 positions; in 3 seconds it reaches 1 to 3 plies deeper.
- The largest gains are in positions where one side is clearly better (pos5 -80%, pos6 -77%): there the null move
  proves cutoffs very often.
