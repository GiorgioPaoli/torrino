"""README charts for the Torrino chess engine.

Usage: python tools/make_charts.py docs/images
Writes <name>_light.png and <name>_dark.png for each chart.
Data: match results from VERSIONS.md (below), docs/data/depth_per_version.csv
(from tools/pgn_depth_stats.py), docs/data/bench_versions.csv (from tools/bench_versions.py)
and docs/data/lichess_rating_history.json (Lichess API).
"""
import sys, os, math, csv, json, datetime
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

OUT = sys.argv[1]
HERE = os.path.dirname(os.path.abspath(__file__))
DATA = os.path.join(HERE, "..", "docs", "data")
os.makedirs(OUT, exist_ok=True)

THEMES = {
    "light": dict(surface="#fcfcfb", ink="#0b0b0b", ink2="#52514e", muted="#898781",
                  grid="#e1e0d9", axis="#c3c2b7", s1="#2a78d6", s2="#eb6834"),
    "dark":  dict(surface="#1a1a19", ink="#f0efec", ink2="#c3c2b7", muted="#898781",
                  grid="#2c2c2a", axis="#383835", s1="#3987e5", s2="#d95926"),
}
FONT = ["Segoe UI", "DejaVu Sans"]

# ---------------- match data (VERSIONS.md) ----------------
# each version vs the previous one: (label, elo, err, time control)
STEPS = [("v4", 195, 50, "10+0.1"), ("v5", 59, 34, "10+0.1"), ("v6", 31, 39, "10+0.1"),
         ("v7", 67, 48, "10+0.1"), ("v8", 60, 55, "10+0.1"), ("v9", 49, 66, "30+1"),
         ("v10", 135, 69, "10+0.1")]
# vs Stockfish UCI_Elo 2000, 100 games each: (label, elo, err, SF losses on time, actual score %)
VS_SF = [("v5", -63, 67, 0, 41.0), ("v6", 24, 66, 0, 53.5), ("v7", 20, 67, 5, 53.0),
         ("v8", 31, 67, 3, 54.5), ("v10", 38, 67, 0, 55.5)]


def score(elo):
    return 100.0 / (1.0 + 10 ** (-elo / 400.0))


def base(t, w=8.0, h=4.6):
    plt.rcParams["font.family"] = FONT
    fig, ax = plt.subplots(figsize=(w, h), dpi=150)
    fig.patch.set_facecolor(t["surface"])
    ax.set_facecolor(t["surface"])
    for s in ("top", "right", "left"):
        ax.spines[s].set_visible(False)
    ax.spines["bottom"].set_color(t["axis"])
    ax.tick_params(colors=t["muted"], labelsize=9, length=0)
    ax.yaxis.grid(True, color=t["grid"], linewidth=1, linestyle="-")
    ax.set_axisbelow(True)
    return fig, ax


def titles(fig, t, title, subtitle):
    fig.text(0.06, 0.95, title, fontsize=13, fontweight="semibold", color=t["ink"], ha="left", va="top")
    fig.text(0.06, 0.885, subtitle, fontsize=9, color=t["ink2"], ha="left", va="top")


def save(fig, name, mode):
    fig.savefig(os.path.join(OUT, f"{name}_{mode}.png"), facecolor=fig.get_facecolor())
    plt.close(fig)


def dot(ax, x, y, t, color=None, hollow=False):
    c = color or t["s1"]
    ax.plot(x, y, "o", markersize=8, markerfacecolor=t["surface"] if hollow else c,
            markeredgecolor=c if hollow else t["surface"], markeredgewidth=2, zorder=4)


# ---------------- charts ----------------
def chart_progression(mode):
    t = THEMES[mode]
    fig, ax = base(t)
    labels = ["v3"] + [s[0] for s in STEPS]
    cum, err, c, e2 = [0], [0], 0, 0
    for _, elo, e, _ in STEPS:
        c += elo; e2 += e * e
        cum.append(c); err.append(math.sqrt(e2))
    x = list(range(len(labels)))
    ax.fill_between(x, [a - b for a, b in zip(cum, err)], [a + b for a, b in zip(cum, err)],
                    color=t["s1"], alpha=0.10, linewidth=0)
    ax.plot(x, cum, color=t["s1"], linewidth=2, solid_joinstyle="round", solid_capstyle="round", zorder=3)
    for xi, yi in zip(x, cum):
        dot(ax, xi, yi, t)
    ax.annotate(f"+{cum[-1]}", (x[-1], cum[-1]), xytext=(10, 0), textcoords="offset points",
                va="center", fontsize=10, fontweight="semibold", color=t["ink"])
    ax.set_xticks(x, labels)
    ax.set_xlim(-0.4, len(labels) - 0.4)
    ax.set_ylabel("Elo relative to v3", fontsize=9, color=t["muted"])
    titles(fig, t, "Strength progression, v3 → v10",
           "Cumulative Elo from chained matches (each version vs the previous one); band = 95% margin, errors summed")
    fig.subplots_adjust(left=0.1, right=0.93, top=0.8, bottom=0.12)
    save(fig, "elo_progression", mode)


