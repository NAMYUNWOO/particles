#ifndef GAME_H
#define GAME_H

#include "../entities/player.h"
#include "../entities/particle.h"
#include "../entities/enemy.h"
#include "../entities/explosion.h"
#include "../entities/managers/enemy_manager.h"
#include "../entities/managers/particle_manager.h"

// Constants
#define PARTICLE_COUNT 100000  // Maximum number of particles
#define DEFAULT_ATTRACTION_FORCE 1.0f  // Default force for particle attraction
#define BOOSTED_ATTRACTION_FORCE 5.0f  // Boosted force when space key is pressed

// Game state structure
typedef struct Game {
    // Window properties
    int screenWidth;
    int screenHeight;
    
    // Game properties
    int moveSpeed;
    float deltaTime;
    float lastEnemySpawnTime;  // Time when last enemy was spawned
    int enemyCount;           // Current number of enemies
    
    // Game entities
    Player player;
    Particle* particles;  // Dynamic array of particles
    Enemy* enemies;  // Dynamic array of enemies
    ExplosionParticle explosionParticles[MAX_EXPLOSION_PARTICLES];
    int explosionParticleCount;
} Game;

// Game initialization and cleanup
Game InitGame(int screenWidth, int screenHeight);
void CleanupGame(Game* game);

// Game loop functions
void UpdateGame(Game* game);
void DrawGame(Game game);

// Particle management functions
int FindNearestParticleInDirection(Game* game, Vector2 direction);
void SwapPlayerWithParticle(Game* game, int particleIndex);

// Managers and physics functions
void SpawnEnemyIfNeeded(Game* game);
void UpdateAllEnemies(Game* game);
void UpdateAllParticles(Game* game, bool isSpacePressed);
void UpdateAllExplosionParticles(Game* game);
bool CheckCollisionEnemyParticle(Enemy enemy, Particle particle);
void ProcessEnemyCollisions(Game* game);

#endif // GAME_H 