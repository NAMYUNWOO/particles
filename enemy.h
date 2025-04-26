#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "player.h"

// Enemy entity structure (similar to Particle)
typedef struct {
    Vector2 position;    // Current position
    Vector2 velocity;    // Current velocity
    Color color;        // Enemy color
    float spawnTime;    // Time when enemy was spawned
    float radius;       // Enemy radius (3-10x player size)
    float health;
    float maxHealth;
    float damage;
} Enemy;

// Constants
#define MAX_ENEMIES 10          // Maximum number of enemies
#define ENEMY_SPAWN_TIME 3.0f    // Spawn interval in seconds
#define ENEMY_MIN_SIZE 10.0f  // Minimum size multiplier compared to player
#define ENEMY_MAX_SIZE 30.0f // Maximum size multiplier compared to player

// Enemy initialization
Enemy InitEnemy(int screenWidth, int screenHeight);

// Enemy update and render functions
void UpdateEnemy(Enemy* enemy, int screenWidth, int screenHeight);
void DrawEnemy(Enemy enemy);

#endif // ENEMY_H 