def chart_per_version(mode):
    t = THEMES[mode]
    fig, ax = base(t)
    ax.set_ylim(0, max(e + r for _, e, r, _ in STEPS) + 25)
    ax.set_xlim(-0.6, len(STEPS) - 0.4)
    for i, (lab, elo, err, tc) in enumerate(STEPS):
        ax.bar(i, elo, width=0.28, color=t["s1"], linewidth=0, zorder=3)
        ax.errorbar(i, elo, yerr=err, fmt="none", ecolor=t["muted"], elinewidth=1, capsize=3, zorder=4)
        ax.annotate(f"+{elo}", (i, elo + err), xytext=(0, 4), textcoords="offset points",
                    ha="center", va="bottom", fontsize=9, color=t["ink"])
    ax.set_xticks(range(len(STEPS)), [f"{s[0]}\n{s[3]}" for s in STEPS])
    ax.set_ylabel("Elo vs previous version", fontsize=9, color=t["muted"])
    titles(fig, t, "Elo gained by each version",
           "100-game matches vs the previous version (v9: 50 games); whiskers = 95% margin; label under each version = time control")
    fig.subplots_adjust(left=0.1, right=0.97, top=0.8, bottom=0.16)
    save(fig, "elo_per_version", mode)


def chart_vs_sf(mode):
    t = THEMES[mode]
    fig, ax = base(t)
    ax.axhline(50, color=t["axis"], linewidth=1, zorder=2)
    ax.annotate("50% = equal\nstrength", (len(VS_SF) - 0.45, 50), xytext=(0, -4), textcoords="offset points",
                ha="left", va="top", fontsize=8, color=t["muted"])
    for i, (lab, elo, err, to, p) in enumerate(VS_SF):
        lo, hi = score(elo - err), score(elo + err)
        ax.plot([i, i], [lo, hi], color=t["s1"], linewidth=2, solid_capstyle="round", alpha=0.35, zorder=3)
        ax.plot(i, p, "o", markersize=9, color=t["s1"], markeredgecolor=t["surface"], markeredgewidth=2, zorder=4)
        ax.annotate(f"{p:.1f}%" + ("*" if to else ""), (i, p), xytext=(10, 0), textcoords="offset points",
                    va="center", fontsize=9, color=t["ink"])
    ax.set_xticks(range(len(VS_SF)), [s[0] for s in VS_SF])
    ax.set_xlim(-0.5, len(VS_SF) + 0.2)
    ax.set_ylim(25, 75)
    ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda v, _: f"{v:.0f}%"))
    ax.set_ylabel("Score vs Stockfish 2000", fontsize=9, color=t["muted"])
    titles(fig, t, "Score against Stockfish (UCI_Elo 2000)",
           "100 games each at 10+0.1; vertical line = 95% interval; * includes Stockfish losses on time (v7: 5, v8: 3); v9 not tested")
    fig.subplots_adjust(left=0.1, right=0.95, top=0.8, bottom=0.1)
    save(fig, "vs_sf2000", mode)


def chart_depth(mode):
    """Average search depth per version in 10+0.1 games (middlegame, moves 10-40)."""
    t = THEMES[mode]
    rows = list(csv.DictReader(open(os.path.join(DATA, "depth_per_version.csv"))))
    order = ["v5", "v6", "v7", "v8", "v9", "v10.1", "v10.2", "v10.3"]
    rows = sorted([r for r in rows if r["version"] in order], key=lambda r: order.index(r["version"]))
    fig, ax = base(t)
    xs = list(range(len(rows)))
    ys = [float(r["avg_depth_moves_10_40"]) for r in rows]
    ax.plot(xs, ys, color=t["s1"], linewidth=2, solid_joinstyle="round", zorder=3)
    for x, y in zip(xs, ys):
        dot(ax, x, y, t)
        ax.annotate(f"{y:.2f}", (x, y), xytext=(0, 9), textcoords="offset points", ha="center",
                    fontsize=9, color=t["ink"])
    notes = {"v6": "check extension:\nextended plies\nnot counted", "v10.2": "king safety:\nslower evaluation"}
    for x, r in zip(xs, rows):
        if r["version"] in notes:
            ax.annotate(notes[r["version"]], (x, float(r["avg_depth_moves_10_40"])), xytext=(0, -14),
                        textcoords="offset points", ha="center", va="top", fontsize=8, color=t["muted"])
    labels = [r["version"] if r["version"] != "v10.3" else "v10.3\n(= v10)" for r in rows]
    ax.set_xticks(xs, labels)
    ax.set_xlim(-0.5, len(rows) - 0.5)
    ax.set_ylim(4.0, 6.2)
    ax.set_ylabel("Average depth (plies)", fontsize=9, color=t["muted"])
    titles(fig, t, "Search depth per version",
           "Average depth reached per move, moves 10-40, all games at 10+0.1 (from cutechess PGNs; v3-v4 did not report depth)")
    fig.subplots_adjust(left=0.1, right=0.97, top=0.8, bottom=0.14)
    save(fig, "depth_per_version", mode)


