#include "enemy.h"
#include <stdio.h>

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

void UpdateEnemy(Enemy* enemy, int screenWidth, int screenHeight, float deltaTime) {
    // Update position based on velocity
    enemy->position.x += enemy->velocity.x;
    enemy->position.y += enemy->velocity.y;
    
    // Increase radius over time
    enemy->radius += deltaTime * 15.0f; // Grows by 10 pixels per second

    // Increase maxHealth and health as radius grows
    float prevMaxHealth = enemy->maxHealth;
    enemy->maxHealth = enemy->radius * 10.0f;
    // If health was full, keep it full; otherwise, keep the same ratio
    if (enemy->health == prevMaxHealth) {
        enemy->health = enemy->maxHealth;
    } else {
        float ratio = prevMaxHealth > 0 ? (enemy->health / prevMaxHealth) : 1.0f;
        enemy->health = enemy->maxHealth * ratio;
    }

    // Screen boundary check with margin (allow center to go outside by radius*0.7)
    float margin = enemy->radius * 0.7f;
    if (enemy->position.x < -margin) {
        enemy->position.x = -margin;
        enemy->velocity.x *= -1;
    }
    if (enemy->position.x > screenWidth + margin) {
        enemy->position.x = screenWidth + margin;
        enemy->velocity.x *= -1;
    }
    if (enemy->position.y < -margin) {
        enemy->position.y = -margin;
        enemy->velocity.y *= -1;
    }
    if (enemy->position.y > screenHeight + margin) {
        enemy->position.y = screenHeight + margin;
        enemy->velocity.y *= -1;
    }
}

void DrawEnemy(Enemy enemy) {
    float timeSinceSpawn = GetTime() - enemy.spawnTime;
    // Blink for first 0.5 seconds
    if (timeSinceSpawn < 0.5f && ((int)(GetTime() * 10) % 2 == 0)) {
        return; // Skip drawing (blink)
    }
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
    // 채도 보정: health가 낮을수록 색이 흰색에 가까워짐
    float sat = ratio; // 1=원색, 0=흰색
    c.r = (unsigned char)Lerp(255, c.r, sat);
    c.g = (unsigned char)Lerp(255, c.g, sat);
    c.b = (unsigned char)Lerp(255, c.b, sat);
    DrawCircle(enemy.position.x, enemy.position.y, enemy.radius, c);
    // Draw health text above the enemy
    char healthText[32];
    sprintf(healthText, "%d/%d", (int)enemy.health, (int)enemy.maxHealth);
    int textWidth = MeasureText(healthText, 16);
    DrawText(healthText, enemy.position.x - textWidth/2, enemy.position.y - enemy.radius - 20, 16, BLACK);
}

// Helper function for color interpolation
static float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
} 