# XuLang

A toy language based on flex, bison and llvm.

# Build

1. Requirements

```bash
sudo apt install flex bison llvm-14-dev clang++-14 cmake
```

2. Run cmake

```bash
mkdir build && cd build
cmake ../src -DCMAKE_CXX_COMPILER=clang++
make
```

# Usage

```bash
./build/ast2json ./examples/primes.xl
```