def chart_nps(mode):
    """Aggregate nodes per second at fixed depth 6 over the 6 bench positions (tools/bench_versions.py)."""
    t = THEMES[mode]
    nodes, secs = {}, {}
    for r in csv.DictReader(open(os.path.join(DATA, "bench_versions.csv"))):
        if r["nodes"]:      # v3 and v4 do not report nodes
            nodes[r["version"]] = nodes.get(r["version"], 0) + int(r["nodes"])
            secs[r["version"]] = secs.get(r["version"], 0.0) + float(r["seconds"])
    versions = sorted(nodes, key=lambda v: int(v[1:]))
    nps = [nodes[v] / secs[v] / 1e6 for v in versions]
    fig, ax = base(t)
    for i, (v, y) in enumerate(zip(versions, nps)):
        ax.bar(i, y, width=0.5, color=t["s2"] if v == "v10" else t["s1"], linewidth=0, zorder=3)
        ax.annotate(f"{y:.2f} M", (i, y), xytext=(0, 4), textcoords="offset points",
                    ha="center", va="bottom", fontsize=9, color=t["ink"])
    ax.set_xticks(range(len(versions)), versions)
    ax.set_xlim(-0.6, len(versions) - 0.4)
    ax.set_ylim(0, max(nps) * 1.2)
    ax.set_ylabel("Million nodes per second", fontsize=9, color=t["muted"])
    titles(fig, t, "Search speed per version",
           "Total nodes / total time, depth 6, 6 positions, single thread, fastest of 3 runs (v3-v4 do not report nodes)")
    fig.subplots_adjust(left=0.1, right=0.97, top=0.8, bottom=0.1)
    save(fig, "nps_per_version", mode)


def chart_lichess(mode):
    """Daily Lichess rating from the Lichess API (months are 0-based in the API)."""
    t = THEMES[mode]
    hist = {h["name"]: h["points"] for h in json.load(open(os.path.join(DATA, "lichess_rating_history.json")))}
    # skip the very first game of July (provisional rating before any result)
    def series(name):
        pts = [(datetime.date(y, m + 1, d), r) for y, m, d, r in hist[name]]
        return [p for p in pts if p[0] >= datetime.date(2026, 7, 20)]
    bullet, blitz = series("bullet"), series("blitz")
    dates = [d for d, _ in bullet]
    xs = list(range(len(dates)))
    fig, ax = base(t)
    for pts, name, color in ((bullet, "Bullet", t["s1"]), (blitz, "Blitz", t["s2"])):
        ys = [r for _, r in pts]
        ax.plot(xs[1:], ys[1:], color=color, linewidth=2, solid_joinstyle="round", zorder=3, label=name)
        ax.plot(xs[:2], ys[:2], color=color, linewidth=1.5, linestyle=(0, (2, 3)), zorder=2)   # gap July -> October
        for x, y in zip(xs, ys):
            dot(ax, x, y, t, color)
        ax.annotate(f"{name} {ys[-1]}", (xs[-1], ys[-1]), xytext=(10, 0), textcoords="offset points",
                    va="center", fontsize=9, color=t["ink"])
    version_on = {datetime.date(2026, 7, 25): "early\nversions", datetime.date(2026, 10, 17): "v4",
                  datetime.date(2026, 10, 18): "v4 → v5", datetime.date(2026, 10, 19): "v5 → v7",
                  datetime.date(2026, 10, 20): "v7 → v9", datetime.date(2026, 10, 21): "v9",
                  datetime.date(2026, 10, 22): "v10.1 / v10"}
    ax.set_xticks(xs, [d.strftime("%b %d") + "\n" + version_on.get(d, "") for d in dates])
    ax.set_xlim(-0.3, len(xs) - 0.2)
    ax.set_ylim(1600, 2000)
    leg = ax.legend(loc="lower right", frameon=False, fontsize=9, handlelength=1.8)
    for txt in leg.get_texts():
        txt.set_color(t["ink2"])
    ax.set_ylabel("Lichess rating", fontsize=9, color=t["muted"])
    titles(fig, t, "TorrinoBOT on Lichess",
           "Daily rating (Lichess API); dashed = gap between the early versions (July) and v4 onwards (October)")
    fig.subplots_adjust(left=0.1, right=0.86, top=0.8, bottom=0.2)
    save(fig, "lichess_rating", mode)


for m in ("light", "dark"):
    chart_progression(m)
    chart_per_version(m)
    chart_vs_sf(m)
    chart_depth(m)
    chart_nps(m)
    chart_lichess(m)
print("done:", sorted(f for f in os.listdir(OUT) if f.endswith(".png")))
