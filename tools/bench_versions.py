"""Speed benchmark across engine versions: time to reach a fixed depth and nodes per second (NPS).

Usage: python tools/bench_versions.py [--depth 6] [--runs 3] [--csv docs/data/bench_versions.csv]
Run it with NOTHING ELSE using the CPU (no Lichess bot, no cutechess, no videos): timings depend on load.

For every engine in ENGINES and every position in POSITIONS it sends `go depth N`, measures the wall-clock
time until `bestmove`, and reads the node count from the last `info depth N ... nodes X` line
(versions before v5 do not print `info` lines: only their time is recorded).
Each measurement is repeated `--runs` times and the fastest run is kept (the least disturbed by the OS).
"""
import sys, os, re, csv, time, subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
BIN = os.path.join(HERE, "..", "bin")

# version label -> exe file name in bin/ (edit when you add versions)
ENGINES = [
    ("v3",    "engine_v3_fix.exe"),
    ("v4",    "engine_v4_QUIESCENCE.exe"),
    ("v5",    "engine_v5_KILLER_MOVES.exe"),
    ("v6",    "engine_v6_CHECK_EXTENSION.exe"),
    ("v7",    "engine_v7_Zobrist.exe"),
    ("v8",    "engine_v8_Time_Management.exe"),
    ("v9",    "engine_v9_Time_Management_2.exe"),
    ("v10",   "engine_v10.exe"),
    ("v11",   "engine_v11.exe"),          # = v11.3
    ("v12",   "engine_v12.exe"),
    ("v13",   "engine_v13.exe"),          # = v13.4
]

# standard test positions (Chess Programming Wiki perft positions)
POSITIONS = [
    ("start",    "startpos"),
    ("kiwipete", "fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"),
    ("pos3",     "fen 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"),
    ("pos4",     "fen r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"),
    ("pos5",     "fen rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"),
    ("pos6",     "fen r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"),
]

INFO = re.compile(r"info depth (\d+) .*?nodes (\d+)")


def run_once(exe, position, depth, timeout=300):
    """Returns (seconds, nodes or None) for one `go depth` search."""
    p = subprocess.Popen([exe], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True, bufsize=1)
    p.stdin.write(f"uci\nisready\nposition {position}\n"); p.stdin.flush()
    while p.stdout.readline().strip() != "readyok":           # wait until the engine is ready
        pass
    start = time.perf_counter()
    p.stdin.write(f"go depth {depth}\n"); p.stdin.flush()
    nodes = None
    deadline = start + timeout
    while True:
        line = p.stdout.readline()
        if not line or time.perf_counter() > deadline:
            p.kill(); return None, None
        m = INFO.search(line)
        if m and int(m.group(1)) == depth:
            nodes = int(m.group(2))
        if line.startswith("bestmove"):
            break
    secs = time.perf_counter() - start
    p.stdin.write("quit\n"); p.stdin.flush(); p.wait(timeout=5)
    return secs, nodes


def main():
    args = sys.argv[1:]
    def opt(name, default):
        if name in args:
            i = args.index(name); v = args[i + 1]; del args[i:i + 2]; return v
        return default
    depth = int(opt("--depth", 6))
    runs = int(opt("--runs", 3))
    out = opt("--csv", os.path.join(HERE, "..", "docs", "data", "bench_versions.csv"))

    rows = []
    for label, exe_name in ENGINES:
        exe = os.path.join(BIN, exe_name)
        if not os.path.exists(exe):
            print(f"{label}: {exe_name} not found, skipped"); continue
        for pos_name, pos in POSITIONS:
            best = None
            for _ in range(runs):
                secs, nodes = run_once(exe, pos, depth)
                if secs is not None and (best is None or secs < best[0]):
                    best = (secs, nodes)
            if best is None:
                print(f"{label:5} {pos_name:9} timeout"); continue
            secs, nodes = best
            nps = round(nodes / secs) if nodes else ""
            rows.append({"version": label, "position": pos_name, "depth": depth,
                         "seconds": round(secs, 4), "nodes": nodes or "", "nps": nps})
            print(f"{label:5} {pos_name:9} depth {depth}: {secs:7.3f} s  nodes {nodes or '-':>10}  nps {nps or '-'}")
    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, "w", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=list(rows[0].keys())); w.writeheader(); w.writerows(rows)
    print("saved", out)


if __name__ == "__main__":
    main()
