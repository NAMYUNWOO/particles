#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include "../items/hp_potion.h"
#include "../player.h"

typedef struct {
    HPPotion hpPotion;
    float potionSpawnTimer;
    bool initialized;
} ItemManager;

// Global item manager instance
extern ItemManager g_itemManager;

void InitItemManager(void);
void CleanupItemManager(void);
void UpdateItemManager(float deltaTime, int screenWidth, int screenHeight);
void DrawItems(void);
void CheckItemCollisions(Player* player);

#endif // ITEM_MANAGER_H