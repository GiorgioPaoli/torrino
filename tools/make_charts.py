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
         ("v10", 135, 69, "10+0.1"), ("v11", 121, 45, "10+0.1"), ("v12", 143, 67, "10+0.1"),
         ("v13", 70, 64, "10+0.1")]
# vs Stockfish UCI_Elo 2000, 100 games each: (label, elo, err, SF losses on time, actual score %)
VS_SF = [("v5", -63, 67, 0, 41.0), ("v6", 24, 66, 0, 53.5), ("v7", 20, 67, 5, 53.0),
         ("v8", 31, 67, 3, 54.5), ("v10", 38, 67, 0, 55.5), ("v12", 155, 73, 0, 71.0),
         ("v13", 235, 86, 0, 79.5)]
# v12 and v13 against several Stockfish levels, 100 games each: (UCI_Elo, elo, err, actual score %)
LADDER = {"v12": [(2000, 155, 73, 71.0), (2100, 96, 70, 63.5)],
          "v13": [(2000, 235, 86, 79.5), (2100, 186, 79, 74.5), (2200, 100, 71, 64.0),
                  (2300, 85, 68, 62.0), (2400, 14, 66, 52.0)]}


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
    titles(fig, t, "Strength progression, v3 → v13",
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
           "100-game matches vs the previous version (v9: 50 games, v11: 200); vertical line = uncertainty (95% confidence); label = time control")
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
    ax.set_ylim(25, 95)
    ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda v, _: f"{v:.0f}%"))
    ax.set_ylabel("Score vs Stockfish 2000", fontsize=9, color=t["muted"])
    titles(fig, t, "Score against the same opponent: Stockfish UCI_Elo 2000",
           "100 games each at 10+0.1; vertical line = 95% interval; * includes Stockfish losses on time (v7: 5, v8: 3); v9, v11 not tested")
    fig.subplots_adjust(left=0.1, right=0.95, top=0.8, bottom=0.1)
    save(fig, "vs_sf2000", mode)


def style_ax(ax, t):
    ax.set_facecolor(t["surface"])
    for s in ("top", "right", "left"):
        ax.spines[s].set_visible(False)
    ax.spines["bottom"].set_color(t["axis"])
    ax.tick_params(colors=t["muted"], labelsize=9, length=0)
    ax.yaxis.grid(True, color=t["grid"], linewidth=1, linestyle="-")
    ax.set_axisbelow(True)
    ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda v, _: f"{v:.0f}%"))
    ax.set_ylim(25, 95)
    ax.axhline(50, color=t["axis"], linewidth=1, zorder=2)


def chart_stockfish(mode):
    """One figure, two panels: all versions vs Stockfish 2000 (progress) and v12/v13 vs 2000-2400 (current level)."""
    t = THEMES[mode]
    plt.rcParams["font.family"] = FONT
    fig, (a1, a2) = plt.subplots(1, 2, figsize=(11.0, 4.6), dpi=150, gridspec_kw={"width_ratios": [1.15, 1]})
    fig.patch.set_facecolor(t["surface"])
    for ax in (a1, a2):
        style_ax(ax, t)
    # left: the same opponent across versions
    for i, (lab, elo, err, to, p) in enumerate(VS_SF):
        a1.plot([i, i], [score(elo - err), score(elo + err)], color=t["s1"], linewidth=2, alpha=0.35,
                solid_capstyle="round", zorder=3)
        dot(a1, i, p, t)
        a1.annotate(f"{p:.1f}%" + ("*" if to else ""), (i, p), xytext=(0, 10), textcoords="offset points",
                    ha="center", fontsize=8.5, color=t["ink"])
    a1.set_xticks(range(len(VS_SF)), [s[0] for s in VS_SF])
    a1.set_xlim(-0.5, len(VS_SF) - 0.5)
    a1.set_title("Same opponent (Stockfish 2000), all versions", fontsize=10, color=t["ink2"], loc="left")
    # right: stronger opponents for v12 and v13
    for name, color, dx in (("v12", t["s2"], -8), ("v13", t["s1"], 8)):
        pts = LADDER[name]
        xs = [lvl + dx for lvl, *_ in pts]
        for x, (lvl, elo, err, p) in zip(xs, pts):
            a2.plot([x, x], [score(elo - err), score(elo + err)], color=color, linewidth=2, alpha=0.35,
                    solid_capstyle="round", zorder=3)
        a2.plot(xs, [p for *_, p in pts], color=color, linewidth=2, zorder=3, label=name)
        for x, (lvl, elo, err, p) in zip(xs, pts):
            dot(a2, x, p, t, color)
            a2.annotate(f"{p:.1f}%", (x, p), xytext=(0, 10), textcoords="offset points", ha="center",
                        fontsize=8.5, color=t["ink"])
    a2.set_xticks([2000, 2100, 2200, 2300, 2400], ["2000", "2100", "2200", "2300", "2400"])
    a2.set_xlim(1950, 2450)
    a2.set_xlabel("Stockfish UCI_Elo", fontsize=9, color=t["muted"])
    a2.set_title("Stronger Stockfish levels, v12 and v13", fontsize=10, color=t["ink2"], loc="left")
    leg = a2.legend(loc="upper right", frameon=False, fontsize=9, handlelength=1.8)
    for txt in leg.get_texts():
        txt.set_color(t["ink2"])
    fig.text(0.05, 0.95, "Score against Stockfish", fontsize=13, fontweight="semibold", color=t["ink"],
             ha="left", va="top")
    fig.text(0.05, 0.885, "Score = (wins + half the draws) / games, 100 games per point at 10+0.1; vertical line = "
             "uncertainty (95% confidence); 50% = equal strength; * includes Stockfish losses on time", fontsize=9, color=t["ink2"],
             ha="left", va="top")
    fig.subplots_adjust(left=0.06, right=0.98, top=0.76, bottom=0.13, wspace=0.12)
    save(fig, "vs_stockfish", mode)


