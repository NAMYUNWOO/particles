#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "player.h"

// Enemy types
typedef enum {
    ENEMY_TYPE_BASIC,          // Stage 1: Basic enemy with random movement
    ENEMY_TYPE_TRACKER,        // Stage 2: Tracks player position
    ENEMY_TYPE_SPEEDY,         // Stage 3: Fast moving enemy
    ENEMY_TYPE_SPLITTER,       // Stage 4: Splits into smaller enemies when destroyed
    ENEMY_TYPE_ORBITER,        // Stage 5: Orbits around a point
    ENEMY_TYPE_BOSS_1,         // Stage 6: First boss
    ENEMY_TYPE_TELEPORTER,     // Stage 7: Teleports randomly
    ENEMY_TYPE_REPULSOR,       // Stage 8: Pushes particles away
    ENEMY_TYPE_CLUSTER,        // Stage 9: Explodes in chain reaction
    ENEMY_TYPE_BOSS_FINAL,     // Stage 10: Final boss
    ENEMY_TYPE_COUNT
} EnemyType;

// Enemy movement patterns
typedef enum {
    MOVE_PATTERN_RANDOM,       // Random direction changes
    MOVE_PATTERN_STRAIGHT,     // Straight line movement
    MOVE_PATTERN_TRACKING,     // Follows player
    MOVE_PATTERN_CIRCULAR,     // Circular motion
    MOVE_PATTERN_ZIGZAG,       // Zigzag pattern
    MOVE_PATTERN_SPIRAL,       // Spiral motion
    MOVE_PATTERN_TELEPORT,     // Teleportation
    MOVE_PATTERN_PATROL,       // Patrol between points
    MOVE_PATTERN_WAVE,         // Wave motion
    MOVE_PATTERN_AGGRESSIVE    // Aggressive pursuit
} MovementPattern;

// Enemy AI states
typedef enum {
    AI_STATE_IDLE,
    AI_STATE_PATROL,
    AI_STATE_CHASE,
    AI_STATE_ATTACK,
    AI_STATE_FLEE,
    AI_STATE_SPECIAL
} AIState;

// Enemy entity structure
typedef struct Enemy {
    Vector2 position;          // Current position
    Vector2 velocity;          // Current velocity
    Vector2 targetPosition;    // Target position for certain patterns
    Color color;              // Enemy color
    Color originalColor;      // Original color for effects
    float spawnTime;          // Time when enemy was spawned
    float radius;             // Enemy radius
    float health;
    float maxHealth;
    float damage;
    
    // Type and behavior
    EnemyType type;
    MovementPattern movePattern;
    AIState aiState;
    
    // Pattern-specific data
    float patternTimer;       // Timer for pattern changes
    float specialTimer;       // Timer for special abilities
    float angle;              // Current angle for circular movements
    float orbitRadius;        // Radius for orbiting enemies
    Vector2 orbitCenter;      // Center point for orbiting
    int splitCount;           // Number of times this enemy can split
    bool hasShield;           // Shield status
    float shieldHealth;       // Shield health
    
    // Boss-specific data
    int phase;                // Boss phase
    float phaseTimer;         // Timer for phase changes
    bool isInvulnerable;      // Invulnerability during phase change
} Enemy;

// Constants
#define MAX_ENEMIES 50         // Increased for more complex stages
#define ENEMY_SPAWN_TIME 0.8f  // Base spawn interval
#define ENEMY_MIN_SIZE 10.0f
#define ENEMY_MAX_SIZE 20.0f

// Enemy type specific constants
#define TRACKER_SPEED_MULT 1.5f
#define SPEEDY_SPEED_MULT 3.0f
#define TELEPORT_COOLDOWN 2.0f
#define BOSS_SIZE_MULT 5.0f
#define SPLIT_SIZE_REDUCTION 0.5f
#define REPULSE_RADIUS 150.0f
#define CLUSTER_EXPLOSION_RADIUS 100.0f

// Enemy initialization by type
Enemy InitEnemyByType(EnemyType type, int screenWidth, int screenHeight, Vector2 playerPos);
Enemy InitEnemy(int screenWidth, int screenHeight);

// Enemy update and render functions
void UpdateEnemy(Enemy* enemy, int screenWidth, int screenHeight, float deltaTime);
void UpdateEnemyAI(Enemy* enemy, Vector2 playerPos, float deltaTime);
void UpdateEnemyMovement(Enemy* enemy, Vector2 playerPos, float deltaTime);
void DrawEnemy(Enemy enemy);
void DrawEnemyShield(Enemy enemy);

// Special enemy abilities
void ExecuteEnemySpecialAbility(Enemy* enemy, Vector2 playerPos);
bool ShouldEnemySplit(Enemy* enemy);
void ApplyRepulsionField(Enemy* enemy, void* particles, int particleCount);

// Enemy state management
void ChangeEnemyAIState(Enemy* enemy, AIState newState);
void DamageEnemy(Enemy* enemy, float damage);

#endif // ENEMY_H 