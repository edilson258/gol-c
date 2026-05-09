#include <cstdio>

#include "GameOfLife.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace gol
{

GameOfLife::GameOfLife(int width, int height)
    : m_SDLWindowWidth(width), m_SDLWindowHeight(height),
      m_Board(width / DEFAULT_CELL_SIZE, height / DEFAULT_CELL_SIZE)
{
}

void GameOfLife::Tick()
{
    while (SDL_PollEvent(&m_SDLEvent))
    {
        HandleSDLEvent();
    }

    FillBackgroundColor();

    DrawGrid();
    DrawCells();
    m_Board.computeNextGen();

    if (++m_FrameCount % 15 == 0)
    {
        m_Board.sprinkle(3);
    }

    SDL_RenderPresent(m_SDLRenderer);
}

#ifdef __EMSCRIPTEN__
static GameOfLife* g_Game = nullptr;

static void emMainLoop()
{
    g_Game->Tick();
}
#endif

int GameOfLife::Run()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(
            stderr, "[ERROR]: Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    m_Board.initRandomly(0.2);

    if (CreateSDLWindow())
    {
        return -1;
    }

    if (CreateSDLRenderer())
    {
        return -1;
    }

    m_ShouldRun = true;

#ifdef __EMSCRIPTEN__
    g_Game = this;
    emscripten_set_main_loop(emMainLoop, 10, 1);
#else
    m_Board.dump();

    while (m_ShouldRun)
    {
        Tick();
        SDL_Delay(100);
    }

    SDL_DestroyRenderer(m_SDLRenderer);
    SDL_DestroyWindow(m_SDLWindow);
    SDL_Quit();
#endif

    return 0;
}

int GameOfLife::CreateSDLWindow()
{
    m_SDLWindow = SDL_CreateWindow(DEFAULT_WINDOW_TITLE,
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   m_SDLWindowWidth,
                                   m_SDLWindowHeight,
                                   SDL_WINDOW_RESIZABLE);

    if (!m_SDLWindow)
    {
        fprintf(stderr,
                "[ERROR]: Failed to create SDL window: %s\n",
                SDL_GetError());
        return -1;
    }

    return 0;
}

int GameOfLife::CreateSDLRenderer()
{
    m_SDLRenderer =
        SDL_CreateRenderer(m_SDLWindow, -1, SDL_RENDERER_ACCELERATED);

    if (!m_SDLRenderer)
    {
        fprintf(stderr,
                "[ERROR]: Failed to create SDL renderer: %s\n",
                SDL_GetError());
        return -1;
    }

    return 0;
}

int GameOfLife::FillBackgroundColor()
{
    SDL_SetRenderDrawColor(m_SDLRenderer, 21, 45, 50, 255);
    SDL_RenderClear(m_SDLRenderer);
    return 0;
}

int GameOfLife::DrawGrid()
{
    SDL_SetRenderDrawColor(m_SDLRenderer, 0, 0, 0, 255);

    for (int x = 0; x <= m_SDLWindowWidth; x += DEFAULT_CELL_SIZE)
    {
        SDL_RenderDrawLine(m_SDLRenderer, x, 0, x, m_SDLWindowHeight);
    }

    for (int y = 0; y <= m_SDLWindowHeight; y += DEFAULT_CELL_SIZE)
    {
        SDL_RenderDrawLine(m_SDLRenderer, 0, y, m_SDLWindowWidth, y);
    }

    return 0;
}

int GameOfLife::DrawCells()
{
    static SDL_Rect rects[MAX_COLS * MAX_ROWS];
    int             count = 0;

    SDL_SetRenderDrawColor(m_SDLRenderer, 255, 255, 255, 255);

    for (int col = 0; col < m_Board.cols(); col++)
    {
        for (int row = 0; row < m_Board.rows(); row++)
        {
            if (m_Board.get(col, row))
            {
                rects[count++] = { col * DEFAULT_CELL_SIZE,
                                   row * DEFAULT_CELL_SIZE,
                                   DEFAULT_CELL_SIZE,
                                   DEFAULT_CELL_SIZE };
            }
        }
    }

    if (count > 0)
    {
        SDL_RenderFillRects(m_SDLRenderer, rects, count);
    }

    return 0;
}

int GameOfLife::HandleSDLEvent()
{
    if (m_SDLEvent.type == SDL_QUIT ||
        (m_SDLEvent.type == SDL_KEYDOWN &&
         m_SDLEvent.key.keysym.sym == SDLK_ESCAPE))
    {
        m_ShouldRun = false;
        return 0;
    }

    if (m_SDLEvent.type == SDL_WINDOWEVENT &&
        m_SDLEvent.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        SDL_GetWindowSize(m_SDLWindow, &m_SDLWindowWidth, &m_SDLWindowHeight);

        int newCols = m_SDLWindowWidth / DEFAULT_CELL_SIZE;
        int newRows = m_SDLWindowHeight / DEFAULT_CELL_SIZE;

        m_Board.resize(newCols, newRows);

        return 0;
    }

    if (m_SDLEvent.type == SDL_MOUSEBUTTONDOWN &&
        m_SDLEvent.button.button == SDL_BUTTON_LEFT)
    {
        int col = m_SDLEvent.button.x / DEFAULT_CELL_SIZE;
        int row = m_SDLEvent.button.y / DEFAULT_CELL_SIZE;

        if (col >= 0 && col < m_Board.cols() && row >= 0 &&
            row < m_Board.rows())
        {
            m_Board.set(col, row);
        }

        return 0;
    }

    return -1;
}

} // namespace gol