def chart_speed(mode):
    """Time to reach depth 7 per version (total over the 6 bench positions), log scale; NPS in the labels."""
    t = THEMES[mode]
    secs, nodes = {}, {}
    for r in csv.DictReader(open(os.path.join(DATA, "bench_all_versions.csv"))):
        v = r["version"]
        secs[v] = secs.get(v, 0.0) + float(r["seconds"])
        if r["nodes"]:
            nodes[v] = nodes.get(v, 0) + int(r["nodes"])
    versions = sorted(secs, key=lambda v: int(v[1:]))
    fig, ax = base(t)
    ax.set_yscale("log")
    ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda v, _: f"{v:g} s"))
    highlight = {"v11": "speed", "v12": "TT", "v13": "pruning"}
    for i, v in enumerate(versions):
        color = t["s2"] if v in highlight else t["s1"]
        ax.bar(i, secs[v], width=0.55, color=color, linewidth=0, zorder=3)
        label = f"{secs[v]:.1f} s" if secs[v] >= 1 else f"{secs[v]:.2f} s"
        if v in nodes:
            label += f"\n{nodes[v] / secs[v] / 1e6:.1f}M nps"
        ax.annotate(label, (i, secs[v]), xytext=(0, 4), textcoords="offset points", ha="center", va="bottom",
                    fontsize=8, color=t["ink"])
    ax.set_xticks(range(len(versions)), [v + ("\n" + highlight[v] if v in highlight else "") for v in versions])
    ax.set_xlim(-0.6, len(versions) - 0.4)
    ax.set_ylim(min(secs.values()) / 2, max(secs.values()) * 2.5)
    ax.tick_params(which="minor", length=0)
    ax.yaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
    ax.set_ylabel("Time to depth 7 (log scale)", fontsize=9, color=t["muted"])
    titles(fig, t, "Time to reach depth 7, per version",
           "Total over the 6 standard test positions, single thread, fastest of 3 runs; NPS = nodes per second "
           "(v3-v4 do not report nodes)")
    fig.subplots_adjust(left=0.1, right=0.97, top=0.8, bottom=0.16)
    save(fig, "speed_per_version", mode)


def chart_sf_ladder(mode):
    """v12 and v13 against Stockfish limited to increasing UCI_Elo levels."""
    t = THEMES[mode]
    fig, ax = base(t)
    ax.axhline(50, color=t["axis"], linewidth=1, zorder=2)
    ax.annotate("50% = equal strength", (2440, 50), xytext=(0, -4), textcoords="offset points",
                ha="right", va="top", fontsize=8, color=t["muted"])
    for name, color, dx in (("v12", t["s2"], -6), ("v13", t["s1"], 6)):
        pts = LADDER[name]
        xs = [lvl + dx for lvl, *_ in pts]
        ys = [p for *_, p in pts]
        for x, (lvl, elo, err, p) in zip(xs, pts):
            ax.plot([x, x], [score(elo - err), score(elo + err)], color=color, linewidth=2, alpha=0.35,
                    solid_capstyle="round", zorder=3)
        ax.plot(xs, ys, color=color, linewidth=2, zorder=3, label=name)
        for x, y in zip(xs, ys):
            dot(ax, x, y, t, color)
            ax.annotate(f"{y:.1f}%", (x, y), xytext=(0, 9), textcoords="offset points", ha="center",
                        fontsize=8.5, color=t["ink"])
    ax.set_xticks([2000, 2100, 2200, 2300, 2400], ["SF 2000", "SF 2100", "SF 2200", "SF 2300", "SF 2400"])
    ax.set_xlim(1950, 2450)
    ax.set_ylim(25, 95)
    ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda v, _: f"{v:.0f}%"))
    ax.set_ylabel("Score", fontsize=9, color=t["muted"])
    leg = ax.legend(loc="upper right", frameon=False, fontsize=9, handlelength=1.8)
    for txt in leg.get_texts():
        txt.set_color(t["ink2"])
    titles(fig, t, "Against stronger Stockfish levels",
           "100 games per point at 10+0.1, Stockfish with UCI_LimitStrength; vertical line = 95% interval")
    fig.subplots_adjust(left=0.1, right=0.97, top=0.8, bottom=0.1)
    save(fig, "vs_sf_ladder", mode)


