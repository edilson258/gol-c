#pragma once

#include <cstdint>

namespace gol
{

// Max grid dimensions (logical cells, excluding padding)
inline constexpr int MAX_COLS    = 256;
inline constexpr int MAX_ROWS    = 256;
inline constexpr int STRIDE      = MAX_COLS + 2;
inline constexpr int PADDED_SIZE = STRIDE * (MAX_ROWS + 2);

class GameOfLifeBoard
{
  public:
    GameOfLifeBoard(int cols, int rows);

    void initRandomly(double aliveChance = 0.01);
    void dump() const;

    void set(int col, int row);
    void unset(int col, int row);
    bool get(int col, int row) const;

    int cols() const;
    int rows() const;

    void resize(int newCols, int newRows);
    void computeNextGen();
    void sprinkle(int count = 15);

  private:
    int m_Cols;
    int m_Rows;

    // Fixed-size double-buffered padded grids — ZERO runtime allocation
    // 1-cell border of zeros eliminates all bounds checking in hot path
    alignas(64) uint8_t m_BufA[PADDED_SIZE];
    alignas(64) uint8_t m_BufB[PADDED_SIZE];
    uint8_t* m_Current;
    uint8_t* m_Next;

    int idx(int col, int row) const;
};

} // namespace gol
