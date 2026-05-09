#include <algorithm>
#include <cstdio>
#include <cstring>
#include <random>

#include "GameOfLifeBoard.hpp"

namespace gol
{

GameOfLifeBoard::GameOfLifeBoard(int cols, int rows)
    : m_Cols(std::min(cols, MAX_COLS)), m_Rows(std::min(rows, MAX_ROWS)),
      m_Current(m_BufA), m_Next(m_BufB)
{
    memset(m_BufA, 0, PADDED_SIZE);
    memset(m_BufB, 0, PADDED_SIZE);
}

int GameOfLifeBoard::idx(int col, int row) const
{
    return (row + 1) * STRIDE + (col + 1);
}

void GameOfLifeBoard::initRandomly(double aliveChance)
{
    static std::random_device   rd;
    static std::mt19937         gen(rd());
    std::bernoulli_distribution dist(aliveChance);

    for (int r = 0; r < m_Rows; r++)
    {
        const int rowOff = (r + 1) * STRIDE + 1;
        for (int c = 0; c < m_Cols; c++)
        {
            m_Current[rowOff + c] = dist(gen) ? 1 : 0;
        }
    }
}

void GameOfLifeBoard::dump() const
{
    for (int r = 0; r < m_Rows; r++)
    {
        const int rowOff = (r + 1) * STRIDE + 1;
        for (int c = 0; c < m_Cols; c++)
        {
            putchar(m_Current[rowOff + c] ? 'O' : '.');
        }
        putchar('\n');
    }
}

void GameOfLifeBoard::set(int col, int row)
{
    m_Current[idx(col, row)] = 1;
}

void GameOfLifeBoard::unset(int col, int row)
{
    m_Current[idx(col, row)] = 0;
}

bool GameOfLifeBoard::get(int col, int row) const
{
    return m_Current[idx(col, row)] != 0;
}

int GameOfLifeBoard::cols() const
{
    return m_Cols;
}

int GameOfLifeBoard::rows() const
{
    return m_Rows;
}

void GameOfLifeBoard::resize(int newCols, int newRows)
{
    newCols = std::min(newCols, MAX_COLS);
    newRows = std::min(newRows, MAX_ROWS);

    // Clear next buffer, copy intersection via memcpy per row, swap
    memset(m_Next, 0, PADDED_SIZE);

    int cpRows = std::min(m_Rows, newRows);
    int cpCols = std::min(m_Cols, newCols);

    for (int r = 0; r < cpRows; r++)
    {
        int rowOff = (r + 1) * STRIDE + 1;
        memcpy(&m_Next[rowOff], &m_Current[rowOff], cpCols);
    }

    uint8_t* tmp = m_Current;
    m_Current    = m_Next;
    m_Next       = tmp;

    memset(m_Next, 0, PADDED_SIZE);

    m_Cols = newCols;
    m_Rows = newRows;
}

void GameOfLifeBoard::computeNextGen()
{
    // Padding border is always 0 → NO bounds checking needed
    // Neighbor sum is fully unrolled → no loop overhead
    // Branchless GoL rule via bitwise ops → no branch misprediction

    for (int r = 0; r < m_Rows; r++)
    {
        const int rowOff = (r + 1) * STRIDE;

        for (int c = 0; c < m_Cols; c++)
        {
            const int i = rowOff + (c + 1);

            // Unrolled 8-neighbor sum — direct memory access, no function call
            const int alive = m_Current[i - STRIDE - 1] +
                              m_Current[i - STRIDE] +
                              m_Current[i - STRIDE + 1] + m_Current[i - 1] +
                              m_Current[i + 1] + m_Current[i + STRIDE - 1] +
                              m_Current[i + STRIDE] + m_Current[i + STRIDE + 1];

            // Branchless Conway's rules:
            //   alive==3 → birth or survive
            //   alive==2 && cell → survive
            //   else → death
            m_Next[i] = static_cast<uint8_t>((alive == 3) |
                                             ((alive == 2) & m_Current[i]));
        }
    }

    // Pointer swap — zero-cost buffer exchange, no copies
    uint8_t* tmp = m_Current;
    m_Current    = m_Next;
    m_Next       = tmp;
}

void GameOfLifeBoard::sprinkle(int count)
{
    static std::random_device rd;
    static std::mt19937       gen(rd());

    // Small viable patterns that survive and propagate
    // Each pattern is a list of {dc, dr} offsets from a center cell
    static constexpr int PATTERN_COUNT = 3;

    // R-pentomino (chaotic, long-lived)
    static constexpr int rpento[][2] = {
        { 0, -1 }, { 1, -1 }, { -1, 0 }, { 0, 0 }, { 0, 1 }
    };
    // Glider (moves across the board)
    static constexpr int glider[][2] = {
        { 1, -1 }, { 2, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 }
    };
    // Blinker (simple oscillator)
    static constexpr int blinker[][2] = { { -1, 0 }, { 0, 0 }, { 1, 0 } };

    static constexpr int patternSizes[PATTERN_COUNT] = { 5, 5, 3 };

    for (int i = 0; i < count; i++)
    {
        int cx = 3 + static_cast<int>(gen() % (m_Cols - 6));
        int cy = 3 + static_cast<int>(gen() % (m_Rows - 6));

        int which = gen() % PATTERN_COUNT;

        const int (*pattern)[2];
        int size;

        switch (which)
        {
        case 0:
            pattern = rpento;
            size    = patternSizes[0];
            break;
        case 1:
            pattern = glider;
            size    = patternSizes[1];
            break;
        default:
            pattern = blinker;
            size    = patternSizes[2];
            break;
        }

        for (int j = 0; j < size; j++)
        {
            int c                = cx + pattern[j][0];
            int r                = cy + pattern[j][1];
            m_Current[idx(c, r)] = 1;
        }
    }
}

} // namespace gol
