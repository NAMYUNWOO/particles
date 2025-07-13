#include "item_manager.h"
#include "../../core/event/event_system.h"
#include "../../core/event/event_types.h"
#include "../../core/memory_pool.h"

ItemManager g_itemManager = {0};

// Memory pools for item events
MemoryPool g_itemEventPool;
MemoryPool g_healthEventPool;

void InitItemManager(void) {
    g_itemManager.hpPotion = InitHPPotion();
    g_itemManager.potionSpawnTimer = 0.0f;
    g_itemManager.initialized = true;
    
    // Initialize memory pools
    MemoryPool_Init(&g_itemEventPool, sizeof(ItemEventData), 32);
    MemoryPool_Init(&g_healthEventPool, sizeof(HealthRestoredEventData), 16);
}

void CleanupItemManager(void) {
    g_itemManager.initialized = false;
    MemoryPool_Destroy(&g_itemEventPool);
    MemoryPool_Destroy(&g_healthEventPool);
}

void UpdateItemManager(float deltaTime, int screenWidth, int screenHeight) {
    if (!g_itemManager.initialized) return;
    
    // Update spawn timer
    g_itemManager.potionSpawnTimer += deltaTime;
    
    // Check if should spawn potion
    if (g_itemManager.potionSpawnTimer >= POTION_SPAWN_INTERVAL && 
        !g_itemManager.hpPotion.isActive) {
        SpawnHPPotion(&g_itemManager.hpPotion, screenWidth, screenHeight);
        g_itemManager.potionSpawnTimer = 0.0f;
    }
    
    // Update existing potion
    UpdateHPPotion(&g_itemManager.hpPotion, deltaTime);
}

void DrawItems(void) {
    if (!g_itemManager.initialized) return;
    DrawHPPotion(g_itemManager.hpPotion);
}

void CheckItemCollisions(Player* player) {
    if (!g_itemManager.initialized) return;
    
    // Check HP potion collision
    if (CheckPotionPlayerCollision(
            g_itemManager.hpPotion, 
            player->position, 
            player->size)) {
        
        // Only collect if player needs health
        if (player->health < 3) {
            int oldHealth = player->health;
            player->health = 3; // Full restore
            
            // Deactivate potion
            g_itemManager.hpPotion.isActive = false;
            
            // Publish collected event
            ItemEventData* itemData = MemoryPool_Alloc(&g_itemEventPool);
            if (itemData) {
                itemData->itemType = 0;
                itemData->position = g_itemManager.hpPotion.position;
                itemData->itemPtr = &g_itemManager.hpPotion;
                PublishEvent(EVENT_ITEM_COLLECTED, itemData);
            }
            
            // Publish health restored event
            HealthRestoredEventData* healthData = MemoryPool_Alloc(&g_healthEventPool);
            if (healthData) {
                healthData->oldHealth = oldHealth;
                healthData->newHealth = player->health;
                healthData->amountRestored = player->health - oldHealth;
                PublishEvent(EVENT_HP_RESTORED, healthData);
            }
        }
    }
}