#ifndef HP_POTION_H
#define HP_POTION_H

#include "raylib.h"

#define HPPOTION_BASE_SIZE 10.0f
#define POTION_LIFETIME 10.0f
#define POTION_BLINK_TIME 3.0f
#define POTION_SPAWN_INTERVAL 25.0f

typedef struct {
    Vector2 position;
    float radius;
    float lifetime;           // Time remaining before disappearing
    bool isActive;           // Whether potion exists
    bool isBlinking;         // Whether in blinking phase
    float blinkTimer;        // For blink animation
} HPPotion;

// Function declarations
HPPotion InitHPPotion(void);
void SpawnHPPotion(HPPotion* potion, int screenWidth, int screenHeight);
void UpdateHPPotion(HPPotion* potion, float deltaTime);
void DrawHPPotion(HPPotion potion);
bool CheckPotionPlayerCollision(HPPotion potion, Vector2 playerPos, float playerRadius);

#endif // HP_POTION_H