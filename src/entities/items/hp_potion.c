#include "hp_potion.h"
#include "../../core/event/event_system.h"
#include "../../core/event/event_types.h"
#include "../../core/memory_pool.h"
#include <stdlib.h>

HPPotion InitHPPotion(void) {
    HPPotion potion = {
        .position = (Vector2){0, 0},
        .radius = HPPOTION_BASE_SIZE,
        .lifetime = 0.0f,
        .isActive = false,
        .isBlinking = false,
        .blinkTimer = 0.0f
    };
    return potion;
}

void SpawnHPPotion(HPPotion* potion, int screenWidth, int screenHeight) {
    if (potion->isActive) return; // Already active
    
    // Random position within screen bounds (with margin)
    float margin = HPPOTION_BASE_SIZE * 2;
    potion->position.x = margin + (rand() % (int)(screenWidth - 2 * margin));
    potion->position.y = margin + (rand() % (int)(screenHeight - 2 * margin));
    
    potion->lifetime = POTION_LIFETIME;
    potion->isActive = true;
    potion->isBlinking = false;
    potion->blinkTimer = 0.0f;
    
    // Publish spawn event
    extern MemoryPool g_itemEventPool; // Defined in item_manager.c
    // Only publish events if the pool is initialized (for testing compatibility)
    if (g_itemEventPool.blocks != NULL) {
        ItemEventData* data = MemoryPool_Alloc(&g_itemEventPool);
        if (data) {
            data->itemType = 0; // HP Potion
            data->position = potion->position;
            data->itemPtr = potion;
            PublishEvent(EVENT_ITEM_SPAWNED, data);
        }
    }
}

void UpdateHPPotion(HPPotion* potion, float deltaTime) {
    if (!potion->isActive) return;
    
    potion->lifetime -= deltaTime;
    
    // Check if should start blinking
    if (potion->lifetime <= POTION_BLINK_TIME && !potion->isBlinking) {
        potion->isBlinking = true;
    }
    
    // Update blink timer
    if (potion->isBlinking) {
        potion->blinkTimer += deltaTime;
    }
    
    // Check if expired
    if (potion->lifetime <= 0) {
        potion->isActive = false;
        
        // Publish expire event
        extern MemoryPool g_itemEventPool;
        // Only publish events if the pool is initialized (for testing compatibility)
        if (g_itemEventPool.blocks != NULL) {
            ItemEventData* data = MemoryPool_Alloc(&g_itemEventPool);
            if (data) {
                data->itemType = 0;
                data->position = potion->position;
                data->itemPtr = potion;
                PublishEvent(EVENT_ITEM_EXPIRED, data);
            }
        }
    }
}

void DrawHPPotion(HPPotion potion) {
    if (!potion.isActive) return;
    
    // Determine visibility for blinking
    bool visible = true;
    if (potion.isBlinking) {
        // Blink every 0.3 seconds
        visible = ((int)(potion.blinkTimer / 0.3f) % 2) == 0;
    }
    
    if (visible) {
        // Draw potion as a red circle with white cross
        DrawCircle(potion.position.x, potion.position.y, potion.radius, RED);
        
        // Draw white cross
        float crossSize = potion.radius * 0.6f;
        DrawRectangle(
            potion.position.x - crossSize, 
            potion.position.y - 2, 
            crossSize * 2, 
            4, 
            WHITE
        );
        DrawRectangle(
            potion.position.x - 2, 
            potion.position.y - crossSize, 
            4, 
            crossSize * 2, 
            WHITE
        );
    }
}

bool CheckPotionPlayerCollision(HPPotion potion, Vector2 playerPos, float playerRadius) {
    if (!potion.isActive) return false;
    
    return CheckCollisionCircles(
        potion.position, potion.radius,
        playerPos, playerRadius
    );
}