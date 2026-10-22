"""Board diagrams (SVG) for the README case studies, generated with python-chess.

Usage: python tools/make_boards.py docs/images
Red arrow = move actually played by the engine (the mistake); green arrow = a better move.
"""
import sys, os, io
import chess, chess.svg, chess.pgn

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)
RED, GREEN = "#e34948cc", "#1baf7acc"


def save(name, board, arrows, orientation=chess.WHITE, lastmove=None):
    # every arrow must be a legal move in this position (catches wrong squares / blocked paths)
    for a in arrows:
        move = chess.Move(a.tail, a.head)
        if move not in board.legal_moves:
            raise ValueError(f"{name}: arrow {chess.square_name(a.tail)}-{chess.square_name(a.head)} is not a legal move")
    svg = chess.svg.board(board, arrows=arrows, orientation=orientation, lastmove=lastmove, size=360,
                          coordinates=True)
    with open(os.path.join(OUT, name), "w", encoding="utf-8") as fh:
        fh.write(svg)


# 1. King safety (Lichess, TorrinoBOT vs GroshBoss, 3+1): before 39.f4?? (Stockfish +4.2 -> +0.2)
b = chess.Board("3r2k1/1p3rpn/3p1p1p/P1pP3q/2N1P2P/2Q1PPR1/1PP3K1/7R w - - 5 39")
# red: f3-f4 (played, opens the king); green: Rh1-g1 (Stockfish's best move)
save("case_king_safety.svg", b, [chess.svg.Arrow(chess.F3, chess.F4, color=RED),
                                 chess.svg.Arrow(chess.H1, chess.G1, color=GREEN)])

# 2. Passed pawn (Lichess, maia9 vs TorrinoBOT, 1+1): before 27...Rf6? (Stockfish +4.4 for Black -> 0.0)
b = chess.Board("8/ppp2p2/7r/2kPp2P/P1n1P3/2K5/5P2/7R b - - 0 27")
save("case_passed_pawn.svg", b, [chess.svg.Arrow(chess.H6, chess.F6, color=RED),
                                 chess.svg.Arrow(chess.A7, chess.A6, color=GREEN)],
     orientation=chess.BLACK)

# 3. Basic mate not converted (cutechess, v10.1 vs v9, game 9): v9 a queen up, draw by the fifty-move rule
pgn_path = sys.argv[2] if len(sys.argv) > 2 else None
if pgn_path and os.path.exists(pgn_path):
    with open(pgn_path, encoding="utf-8", errors="replace") as fh:
        for i in range(9):
            g = chess.pgn.read_game(fh)
    end = g.end()
    save("case_kq_vs_k.svg", end.board(), [], lastmove=end.move)
    print("game 9:", g.headers["White"], "vs", g.headers["Black"], g.headers["Result"], "plies:", end.ply())
print("done")
