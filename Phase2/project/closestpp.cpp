// closest.cpp  -  O(n log n) divide and conquer closest pair (Kleinberg 5.4)
// Build: g++ -O2 -std=c++17 -o closest closest.cpp
// Run:   ./closest data/cp01.txt            (result + time)
//        ./closest data/cp10.txt --trace    (JSON lines for the GUI, n <= 1000)
//        ./closest data/cp02.txt --check    (compare with O(n^2) brute force, n <= 20000)

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <vector>
#include <chrono>
using namespace std;

struct P { double x, y; };
struct Res { double d; P a, b; };

static bool TRACE = false;
static vector<P> tmpBuf, stripBuf;

static inline double dist(const P& a, const P& b) {
    double dx = a.x - b.x, dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}
static inline void consider(Res& r, const P& a, const P& b) {
    double d = dist(a, b);
    if (d < r.d) { r.d = d; r.a = a; r.b = b; }
}

// own merge sort by x (no std::sort, as the spec forbids library sorting)
static void msortX(P* A, int n) {
    if (n < 2) return;
    int m = n / 2;
    msortX(A, m);
    msortX(A + m, n - m);
    int i = 0, j = m, k = 0;
    while (i < m && j < n) {
        if (A[j].x < A[i].x || (A[j].x == A[i].x && A[j].y < A[i].y)) tmpBuf[k++] = A[j++];
        else tmpBuf[k++] = A[i++];
    }
    while (i < m) tmpBuf[k++] = A[i++];
    while (j < n) tmpBuf[k++] = A[j++];
    memcpy(A, tmpBuf.data(), (size_t)n * sizeof(P));
}

static void pt(const P& p) { printf("[%.6f,%.6f]", p.x, p.y); }

