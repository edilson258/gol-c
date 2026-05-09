# Game of Life

A high-performance implementation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) in **C++20** with **SDL2** rendering. Compiles natively and to **WebAssembly** for the browser.

## Features

- Zero runtime allocations with fixed-size padded buffers and pointer-swap double buffering
- Branchless Conway's rules via bitwise operations
- Fully unrolled 8-neighbor sum with direct memory access
- Padded border eliminates all bounds checking in the hot path
- Batched `SDL_RenderFillRects` rendering (single draw call per frame)
- Click to create cells
- Auto-sprinkle of gliders, R-pentominoes, and blinkers
- WebAssembly support via Emscripten

## Build

### Prerequisites

- C++20 compiler (GCC / Clang)
- CMake >= 3.20
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
git clone https://github.com/emscripten-core/emsdk.git --depth 1
cd emsdk && ./emsdk install latest && ./emsdk activate latest && cd ..

./build-wasm.sh

cd web && python3 -m http.server 8080
```

Open `http://localhost:8080` in your browser.

## Controls

- **Left Click** — Set cell alive
- **Escape** — Quit (native only)
- **Window Resize** — Grid adapts automatically

## Performance

- **Data layout** — Flat `uint8_t` array, cache-line aligned (`alignas(64)`)
- **Buffering** — Double-buffered pointer swap, zero copies per generation
- **Bounds checking** — Eliminated via 1-cell padding border
- **Neighbor counting** — Fully unrolled 8-neighbor sum
- **GoL rules** — Branchless: `(alive == 3) | ((alive == 2) & cell)`
- **Rendering** — Batched `SDL_RenderFillRects`, one draw call per frame
- **Compiler** — `-O3 -march=native -flto -funroll-loops`

## License

MIT
