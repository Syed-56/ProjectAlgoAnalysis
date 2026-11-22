import csv
import math
import subprocess
import sys

CLOSEST = "closestpp"      # your closest-pair executable name
KARA = "karatsuba"         # your multiplication executable name
DATA = "data"
PY_EXACT_LIMIT = 200_000   # digits; above this, rely on the program's mod_check (raise it if you want)

sys.set_int_max_str_digits(0)

def run(cmd):
    r = subprocess.run(cmd, capture_output=True, text=True, timeout=1800)
    return r.stdout

def parse(out):
    d = {}
    for line in out.splitlines():
        parts = line.split(None, 1)
        if len(parts) == 2:
            d[parts[0]] = parts[1].strip()
    return d

def verify_cp(path, pair, dist):
    """Independent check: the pair has the reported distance, and no pair is closer (grid method)."""
    ax, ay, bx, by = map(float, pair.split())
    if abs(math.hypot(ax - bx, ay - by) - dist) > 1e-5:
        return False
    if dist == 0:
        return ax == bx and ay == by          # a duplicate point is trivially optimal
    with open(path) as f:
        n = int(f.readline())
        v = f.read().split()
    cell = dist
    grid = {}
    for i in range(n):
        x, y = float(v[2 * i]), float(v[2 * i + 1])
        grid.setdefault((int(x // cell), int(y // cell)), []).append((x, y))
    lim = dist - 1e-9
    offs = [(1, -1), (1, 0), (1, 1), (0, 1)]
    for (cx, cy), pts in grid.items():
        for i in range(len(pts)):             # pairs inside the same cell
            for j in range(i + 1, len(pts)):
                if math.hypot(pts[i][0] - pts[j][0], pts[i][1] - pts[j][1]) < lim:
                    return False
        for dx, dy in offs:                   # pairs with neighbouring cells
            other = grid.get((cx + dx, cy + dy))
            if other:
                for p in pts:
                    for q in other:
                        if math.hypot(p[0] - q[0], p[1] - q[1]) < lim:
                            return False
    return True

rows = []

for k in range(1, 11):
    name = f"cp{k:02d}"
    path = f"{DATA}/{name}.txt"
    d = parse(run([CLOSEST, path, "--check"]))
    n = int(d["n"])
    dist = float(d["distance"])
    ok = verify_cp(path, d["pair"], dist)
    brute = d.get("check", "skipped (n > 20000)")
    rows.append([name, n, f"distance={dist:.9f}", d["time_ms"], f"grid check {'OK' if ok else 'FAIL'}; brute {brute}"])
    print(rows[-1])

for k in range(1, 11):
    name = f"im{k:02d}"
    path = f"{DATA}/{name}.txt"
    d = parse(run([KARA, path, "--check", "--out", f"product_{name}.txt"]))
    la, lb = int(d["digits_a"]), int(d["digits_b"])
    py = "skipped"
    if max(la, lb) <= PY_EXACT_LIMIT:
        a, b = open(path).read().split()[:2]
        py = "OK" if str(int(a) * int(b)) == open(f"product_{name}.txt").read().strip() else "FAIL"
    rows.append([name, f"{la} x {lb} digits", f"product has {d['digits_product']} digits",
                 d["time_ms"], f"mod {d['mod_check']}; python int {py}"])
    print(rows[-1])

with open("results.csv", "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["file", "input size", "result", "time_ms", "verification"])
    w.writerows(rows)

print("\nSaved results.csv")