import os
import random

OUT_DIR = "data"

#CLOSEST PAIR OF POINTS (cp01 - cp10)

def uniform(rng, n):
    # cp01-cp04: uniform real coordinates in [0, 10^6]
    return [(rng.uniform(0, 10**6), rng.uniform(0, 10**6)) for _ in range(n)]

def clustered(rng, n):
    # cp05: points around 10 random centers (Gaussian noise, clipped to [0, 10^6])
    centers = [(rng.uniform(0, 10**6), rng.uniform(0, 10**6)) for _ in range(10)]
    pts = []
    for _ in range(n):
        cx, cy = rng.choice(centers)
        x = min(max(rng.gauss(cx, 20000), 0), 10**6)
        y = min(max(rng.gauss(cy, 20000), 0), 10**6)
        pts.append((x, y))
    return pts

def duplicates(rng, n):
    # cp06: integer coords in [0, 100] -> many duplicates, closest distance 0
    return [(rng.randint(0, 100), rng.randint(0, 100)) for _ in range(n)]

def vertical_line(rng, n):
    # cp07: all points on one vertical line (same x), random y
    return [(500000.0, rng.uniform(0, 10**6)) for _ in range(n)]

def grid(rng, n):
    # cp08: equally spaced 250 x 400 grid = 100,000 points, many tied closest pairs
    rows, cols, step = 250, 400, 1000
    pts = [(c * step, r * step) for r in range(rows) for c in range(cols)]
    rng.shuffle(pts)
    return pts

def across_midline(rng, n):
    # cp09: closest pair is split by the median-x dividing line
    pts = [(rng.uniform(0, 10**6), rng.uniform(0, 10**6)) for _ in range(n - 2)]
    xs = sorted(p[0] for p in pts)
    m = xs[len(xs) // 2]
    y = rng.uniform(0, 10**6)
    pts.append((m - 0.0005, y))
    pts.append((m + 0.0005, y))
    rng.shuffle(pts)
    return pts

def small_demo(rng, n):
    # cp10: 101 points for the step-by-step demo
    return [(round(rng.uniform(0, 1000), 2), round(rng.uniform(0, 1000), 2)) for _ in range(n)]

CP_FILES = {
    "cp01": (uniform,         10**3, 1001),
    "cp02": (uniform,         10**4, 1002),
    "cp03": (uniform,         10**5, 1003),
    "cp04": (uniform,         10**6, 1004),
    "cp05": (clustered,       10**6, 1005),
    "cp06": (duplicates,      10**6, 1006),
    "cp07": (vertical_line,   10**5, 1007),
    "cp08": (grid,            10**5, 1008),
    "cp09": (across_midline,  10**6, 1009),
    "cp10": (small_demo,      101,   1010),
}

def fmt(v):
    return str(v) if isinstance(v, int) else f"{v:.6f}"

def write_cp(name, pts):
    path = os.path.join(OUT_DIR, f"{name}.txt")
    with open(path, "w") as f:
        f.write(f"{len(pts)}\n")
        f.write("\n".join(f"{fmt(x)} {fmt(y)}" for x, y in pts))
        f.write("\n")
    return path

# INTEGER MULTIPLICATION (im01 - im10)
def rand_digits(rng, n):
    # n random digits, no leading zero
    if n == 1:
        return rng.choice("123456789")
    return rng.choice("123456789") + "".join(rng.choices("0123456789", k=n - 1))

def im_random(n):
    # im01-im04: two random numbers, n digits each
    def gen(rng):
        return rand_digits(rng, n), rand_digits(rng, n)
    return gen

def im_unequal(rng):
    # im05: 1,000,000 digits and 10 digits
    return rand_digits(rng, 10**6), rand_digits(rng, 10)

def im_all_nines(rng):
    # im06: 100,000 digits, one number is all 9s
    return "9" * 100000, rand_digits(rng, 100000)

def im_odd_zeros(rng):
    # im07: 99,999 digits (odd length), mostly zeros
    def sparse(n):
        first = rng.choice("123456789")
        rest = "".join(rng.choices("0123456789", weights=[80] + [3] * 9, k=n - 1))
        return first + rest
    return sparse(99999), sparse(99999)

def im_powers_of_ten(rng):
    # im08: both numbers are 10^999999 (1 followed by 999,999 zeros)
    p = "1" + "0" * 999999
    return p, p

def im_zero(rng):
    # im09: one number is 0, the other has 1,000,000 digits
    return "0", rand_digits(rng, 10**6)

def im_small_demo(rng):
    # im10: 101 digits each, for the step-by-step demo
    return rand_digits(rng, 101), rand_digits(rng, 101)

IM_FILES = {
    "im01": (im_random(10**3), 2001),
    "im02": (im_random(10**4), 2002),
    "im03": (im_random(10**5), 2003),
    "im04": (im_random(10**6), 2004),
    "im05": (im_unequal,       2005),
    "im06": (im_all_nines,     2006),
    "im07": (im_odd_zeros,     2007),
    "im08": (im_powers_of_ten, 2008),
    "im09": (im_zero,          2009),
    "im10": (im_small_demo,    2010),
}

def write_im(name, a, b):
    path = os.path.join(OUT_DIR, f"{name}.txt")
    with open(path, "w") as f:
        f.write(a + "\n" + b + "\n")
    return path

#MAIN

def report(name, desc, seed, path, sf):
    size_mb = os.path.getsize(path) / 1e6
    sf.write(f"{name}: {desc}, seed={seed}\n")
    print(f"{name}: {desc}, seed={seed}, {size_mb:.2f} MB, "
          f"{'SUBMIT' if size_mb < 1 else 'too big, do not submit'}")

if __name__ == "__main__":
    os.makedirs(OUT_DIR, exist_ok=True)
    with open("seeds.txt", "w") as sf:
        for name, (fn, n, seed) in CP_FILES.items():
            rng = random.Random(seed)          # own RNG per file -> reproducible alone
            pts = fn(rng, n)
            path = write_cp(name, pts)
            report(name, f"n={len(pts)}", seed, path, sf)

        for name, (fn, seed) in IM_FILES.items():
            rng = random.Random(seed)
            a, b = fn(rng)
            path = write_im(name, a, b)
            report(name, f"digits={len(a)} and {len(b)}", seed, path, sf)