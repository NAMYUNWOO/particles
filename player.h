#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

// Constants
#define PLAYER_BASE_SIZE 10.0f  // Base player size in pixels

// Player structure
typedef struct {
    Vector2 position;
    float size;        // Current player size
} Player;

// Initialize player
Player InitPlayer(int screenWidth, int screenHeight);

// Update player position
void UpdatePlayer(Player* player, int screenWidth, int screenHeight, int moveSpeed);

// Draw player
void DrawPlayer(Player player);

#endif // PLAYER_H