// On entry A[0..n) is sorted by x. On exit it is sorted by y (merged on the way up).
// lo = index of A[0] in the x-sorted array (only used for trace output).
static Res solve(P* A, int lo, int n) {
    if (n <= 3) {
        Res r{INFINITY, {0, 0}, {0, 0}};
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++) consider(r, A[i], A[j]);
        for (int i = 1; i < n; i++) {                    // insertion sort by y
            P key = A[i]; int j = i - 1;
            while (j >= 0 && A[j].y > key.y) { A[j + 1] = A[j]; j--; }
            A[j + 1] = key;
        }
        if (TRACE) {
            printf("{\"event\":\"base\",\"lo\":%d,\"hi\":%d,\"delta\":%.9f,\"a\":", lo, lo + n - 1, r.d);
            pt(r.a); printf(",\"b\":"); pt(r.b); printf("}\n");
        }
        return r;
    }

    int mid = n / 2;
    double midx = A[mid].x;                              // dividing line (taken before merging)
    if (TRACE)
        printf("{\"event\":\"split\",\"lo\":%d,\"hi\":%d,\"mid\":%d,\"midx\":%.6f}\n", lo, lo + n - 1, lo + mid, midx);

    Res L = solve(A, lo, mid);
    Res R = solve(A + mid, lo + mid, n - mid);
    Res r = (L.d <= R.d) ? L : R;                        // delta = min(left, right)

    // merge the two y-sorted halves
    {
        int i = 0, j = mid, k = 0;
        while (i < mid && j < n) {
            if (A[j].y < A[i].y) tmpBuf[k++] = A[j++];
            else tmpBuf[k++] = A[i++];
        }
        while (i < mid) tmpBuf[k++] = A[i++];
        while (j < n) tmpBuf[k++] = A[j++];
        memcpy(A, tmpBuf.data(), (size_t)n * sizeof(P));
    }

    // strip: points within delta of the dividing line, already in y-order
    int m = 0;
    for (int i = 0; i < n; i++)
        if (fabs(A[i].x - midx) < r.d) stripBuf[m++] = A[i];

    if (TRACE) {
        printf("{\"event\":\"merge\",\"lo\":%d,\"hi\":%d,\"midx\":%.6f,\"delta_left\":%.9f,"
               "\"delta_right\":%.9f,\"delta\":%.9f,\"strip\":[",
               lo, lo + n - 1, midx, L.d, R.d, r.d);
        for (int i = 0; i < m; i++) { if (i) printf(","); pt(stripBuf[i]); }
        printf("]}\n");
    }

    for (int i = 0; i < m; i++) {
        for (int j = i + 1; j < m && stripBuf[j].y - stripBuf[i].y < r.d; j++) {
            double d = dist(stripBuf[i], stripBuf[j]);
            if (d < r.d) {
                r.d = d; r.a = stripBuf[i]; r.b = stripBuf[j];
                if (TRACE) {
                    printf("{\"event\":\"strip_update\",\"a\":"); pt(r.a);
                    printf(",\"b\":"); pt(r.b); printf(",\"d\":%.9f}\n", d);
                }
            }
        }
    }
    if (TRACE)
        printf("{\"event\":\"combine_done\",\"lo\":%d,\"hi\":%d,\"delta\":%.9f}\n", lo, lo + n - 1, r.d);
    return r;
}

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s file [--trace] [--check]\n", argv[0]); return 1; }
    bool check = false;
    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "--trace")) TRACE = true;
        else if (!strcmp(argv[i], "--check")) check = true;
    }

    // read the whole file at once, then parse
    FILE* f = fopen(argv[1], "rb");
    if (!f) { fprintf(stderr, "cannot open %s\n", argv[1]); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); rewind(f);
    vector<char> buf(sz + 1);
    size_t got = fread(buf.data(), 1, sz, f);
    buf[got] = '\0';
    fclose(f);

    char* p = buf.data();
    char* end;
    long n = strtol(p, &end, 10); p = end;
    vector<P> pts;
    pts.reserve(n);
    for (long i = 0; i < n; i++) {
        double x = strtod(p, &end); if (end == p) break; p = end;
        double y = strtod(p, &end); if (end == p) break; p = end;
        pts.push_back({x, y});
    }
    n = (long)pts.size();
    if (n < 2) { fprintf(stderr, "need at least 2 points\n"); return 1; }
    if (TRACE && n > 1000) { fprintf(stderr, "--trace only for n <= 1000\n"); return 1; }

    vector<P> orig;
    if (check && n <= 20000) orig = pts;

    tmpBuf.resize(n);
    stripBuf.resize(n);

    auto t0 = chrono::steady_clock::now();
    msortX(pts.data(), (int)n);
    if (TRACE) {
        printf("{\"event\":\"sorted\",\"n\":%ld,\"points\":[", n);
        for (long i = 0; i < n; i++) { if (i) printf(","); pt(pts[i]); }
        printf("]}\n");
    }
    Res r = solve(pts.data(), 0, (int)n);
    auto t1 = chrono::steady_clock::now();
    double ms = chrono::duration<double, milli>(t1 - t0).count();

    if (TRACE) {
        printf("{\"event\":\"result\",\"a\":"); pt(r.a); printf(",\"b\":"); pt(r.b);
        printf(",\"distance\":%.9f,\"time_ms\":%.3f}\n", r.d, ms);
        return 0;
    }

    printf("n %ld\npair %.6f %.6f %.6f %.6f\ndistance %.9f\ntime_ms %.3f\n",
           n, r.a.x, r.a.y, r.b.x, r.b.y, r.d, ms);

    if (check) {
        if (n > 20000) { printf("check skipped (n > 20000)\n"); return 0; }
        Res b{INFINITY, {0, 0}, {0, 0}};
        for (long i = 0; i < n; i++)
            for (long j = i + 1; j < n; j++) consider(b, orig[i], orig[j]);
        printf("brute_distance %.9f\ncheck %s\n", b.d, fabs(b.d - r.d) < 1e-9 ? "OK" : "MISMATCH");
    }
    return 0;
}