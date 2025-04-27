#include "enemy.h"

static float Lerp(float a, float b, float t);

Enemy InitEnemy(int screenWidth, int screenHeight) {
    // Calculate random size multiplier between MIN and MAX
    float enemySize = GetRandomValue(ENEMY_MIN_SIZE, ENEMY_MAX_SIZE);
    
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
        .radius = enemySize,  // Use PLAYER_BASE_SIZE instead
        .health = enemySize*10.0f, 
        .maxHealth = enemySize*10.0f, 
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
    float ratio = (enemy.maxHealth > 0) ? (enemy.health / enemy.maxHealth) : 0.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    Color c;
    if (ratio > 0.5f) {
        // PURPLE to RED
        float t = (1.0f - ratio) * 2.0f; // 0~1
        c.r = (unsigned char)(Lerp(PURPLE.r, RED.r, t));
        c.g = (unsigned char)(Lerp(PURPLE.g, RED.g, t));
        c.b = (unsigned char)(Lerp(PURPLE.b, RED.b, t));
        c.a = (unsigned char)(255 * 1.0);
    } else {
        // RED to YELLOW
        float t = (0.5f - ratio) * 2.0f; // 0~1
        c.r = (unsigned char)(Lerp(RED.r, YELLOW.r, t));
        c.g = (unsigned char)(Lerp(RED.g, YELLOW.g, t));
        c.b = (unsigned char)(Lerp(RED.b, YELLOW.b, t));
        c.a = (unsigned char)(255 * 1.0);
    }
    DrawCircle(enemy.position.x, enemy.position.y, enemy.radius, c);
}

// Helper function for color interpolation
static float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
} 