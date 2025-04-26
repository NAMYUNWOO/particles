#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct {
    Vector2 position;
    int size;
} Player;

// Initialize a new player
Player InitPlayer(int screenWidth, int screenHeight);

// Update player position based on input
void UpdatePlayer(Player* player, int screenWidth, int screenHeight, int moveSpeed);

// Draw the player
void DrawPlayer(Player player);

#endif // PLAYER_H 