// karatsuba.cpp - Karatsuba integer multiplication with schoolbook cutoff
// Build: g++ -O2 -std=c++17 -o karatsuba karatsuba.cpp
// Run:   karatsuba data\im01.txt                 (product written to product_im01.txt, time printed)
//        karatsuba data\im10.txt --trace         (JSON lines for the GUI, up to 1000 digits)
//        karatsuba data\im02.txt --check         (verify the product)
//        karatsuba data\im01.txt --out my.txt    (choose the output file)

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <chrono>
using namespace std;

typedef uint64_t u64;
typedef vector<u64> Big;            // little-endian limbs, each < BASE, empty = 0

static u64 BASE = 1000000000ULL;    // normal mode: base 10^9
static int BASE_DIGITS = 9;
static size_t CUTOFF = 32;          // limbs; at or below this, use schoolbook
static bool TRACE = false;
static int nextId = 0;

static void trim(Big& a) { while (!a.empty() && a.back() == 0) a.pop_back(); }

static Big fromStr(const string& s) {
    Big r;
    int len = (int)s.size();
    for (int end = len; end > 0; end -= BASE_DIGITS) {
        int start = max(0, end - BASE_DIGITS);
        u64 v = 0;
        for (int i = start; i < end; i++) v = v * 10 + (u64)(s[i] - '0');
        r.push_back(v);
    }
    trim(r);
    return r;
}

static string toStr(const Big& a) {
    if (a.empty()) return "0";
    string s = to_string(a.back());
    char tmp[32];
    for (int i = (int)a.size() - 2; i >= 0; i--) {
        snprintf(tmp, sizeof tmp, "%0*llu", BASE_DIGITS, (unsigned long long)a[i]);
        s += tmp;
    }
    return s;
}

static Big add(const Big& a, const Big& b) {
    const Big& x = a.size() >= b.size() ? a : b;
    const Big& y = a.size() >= b.size() ? b : a;
    Big r(x.size() + 1, 0);
    u64 carry = 0;
    for (size_t i = 0; i < x.size(); i++) {
        u64 s = x[i] + (i < y.size() ? y[i] : 0) + carry;
        if (s >= BASE) { s -= BASE; carry = 1; } else carry = 0;
        r[i] = s;
    }
    r[x.size()] = carry;
    trim(r);
    return r;
}

// requires a >= b
static Big sub(const Big& a, const Big& b) {
    Big r(a);
    u64 borrow = 0;
    for (size_t i = 0; i < r.size(); i++) {
        u64 s = (i < b.size() ? b[i] : 0) + borrow;
        if (r[i] >= s) { r[i] -= s; borrow = 0; }
        else { r[i] = r[i] + BASE - s; borrow = 1; }
    }
    trim(r);
    return r;
}

// schoolbook with compile-time base so the division is fast
template <u64 B>
static Big schoolT(const Big& a, const Big& b) {
    Big r(a.size() + b.size(), 0);
    for (size_t i = 0; i < a.size(); i++) {
        u64 carry = 0, ai = a[i];
        for (size_t j = 0; j < b.size(); j++) {
            u64 cur = r[i + j] + ai * b[j] + carry;     // < B^2, fits in 64 bits
            r[i + j] = cur % B;
            carry = cur / B;
        }
        r[i + b.size()] = carry;
    }
    trim(r);
    return r;
}

static Big school(const Big& a, const Big& b) {
    if (a.empty() || b.empty()) return Big();
    return BASE == 1000000000ULL ? schoolT<1000000000ULL>(a, b) : schoolT<10ULL>(a, b);
}

// r += x * BASE^off   (r must be big enough)
static void addAt(Big& r, const Big& x, size_t off) {
    u64 carry = 0;
    for (size_t i = 0; i < x.size() || carry; i++) {
        u64 s = r[off + i] + (i < x.size() ? x[i] : 0) + carry;
        if (s >= BASE) { s -= BASE; carry = 1; } else carry = 0;
        r[off + i] = s;
    }
}

