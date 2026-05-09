# 🧬 Game of Life

A high-performance implementation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) in **C++20** with **SDL2** rendering. Compiles natively and to **WebAssembly** for the browser.

## ✨ Features

- **Zero runtime allocations** — fixed-size padded buffers with pointer-swap double buffering
- **Branchless GoL rules** — bitwise Conway's logic, no branch misprediction
- **Unrolled neighbor sum** — 8 direct memory reads, no loop overhead
- **Padded border** — eliminates all bounds checking in the hot path
- **Batched rendering** — single `SDL_RenderFillRects` call per frame
- **Click to create cells** — left-click anywhere on the grid
- **Auto-sprinkle** — randomly injects gliders, R-pentominoes, and blinkers to keep the simulation alive
- **WebAssembly** — runs in the browser via Emscripten

## 🛠️ Build

### Prerequisites

- C++20 compiler (GCC / Clang)
- CMake ≥ 3.20
- SDL2 (`libsdl2-dev`)

### Native

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./gol
```

### WebAssembly

```bash
# First time: install Emscripten
git clone https://github.com/emscripten-core/emsdk.git --depth 1
cd emsdk && ./emsdk install latest && ./emsdk activate latest && cd ..

# Build
./build-wasm.sh

# Serve
cd web && python3 -m http.server 8080
```

Open `http://localhost:8080` in your browser.

## 🎮 Controls

| Input | Action |
|-------|--------|
| **Left Click** | Set cell alive |
| **Escape** | Quit (native only) |
| **Window Resize** | Grid adapts automatically |

## 📁 Project Structure

```
src/
├── GameOfLifeBoard.hpp   # Board declarations
├── GameOfLifeBoard.cpp   # Board logic (state, neighbors, generation)
├── GameOfLife.hpp         # Game/renderer declarations
├── GameOfLife.cpp         # SDL rendering, event loop, WASM support
└── main.cpp              # Entry point
web/
└── index.html            # Browser shell for WASM build
```

## ⚡ Performance Design

| Technique | Detail |
|-----------|--------|
| Data layout | Flat `uint8_t` array, cache-line aligned (`alignas(64)`) |
| Buffering | Double-buffered pointer swap — zero copies per generation |
| Bounds checking | Eliminated via 1-cell padding border of zeros |
| Neighbor counting | Fully unrolled 8-neighbor sum, direct memory access |
| GoL rules | Branchless: `(alive == 3) \| ((alive == 2) & cell)` |
| Rendering | Batched `SDL_RenderFillRects` — one draw call per frame |
| Compiler | `-O3 -march=native -flto -funroll-loops` |

## 📄 License

MIT
