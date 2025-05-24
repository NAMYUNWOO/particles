#ifndef PLAYER_H

#define PLAYER_H

#include "raylib.h"

// Constants
#define PLAYER_BASE_SIZE 10.0f  // Base player size in pixels
#define BOOST_GAUGE_MAX 100.0f
#define BOOST_GAUGE_REGEN 20.0f // per second - 충전 속도 절반으로 감소
#define PARTICLE_BOOST_CONSUME 10.0f   // SPACEBAR - 파티클 끌어당김 (per second)
#define SPEED_BOOST_CONSUME 100.0f     // SHIFT - 속도 부스트 (per second)

// Player structure
typedef struct {
    Vector2 position;
    float size;        // Current player size
    int health;        // Player health (max 3)
    float invincibleTimer; // Invincibility timer
    bool isInvincible; // Invincibility flag
    float boostGauge;  // Boost gauge (0~BOOST_GAUGE_MAX)
    bool isBoosting;   // Boosting flag (파티클 끌어당김)
    bool isSpeedBoosting; // Speed boosting flag (이동 속도 증가)
} Player;

// Initialize player
Player InitPlayer(int screenWidth, int screenHeight);

// Update player position and invincibility
void UpdatePlayer(Player* player, int screenWidth, int screenHeight, int moveSpeed, float deltaTime);

// Draw player
void DrawPlayer(Player player);

// Damage player (decrease health, trigger invincibility)
void DamagePlayer(Player* player);

#endif // PLAYER_H