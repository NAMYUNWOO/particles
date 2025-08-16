#include "star_item.h"
#include "../../core/memory_pool.h"
#include "../../core/event/event_system.h"
#include "../../core/event/event_types.h"
#include <math.h>
#include <stdlib.h>

extern MemoryPool g_itemEventPool;

StarItem InitStarItem(void) {
    StarItem star = {
        .position = (Vector2){0, 0},
        .velocity = (Vector2){0, 0},
        .radius = STAR_BASE_SIZE,
        .isActive = false,
        .colorTimer = 0.0f
    };
    return star;
}

Vector2 CalculateStarSpawnPosition(int screenWidth, int screenHeight) {
    // Spawn from random edge
    int edge = rand() % 4; // 0=top, 1=right, 2=bottom, 3=left
    Vector2 pos;
    float margin = STAR_BASE_SIZE * 2;
    
    switch(edge) {
        case 0: // Top
            pos.x = margin + (rand() % (int)(screenWidth - 2 * margin));
            pos.y = -margin;
            break;
        case 1: // Right
            pos.x = screenWidth + margin;
            pos.y = margin + (rand() % (int)(screenHeight - 2 * margin));
            break;
        case 2: // Bottom
            pos.x = margin + (rand() % (int)(screenWidth - 2 * margin));
            pos.y = screenHeight + margin;
            break;
        case 3: // Left
            pos.x = -margin;
            pos.y = margin + (rand() % (int)(screenHeight - 2 * margin));
            break;
    }
    return pos;
}

Vector2 CalculateStarVelocity(Vector2 spawnPos, int screenWidth, int screenHeight) {
    // Target random point in center area
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;
    float targetX = centerX + (rand() % 61 - 30); // -30 to +30
    float targetY = centerY + (rand() % 61 - 30); // -30 to +30
    
    // Calculate direction vector
    Vector2 direction = {
        targetX - spawnPos.x,
        targetY - spawnPos.y
    };
    
    // Normalize and apply speed
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x = (direction.x / length) * STAR_SPEED;
        direction.y = (direction.y / length) * STAR_SPEED;
    }
    
    return direction;
}

void SpawnStarItem(StarItem* star, int screenWidth, int screenHeight) {
    if (star->isActive) return;
    
    star->position = CalculateStarSpawnPosition(screenWidth, screenHeight);
    star->velocity = CalculateStarVelocity(star->position, screenWidth, screenHeight);
    star->isActive = true;
    star->colorTimer = 0.0f;
    
    // Publish spawn event
    if (g_itemEventPool.blocks != NULL) {
        ItemEventData* data = MemoryPool_Alloc(&g_itemEventPool);
        if (data) {
            data->itemType = 1; // Star Item
            data->position = star->position;
            data->itemPtr = star;
            PublishEvent(EVENT_ITEM_SPAWNED, data);
        }
    }
}

void UpdateStarItem(StarItem* star, float deltaTime, int screenWidth, int screenHeight) {
    if (!star->isActive) return;
    
    // Update position
    star->position.x += star->velocity.x * deltaTime;
    star->position.y += star->velocity.y * deltaTime;
    
    // Update color timer for rainbow effect
    star->colorTimer += deltaTime * 2.0f; // Speed of color change
    
    // Check if out of bounds
    float margin = STAR_BASE_SIZE * 3;
    if (star->position.x < -margin || star->position.x > screenWidth + margin ||
        star->position.y < -margin || star->position.y > screenHeight + margin) {
        star->isActive = false;
        
        // Publish expire event
        if (g_itemEventPool.blocks != NULL) {
            ItemEventData* data = MemoryPool_Alloc(&g_itemEventPool);
            if (data) {
                data->itemType = 1;
                data->position = star->position;
                data->itemPtr = star;
                PublishEvent(EVENT_ITEM_EXPIRED, data);
            }
        }
    }
}

Color GetRainbowColor(float timer) {
    // Create smooth rainbow transition
    float hue = fmodf(timer, 1.0f) * 360.0f;
    
    // Convert HSV to RGB (simplified)
    float h = hue / 60.0f;
    float x = 1.0f - fabsf(fmodf(h, 2.0f) - 1.0f);
    
    Color color;
    if (h < 1) { color = (Color){255, (unsigned char)(255 * x), 0, 255}; }
    else if (h < 2) { color = (Color){(unsigned char)(255 * x), 255, 0, 255}; }
    else if (h < 3) { color = (Color){0, 255, (unsigned char)(255 * x), 255}; }
    else if (h < 4) { color = (Color){0, (unsigned char)(255 * x), 255, 255}; }
    else if (h < 5) { color = (Color){(unsigned char)(255 * x), 0, 255, 255}; }
    else { color = (Color){255, 0, (unsigned char)(255 * x), 255}; }
    
    return color;
}

void DrawStarItem(StarItem star) {
    if (!star.isActive) return;
    
    Color starColor = GetRainbowColor(star.colorTimer);
    
    // Draw star shape (simplified as octagon for now)
    DrawCircle((int)star.position.x, (int)star.position.y, star.radius, starColor);
    
    // Add sparkle effect
    float sparkleRadius = star.radius * 0.3f;
    Color sparkleColor = (Color){255, 255, 255, 200};
    DrawCircle((int)(star.position.x - star.radius * 0.5f), 
               (int)(star.position.y - star.radius * 0.5f), 
               sparkleRadius, sparkleColor);
}

bool CheckStarPlayerCollision(StarItem* star, Vector2 playerPos, float playerRadius) {
    if (!star->isActive) return false;
    
    float dx = star->position.x - playerPos.x;
    float dy = star->position.y - playerPos.y;
    float distance = sqrtf(dx * dx + dy * dy);
    
    return distance < (star->radius + playerRadius);
}