static Big kara(const Big& a, const Big& b, int parent) {
    int id = nextId++;
    if (TRACE)
        printf("{\"event\":\"call\",\"id\":%d,\"parent\":%d,\"a\":\"%s\",\"b\":\"%s\"}\n",
               id, parent, toStr(a).c_str(), toStr(b).c_str());

    if (a.empty() || b.empty() || min(a.size(), b.size()) <= CUTOFF) {
        Big r = school(a, b);
        if (TRACE)
            printf("{\"event\":\"base\",\"id\":%d,\"a\":\"%s\",\"b\":\"%s\",\"product\":\"%s\"}\n",
                   id, toStr(a).c_str(), toStr(b).c_str(), toStr(r).c_str());
        return r;
    }

    size_t n = max(a.size(), b.size());
    size_t h = (n + 1) / 2;                              // split point in limbs
    Big a0(a.begin(), a.begin() + min(h, a.size())), a1;
    Big b0(b.begin(), b.begin() + min(h, b.size())), b1;
    if (a.size() > h) a1.assign(a.begin() + h, a.end());
    if (b.size() > h) b1.assign(b.begin() + h, b.end());
    trim(a0); trim(b0);

    if (TRACE)
        printf("{\"event\":\"split\",\"id\":%d,\"half\":%zu,\"a1\":\"%s\",\"a0\":\"%s\","
               "\"b1\":\"%s\",\"b0\":\"%s\"}\n",
               id, h, toStr(a1).c_str(), toStr(a0).c_str(), toStr(b1).c_str(), toStr(b0).c_str());

    Big z0 = kara(a0, b0, id);
    Big z2 = kara(a1, b1, id);
    Big sa = add(a0, a1), sb = add(b0, b1);
    Big p1 = kara(sa, sb, id);                           // (a0+a1)(b0+b1)
    Big z1 = sub(sub(p1, z0), z2);                       // middle term

    Big r(a.size() + b.size() + 2, 0);
    addAt(r, z0, 0);
    addAt(r, z1, h);
    addAt(r, z2, 2 * h);
    trim(r);

    if (TRACE)
        printf("{\"event\":\"combine\",\"id\":%d,\"sum_a\":\"%s\",\"sum_b\":\"%s\",\"p1\":\"%s\","
               "\"z0\":\"%s\",\"z1\":\"%s\",\"z2\":\"%s\",\"product\":\"%s\"}\n",
               id, toStr(sa).c_str(), toStr(sb).c_str(), toStr(p1).c_str(),
               toStr(z0).c_str(), toStr(z1).c_str(), toStr(z2).c_str(), toStr(r).c_str());
    return r;
}

// value mod p, by Horner's rule over the limbs
static u64 modOf(const Big& a, u64 p) {
    u64 r = 0, bm = BASE % p;
    for (int i = (int)a.size() - 1; i >= 0; i--) r = (r * bm + a[i] % p) % p;
    return r;
}

static void cleanDigits(string& s) {
    string t;
    t.reserve(s.size());
    for (char c : s) if (c >= '0' && c <= '9') t += c;
    s.swap(t);
}

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s file [--trace] [--check] [--out file]\n", argv[0]); return 1; }
    bool check = false;
    string outPath;
    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "--trace")) TRACE = true;
        else if (!strcmp(argv[i], "--check")) check = true;
        else if (!strcmp(argv[i], "--out") && i + 1 < argc) outPath = argv[++i];
    }

    ifstream in(argv[1], ios::binary);
    if (!in) { fprintf(stderr, "cannot open %s\n", argv[1]); return 1; }
    string sa, sb;
    getline(in, sa);
    getline(in, sb);
    cleanDigits(sa); cleanDigits(sb);
    if (sa.empty() || sb.empty()) { fprintf(stderr, "file must hold two numbers, one per line\n"); return 1; }

    if (TRACE) {                       // trace mode: one digit per limb, so the GUI sees real digits
        if (sa.size() > 1000 || sb.size() > 1000) { fprintf(stderr, "--trace only for up to 1000 digits\n"); return 1; }
        BASE = 10; BASE_DIGITS = 1; CUTOFF = 4;
    }

    Big A = fromStr(sa), B = fromStr(sb);

    auto t0 = chrono::steady_clock::now();
    Big P = kara(A, B, -1);
    auto t1 = chrono::steady_clock::now();
    double ms = chrono::duration<double, milli>(t1 - t0).count();

    if (TRACE) {
        printf("{\"event\":\"result\",\"a\":\"%s\",\"b\":\"%s\",\"product\":\"%s\",\"time_ms\":%.3f}\n",
               toStr(A).c_str(), toStr(B).c_str(), toStr(P).c_str(), ms);
        return 0;
    }

    string ps = toStr(P);
    if (outPath.empty()) {
        string f = argv[1];
        size_t k = f.find_last_of("/\\");
        outPath = "product_" + (k == string::npos ? f : f.substr(k + 1));
    }
    FILE* fo = fopen(outPath.c_str(), "wb");
    if (fo) { fwrite(ps.data(), 1, ps.size(), fo); fputc('\n', fo); fclose(fo); }

    printf("digits_a %zu\ndigits_b %zu\ndigits_product %zu\n", sa.size(), sb.size(), ps.size());
    if (ps.size() <= 60) printf("product %s\n", ps.c_str());
    else printf("product_head %s...\nproduct_tail ...%s\n", ps.substr(0, 25).c_str(), ps.substr(ps.size() - 25).c_str());
    printf("time_ms %.3f\nsaved_to %s\n", ms, fo ? outPath.c_str() : "(could not write file)");

    if (check) {
        bool ok = true;
        const u64 mods[2] = {1000000007ULL, 998244353ULL};
        for (u64 p : mods)                                    // (a*b) mod p must match, any size
            if ((modOf(A, p) * modOf(B, p)) % p != modOf(P, p)) ok = false;
        printf("mod_check %s\n", ok ? "OK" : "MISMATCH");
        if ((double)A.size() * (double)B.size() <= 4e8) {     // exact comparison when schoolbook is cheap
            bool same = (school(A, B) == P);
            printf("exact_check %s\n", same ? "OK" : "MISMATCH");
        } else printf("exact_check skipped (too large)\n");
    }
    return 0;
}