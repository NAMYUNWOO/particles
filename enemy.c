#include "enemy.h"

Enemy InitEnemy(int screenWidth, int screenHeight) {
    // Calculate random size multiplier between MIN and MAX
    float sizeMultiplier = GetRandomValue(ENEMY_MIN_SIZE_MULTIPLIER, ENEMY_MAX_SIZE_MULTIPLIER);
    
    Enemy enemy = {
        .position = {
            GetRandomValue(0, screenWidth),
            GetRandomValue(0, screenHeight)
        },
        .velocity = {
            GetRandomValue(-100, 100) / 100.0f,
            GetRandomValue(-100, 100) / 100.0f
        },
        .color = PURPLE,
        .spawnTime = GetTime(),
        .radius = PLAYER_BASE_SIZE * sizeMultiplier  // Use PLAYER_BASE_SIZE instead
    };
    
    return enemy;
}

void UpdateEnemy(Enemy* enemy, int screenWidth, int screenHeight) {
    // Update position based on velocity
    enemy->position.x += enemy->velocity.x;
    enemy->position.y += enemy->velocity.y;
    
    // Screen boundary check with radius consideration
    if (enemy->position.x - enemy->radius < 0) {
        enemy->position.x = enemy->radius;
        enemy->velocity.x *= -1;
    }
    if (enemy->position.x + enemy->radius > screenWidth) {
        enemy->position.x = screenWidth - enemy->radius;
        enemy->velocity.x *= -1;
    }
    if (enemy->position.y - enemy->radius < 0) {
        enemy->position.y = enemy->radius;
        enemy->velocity.y *= -1;
    }
    if (enemy->position.y + enemy->radius > screenHeight) {
        enemy->position.y = screenHeight - enemy->radius;
        enemy->velocity.y *= -1;
    }
}

void DrawEnemy(Enemy enemy) {
    DrawCircle(enemy.position.x, enemy.position.y, enemy.radius, enemy.color);
} 