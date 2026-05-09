#include "GameOfLife.hpp"

int main(void)
{
    int width  = 800;
    int height = 600;

    gol::GameOfLife game(width, height);

    return game.Run();
}
