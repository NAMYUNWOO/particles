#include "raylib.h"
#include "game.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Particle Simulation");
    SetTargetFPS(60);

    Game game = InitGame(screenWidth, screenHeight);

    while (!WindowShouldClose())
    {
        UpdateGame(&game);
        DrawGame(game);
    }

    CleanupGame(&game);
    
    CloseWindow();
    return 0;
} 