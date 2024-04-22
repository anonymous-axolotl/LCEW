# Longest common extension in strings with wildcards (LCEW)

Library for computing longest common extension in strings with wildcards.

## Building the project

To download and build the project:
```bash
git clone https://github.com/anonymous-axolotl/LCEW.git # Clone the repository
cd LCEW
make # build `main` executable
```

## Source files

There are the
- `lcew.{c,h}pp`: the whole point. Data structure for longest common extension queries with wildcards.
- `fast_mm.{c,h}pp`: sparse boolean matrix multiplication using the LCEW data structure.
- `pm_wc.{c,h}pp`: algorithm for pattern matching in strings with wildcards.
- `ntt.{c,h}pp`: implementation of the Number Theoretic Transform (Fourier transform over finite fields).
- `ukkonen.{c,h}pp`: Ukkonen's algorithm to build suffix trees, used to compute suffix and LCP arrays.
- `lce.hpp`: data structure for (usual) longest common extension queries.
- `main.cpp`: entry point and test functions.
