#ifndef STAR_ITEM_H
#define STAR_ITEM_H

#include "raylib.h"
#include <stdbool.h>

#define STAR_BASE_SIZE 12.0f
#define STAR_SPEED 150.0f
#define STAR_INVINCIBILITY_DURATION 20.0f
#define STAR_SPAWN_MIN_INTERVAL 30.0f
#define STAR_SPAWN_MAX_INTERVAL 40.0f
#define STAR_TARGET_AREA_RADIUS 30.0f

typedef struct {
    Vector2 position;
    Vector2 velocity;  // For trajectory movement
    float radius;
    bool isActive;
    float colorTimer;  // For rainbow effect
} StarItem;

// Core functions
StarItem InitStarItem(void);
void SpawnStarItem(StarItem* star, int screenWidth, int screenHeight);
void UpdateStarItem(StarItem* star, float deltaTime, int screenWidth, int screenHeight);
void DrawStarItem(StarItem star);
bool CheckStarPlayerCollision(StarItem* star, Vector2 playerPos, float playerRadius);

// Helper functions
Vector2 CalculateStarSpawnPosition(int screenWidth, int screenHeight);
Vector2 CalculateStarVelocity(Vector2 spawnPos, int screenWidth, int screenHeight);
Color GetRainbowColor(float timer);

#endif