\# ProjectAlgoAnalysis



Empirical and theoretical analysis of classic divide-and-conquer algorithms, built for \*\*CS2009 – Design and Analysis of Algorithms (Fall 2026)\*\* at FAST-NUCES Karachi.



The project implements each algorithm alongside a naive baseline, runs both on generated datasets of increasing size, and compares measured running time against the theoretical complexity.



\## Algorithms Covered



| Problem | Naive Approach | Divide \& Conquer Approach |

|---|---|---|

| Closest pair of points | Brute force, O(n²) | Divide and conquer, O(n log n) |

| Large integer multiplication | Schoolbook, O(n²) | Karatsuba, O(n^log₂3) ≈ O(n^1.585) |



\## Repository Structure



```

ProjectAlgoAnalysis/

├── Phase 1/     # TODO: one-line description (e.g. problem analysis, algorithm design, pseudocode)

├── Phase2/      # TODO: one-line description (e.g. implementation, experiments, GUI)

└── README.md

```



\## Project Phases



\### Phase 1

\- TODO: summarize what Phase 1 contains



\### Phase 2

\- Dataset generator for points and large integers of configurable size

\- Implementations of the naive and optimized algorithms

\- Benchmark runs across multiple input sizes

\- Result tables and plots comparing runtime against theory

\- GUI for visualizing the algorithms and results



\## Getting Started



\### Prerequisites

\- TODO: language and version (e.g. Python 3.10+ or a C++17 compiler)

\- TODO: libraries (e.g. `matplotlib`, `numpy`)



\### Installation

```bash

git clone https://github.com/Syed-56/ProjectAlgoAnalysis.git

cd ProjectAlgoAnalysis

\# TODO: pip install -r requirements.txt

```



\### Running

```bash

\# TODO: replace with your actual commands

\# Generate datasets

\# Run algorithms / benchmarks

\# Launch the GUI

```



\## Methodology



1\. \*\*Dataset generation\*\* – inputs are generated at increasing sizes (e.g. n = 100, 1,000, 10,000, …).

2\. \*\*Execution\*\* – each algorithm runs on identical inputs; results are checked against the naive version for correctness.

3\. \*\*Timing\*\* – runtime is recorded per input size, ideally averaged over several runs.

4\. \*\*Analysis\*\* – measured growth is compared with the expected asymptotic complexity.



\## Results



TODO: add a runtime table and/or plot, plus 2–3 lines on key observations (e.g. the input size where the optimized algorithm overtakes the naive one).



| Input Size | Naive (s) | Optimized (s) |

|---|---|---|

| TODO | TODO | TODO |



\## Team



\- Syed — algorithm implementation, dataset generation, benchmarking

\- TODO: teammate — GUI

\- TODO: teammate



\## License



TODO: add a license (e.g. MIT), or state that this is coursework.