def chart_depth(mode):
    """Average search depth per version in 10+0.1 games (middlegame, moves 10-40)."""
    t = THEMES[mode]
    rows = list(csv.DictReader(open(os.path.join(DATA, "depth_per_version.csv"))))
    order = ["v5", "v6", "v7", "v8", "v9", "v10", "v11", "v12", "v13"]
    rows = sorted([r for r in rows if r["version"] in order], key=lambda r: order.index(r["version"]))
    fig, ax = base(t)
    xs = list(range(len(rows)))
    ys = [float(r["avg_depth_moves_10_40"]) for r in rows]
    ax.plot(xs, ys, color=t["s1"], linewidth=2, solid_joinstyle="round", zorder=3)
    for x, y in zip(xs, ys):
        dot(ax, x, y, t)
        ax.annotate(f"{y:.2f}", (x, y), xytext=(0, 9), textcoords="offset points", ha="center",
                    fontsize=9, color=t["ink"])
    notes = {"v6": "check extension:\nextended plies\nnot counted", "v10": "evaluation rework:\nslower nodes",
             "v12": "transposition\ntable", "v13": "null move,\nLMR"}
    for x, r in zip(xs, rows):
        if r["version"] in notes:
            # the steep v11 → v13 segment would cross notes placed below the point: put those on the right
            dx, ha = (14, "left") if r["version"] in ("v12", "v13") else (0, "center")
            ax.annotate(notes[r["version"]], (x, float(r["avg_depth_moves_10_40"])), xytext=(dx, -14),
                        textcoords="offset points", ha=ha, va="top", fontsize=8, color=t["muted"])
    labels = [r["version"] for r in rows]
    ax.set_xticks(xs, labels)
    ax.set_xlim(-0.5, len(rows) + 0.1)
    ax.set_ylim(4.0, 8.6)
    ax.set_ylabel("Average depth (plies)", fontsize=9, color=t["muted"])
    titles(fig, t, "Search depth per version",
           "Average depth per move, moves 10-40, all games at 10+0.1 (cutechess PGNs in docs/matches; v3-v4 did not report depth)")
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
                  datetime.date(2026, 10, 22): "v10.1 / v10", datetime.date(2026, 10, 23): "v10",
                  datetime.date(2026, 10, 24): "v11", datetime.date(2026, 10, 25): "v12",
                  datetime.date(2026, 10, 26): "v13"}
    ax.set_xticks(xs, [d.strftime("%b %d") + "\n" + version_on.get(d, "") for d in dates])
    ax.set_xlim(-0.3, len(xs) - 0.2)
    ax.set_ylim(1600, 2100)
    leg = ax.legend(loc="lower right", frameon=False, fontsize=9, handlelength=1.8)
    for txt in leg.get_texts():
        txt.set_color(t["ink2"])
    ax.set_ylabel("Lichess rating", fontsize=9, color=t["muted"])
    titles(fig, t, "TorrinoBOT on Lichess",
           f"Daily rating (Lichess API) up to {dates[-1].strftime('%b %d, %Y')}; dashed = gap between July and October")
    fig.subplots_adjust(left=0.1, right=0.86, top=0.8, bottom=0.2)
    save(fig, "lichess_rating", mode)


for m in ("light", "dark"):
    chart_progression(m)
    chart_per_version(m)
    chart_stockfish(m)
    chart_depth(m)
    if os.path.exists(os.path.join(DATA, "bench_all_versions.csv")):
        chart_speed(m)
    chart_lichess(m)
print("done:", sorted(f for f in os.listdir(OUT) if f.endswith(".png")))
