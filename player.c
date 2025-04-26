#include "player.h"

Player InitPlayer(int screenWidth, int screenHeight) {
    Player player = {
        .position = (Vector2){ screenWidth/2, screenHeight/2 },
        .size = 3
    };
    return player;
}

void UpdatePlayer(Player* player, int screenWidth, int screenHeight, int moveSpeed) {
    // Move the square with arrow keys
    if (IsKeyDown(KEY_RIGHT)) player->position.x += moveSpeed;
    if (IsKeyDown(KEY_LEFT)) player->position.x -= moveSpeed;
    if (IsKeyDown(KEY_DOWN)) player->position.y += moveSpeed;
    if (IsKeyDown(KEY_UP)) player->position.y -= moveSpeed;

    // Keep player within screen bounds
    if (player->position.x < 0) player->position.x = 0;
    if (player->position.y < 0) player->position.y = 0;
    if (player->position.x > screenWidth - player->size) player->position.x = screenWidth - player->size;
    if (player->position.y > screenHeight - player->size) player->position.y = screenHeight - player->size;
}

void DrawPlayer(Player player) {
    DrawRectangle(
        player.position.x,
        player.position.y,
        player.size,
        player.size,
        RED
    );
} 