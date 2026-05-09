#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
EMSDK_DIR="$SCRIPT_DIR/emsdk"

# Source emsdk environment
source "$EMSDK_DIR/emsdk_env.sh" 2>/dev/null

# Create wasm build directory
mkdir -p "$SCRIPT_DIR/build-wasm"

# Build with emcc directly (no cmake needed for this small project)
echo "[BUILD] Compiling to WebAssembly..."

emcc \
    "$SCRIPT_DIR/src/main.cpp" \
    "$SCRIPT_DIR/src/GameOfLife.cpp" \
    "$SCRIPT_DIR/src/GameOfLifeBoard.cpp" \
    -I"$SCRIPT_DIR/src" \
    -std=c++20 \
    -O3 \
    -s USE_SDL=2 \
    -s WASM=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s TOTAL_STACK=1048576 \
    -o "$SCRIPT_DIR/web/gol.js"

echo "[DONE] Output: web/gol.js + web/gol.wasm"
echo "[RUN]  cd web && python3 -m http.server 8080"
