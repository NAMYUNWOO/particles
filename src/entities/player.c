#include "player.h"

Player InitPlayer(int screenWidth, int screenHeight) {
    Player player = {
        .position = (Vector2){ screenWidth/2, screenHeight/2 },
        .size = PLAYER_BASE_SIZE,
        .health = 3,
        .invincibleTimer = 0.0f,
        .isInvincible = false
    };
    return player;
}

void UpdatePlayer(Player* player, int screenWidth, int screenHeight, int moveSpeed, float deltaTime) {
    // Boost logic
    float speed = moveSpeed;
    bool shiftHeld = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    if (player->isBoosting) {
        // Continue boosting as long as shift is held and gauge > 0
        if (shiftHeld && player->boostGauge > 0.0f) {
            speed = moveSpeed * 2.5f;
            player->boostGauge -= BOOST_GAUGE_CONSUME * deltaTime;
            if (player->boostGauge < 0.0f) player->boostGauge = 0.0f;
        } else {
            player->isBoosting = false;
        }
    } else if (shiftHeld && player->boostGauge > 50.0f) {
        // Start boosting only if gauge > 50
        player->isBoosting = true;
        speed = moveSpeed * 2.5f;
        player->boostGauge -= BOOST_GAUGE_CONSUME * deltaTime;
        if (player->boostGauge < 0.0f) player->boostGauge = 0.0f;
    }
    if (!player->isBoosting) {
        player->boostGauge += BOOST_GAUGE_REGEN * deltaTime;
        if (player->boostGauge > BOOST_GAUGE_MAX) player->boostGauge = BOOST_GAUGE_MAX;
    }
    // Move the square with arrow keys
    if (IsKeyDown(KEY_RIGHT)) player->position.x += speed;
    if (IsKeyDown(KEY_LEFT)) player->position.x -= speed;
    if (IsKeyDown(KEY_DOWN)) player->position.y += speed;
    if (IsKeyDown(KEY_UP)) player->position.y -= speed;

    // Keep player within screen bounds
    if (player->position.x < 0) player->position.x = 0;
    if (player->position.y < 0) player->position.y = 0;
    if (player->position.x > screenWidth - player->size) player->position.x = screenWidth - player->size;
    if (player->position.y > screenHeight - player->size) player->position.y = screenHeight - player->size;

    // Invincibility timer update
    if (player->isInvincible) {
        player->invincibleTimer -= deltaTime;
        if (player->invincibleTimer <= 0.0f) {
            player->isInvincible = false;
            player->invincibleTimer = 0.0f;
        }
    }
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

void DamagePlayer(Player* player) {
    if (!player->isInvincible) {
        player->health--;
        player->isInvincible = true;
        player->invincibleTimer = 1.5f;
    }
} 