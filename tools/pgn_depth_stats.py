"""Search depth per engine version, extracted from cutechess PGN comments like {+0.85/7 0.39s}.

Usage: python tools/pgn_depth_stats.py <folder-or-pgn> [<folder-or-pgn> ...] [--tc 10+0.1] [--csv out.csv]
Only games played at the given time control are used, so versions are compared under equal conditions.
Book moves, forced moves (reported depth >= 64) and moves without a comment are ignored.
"""
import sys, os, re, csv, statistics as st
import chess.pgn

COMMENT = re.compile(r"([-+]?\d+\.\d+|[-+]?M\d+)/(\d+) ([\d.]+)s")


def version_of(name):
    """Map cutechess player names (e.g. 'v5_Killer_Moves', 'v10.2', 'v6_early') to a version label."""
    m = re.match(r"v(\d+(?:\.\d+)?)", name.strip().lower())
    if not m:
        return None                                  # Stockfish, Torrino (old self-play), ...
    v = m.group(1)
    return "v" + v


def collect(paths, tc):
    data = {}                                        # version -> list of (depth, seconds, move number)
    files = []
    for p in paths:
        files += [os.path.join(p, f) for f in os.listdir(p) if f.endswith(".pgn")] if os.path.isdir(p) else [p]
    for path in files:
        with open(path, encoding="utf-8", errors="replace") as fh:
            while (g := chess.pgn.read_game(fh)) is not None:
                if g.headers.get("TimeControl") != tc:
                    continue
                names = {True: g.headers["White"], False: g.headers["Black"]}
                node, ply = g, 0
                while node.variations:
                    node = node.variations[0]
                    side = ply % 2 == 0
                    m = COMMENT.search(node.comment or "")
                    v = version_of(names[side])
                    if m and v:
                        depth = int(m.group(2))
                        if depth < 64:
                            data.setdefault(v, []).append((depth, float(m.group(3)), ply // 2 + 1))
                    ply += 1
    return data


def sort_key(v):
    return [float(x) for x in v[1:].split(".")]


if __name__ == "__main__":
    args = sys.argv[1:]
    tc, out = "10+0.1", None
    if "--tc" in args:
        i = args.index("--tc"); tc = args[i + 1]; del args[i:i + 2]
    if "--csv" in args:
        i = args.index("--csv"); out = args[i + 1]; del args[i:i + 2]
    data = collect(args, tc)
    rows = []
    for v in sorted(data, key=sort_key):
        d = [x[0] for x in data[v]]
        mid = [x[0] for x in data[v] if 10 <= x[2] <= 40]
        rows.append({"version": v, "moves": len(d), "avg_depth": round(st.mean(d), 2),
                     "median_depth": st.median(d), "avg_depth_moves_10_40": round(st.mean(mid), 2) if mid else "",
                     "avg_time_s": round(st.mean(x[1] for x in data[v]), 3)})
    for r in rows:
        print(f"{r['version']:7} moves={r['moves']:6}  avg depth={r['avg_depth']:5}  median={r['median_depth']:4}  "
              f"avg depth (moves 10-40)={r['avg_depth_moves_10_40']:5}  avg time/move={r['avg_time_s']}s")
    if out:
        with open(out, "w", newline="") as fh:
            w = csv.DictWriter(fh, fieldnames=list(rows[0].keys())); w.writeheader(); w.writerows(rows)
