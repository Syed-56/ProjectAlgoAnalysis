# ProjectAlgoAnalysis
 
**CS2009 – Design and Analysis of Algorithms (Fall 2026), FAST-NUCES Karachi**
 
Two classic divide-and-conquer algorithms, implemented from scratch in C++ and benchmarked on inputs of up to one million points or digits:
 
- **Closest pair of points** in the plane, O(n log n)
- **Karatsuba integer multiplication** with a schoolbook cut-off, Θ(n^1.585)
The project includes seeded, reproducible test data, independent correctness checks, a browser-based step-by-step visualizer, and a written report.
 
## Highlights
 
| Result | Value |
|---|---|
| Closest pair, 1,000,000 points | about 0.27 s |
| Karatsuba, two 1,000,000-digit numbers | about 1.4 s |
| Measured growth, closest pair | exponent 1.09 (theory: n log n, about 1.10) |
| Measured growth, Karatsuba | exponent 1.52 (theory: 1.585; the cut-off flattens small sizes) |
| Test inputs | 20 seeded files, 101 up to 10⁶ points or digits |
 
No library sorting or big-integer routines are used. Closest pair has its own merge sort, and Karatsuba works on base-10⁹ limbs.
 
## Repository Structure
 
```
ProjectAlgoAnalysis/
├── Phase 1/
│   ├── DAA written part(project).pdf   # written solutions
│   └── psuedocode/                     # pseudocode: min-max, power, inversions, 1-D closest pair
├── Phase 2/
│   ├── project/
│   │   ├── closestpp.cpp               # closest pair (divide and conquer)
│   │   ├── karatsuba.cpp               # Karatsuba multiplication
│   │   ├── generator.py                # builds all 20 datasets from fixed seeds
│   │   ├── run_all.py                  # runs everything, verifies, writes results.csv
│   │   ├── results.csv                 # benchmark output
│   │   ├── seeds.txt                   # seed and size per dataset
│   │   └── data/                       # cp01–cp10, im01–im10
│   └── ui-interface/
│       └── ui_interface.html           # visualizer (open in a browser)
└── Phase 3/
    ├── report.tex / report.pdf         # final report
    ├── refs.bib
    └── figures/
```
 
## Algorithms
 
| Problem | Brute force | Divide and conquer | Recurrence |
|---|---|---|---|
| Closest pair of points | Θ(n²) | O(n log n) | 2T(n/2) + O(n) |
| Integer multiplication | Θ(n²) | Θ(n^log₂3) ≈ Θ(n^1.585) | 3T(n/2) + O(n) |
 
Brute force is used as a correctness reference (`--check`). Karatsuba falls back to schoolbook multiplication at or below a cut-off of 32 limbs.
 
## Getting Started
 
### Requirements
- A C++17 compiler (`g++`)
- Python 3 (standard library only)
- A modern web browser for the visualizer
### Build
```bash
cd "Phase 2/project"
g++ -O2 -std=c++17 -o closestpp closestpp.cpp
g++ -O2 -std=c++17 -o karatsuba karatsuba.cpp
```
 
### Generate the datasets
```bash
python generator.py        # recreates data/ from the seeds in seeds.txt
```
 
### Run a single file
```bash
./closestpp data/cp01.txt                # result and time
./closestpp data/cp02.txt --check        # compare with brute force (n <= 20,000)
./karatsuba data/im01.txt                # product written to product_im01.txt
./karatsuba data/im02.txt --check        # verify the product
./karatsuba data/im01.txt --out my.txt   # choose the output file
```
 
### Run the full benchmark
```bash
python run_all.py          # runs all 20 files, verifies, prints a table
```
 
### Visualizer
Open `Phase 2/ui-interface/ui_interface.html` in a browser and drop in a dataset file (`cp01`–`cp10` or `im01`–`im10`). Small inputs such as `cp10` and `im10` are best for stepping through the divide, conquer and combine stages. The visualizer is meant for demonstration, not for timing million-size inputs.
 
The `--trace` flag on both programs prints JSON lines of the recursion (limited to n ≤ 1000 points or 1000 digits).
 
## Datasets
 
| Files | Size | Content |
|---|---|---|
| cp01–cp04 | 10³ to 10⁶ points | Uniform random coordinates |
| cp05 | 10⁶ | Clustered around 10 centres |
| cp06 | 10⁶ | Many duplicate points (distance 0) |
| cp07 | 10⁵ | All points on one vertical line |
| cp08 | 10⁵ | Shuffled 250 × 400 grid (many tied pairs) |
| cp09 | 10⁶ | Closest pair straddles the dividing line |
| cp10 | 101 | Small demo set |
| im01–im04 | 10³ to 10⁶ digits | Two random numbers of equal length |
| im05 | 10⁶ × 10 digits | Very unequal lengths |
| im06 | 10⁵ | All-nines times a random number |
| im07 | 99,999 | Odd length, mostly zeros |
| im08 | 10⁶ | 10^999999 × 10^999999 |
| im09 | 10⁶ | One factor is zero |
| im10 | 101 | Small demo set |
 
Seeds are `1000 + k` for `cp`k and `2000 + k` for `im`k, so every file can be regenerated exactly.
 
## Verification
 
- **Closest pair:** the reported pair must match the reported distance, an independent grid method must find nothing closer (all files), and brute force is compared for n ≤ 20,000.
- **Multiplication:** modular checks against two primes (all files), plus an exact comparison with Python's `int` up to 200,000 digits.
Every file passed all checks that were run.
 
## Selected Results
 
| File | Input | Time (ms) |
|---|---|---|
| cp01 | 1,000 points | 0.145 |
| cp03 | 100,000 points | 20.924 |
| cp04 | 1,000,000 points | 267.319 |
| im01 | 1,000 digits | 0.038 |
| im03 | 100,000 digits | 38.839 |
| im04 | 1,000,000 digits | 1361.753 |
 
Full numbers are in `Phase 2/project/results.csv`. Timings are single runs on one laptop, measured around the algorithm only (file I/O excluded), compiled with `g++ -O2 -std=c++17`. See `Phase 3/report.pdf` for the analysis.
 
## Team
 
- Sultan Ahmed
- Rayyan Ahmer
- Hamza Atif
 