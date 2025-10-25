#include "physics.h"
#include "game.h"
#include "../entities/explosion.h"
#include "event/event_system.h"
#include "event/event_types.h"
#include "memory_pool.h"
#include "raymath.h"
#include <stdlib.h>

// 충돌 이벤트 데이터를 위한 메모리 풀
MemoryPool g_collisionEventPool;
MemoryPool g_enemyEventPool;
MemoryPool g_enemyHealthEventPool;
MemoryPool g_gameStateEventPool;
MemoryPool g_enemyStateEventPool;
MemoryPool g_stageEventPool;
MemoryPool g_specialAbilityEventPool;
MemoryPool g_bossPhaseEventPool;

// External memory pools from item_manager.c
extern MemoryPool g_itemEventPool;
extern MemoryPool g_healthEventPool;

static bool poolsInitialized = false;

float PARTICLE_ENEMY_DAMAGE = 0.001f;

// 메모리 풀 초기화
void InitPhysicsMemoryPools(void) {
    if (!poolsInitialized) {
        MemoryPool_Init(&g_collisionEventPool, sizeof(CollisionEventData), 64);
        MemoryPool_Init(&g_enemyEventPool, sizeof(EnemyEventData), 64);
        MemoryPool_Init(&g_enemyHealthEventPool, sizeof(EnemyHealthEventData), 64);
        MemoryPool_Init(&g_gameStateEventPool, sizeof(GameStateEventData), 16);
        MemoryPool_Init(&g_enemyStateEventPool, sizeof(EnemyStateEventData), 64);
        MemoryPool_Init(&g_stageEventPool, sizeof(StageChangeEventData), 16);
        MemoryPool_Init(&g_specialAbilityEventPool, sizeof(SpecialAbilityEventData), 32);
        MemoryPool_Init(&g_bossPhaseEventPool, sizeof(BossPhaseEventData), 16);
        poolsInitialized = true;
    }
}

// 메모리 풀 정리
void CleanupPhysicsMemoryPools(void) {
    if (poolsInitialized) {
        MemoryPool_Destroy(&g_collisionEventPool);
        MemoryPool_Destroy(&g_enemyEventPool);
        MemoryPool_Destroy(&g_enemyHealthEventPool);
        MemoryPool_Destroy(&g_gameStateEventPool);
        MemoryPool_Destroy(&g_enemyStateEventPool);
        MemoryPool_Destroy(&g_stageEventPool);
        MemoryPool_Destroy(&g_specialAbilityEventPool);
        MemoryPool_Destroy(&g_bossPhaseEventPool);
        poolsInitialized = false;
    }
}

bool CheckCollisionEnemyParticle(Enemy enemy, Particle particle) {
    return CheckCollisionCircles(enemy.position, enemy.radius, particle.position, 1.0f);
}

// Enhanced collision processing for different enemy types
void ProcessEnemyCollisions(Game* game) {
    // 필요 시 메모리 풀 초기화
    if (!poolsInitialized) {
        InitPhysicsMemoryPools();
    }

    int e = 0;
    while (e < game->enemyCount) {
        float prevHealth = game->enemies[e].health;
        int collisionCount = 0;
        float totalDamage = 0.0f;
        
        // Check if enemy has special collision properties
        bool isRepulsor = (game->enemies[e].type == ENEMY_TYPE_REPULSOR);
        bool hasShield = HasState(game->enemies[e].stateFlags, ENEMY_STATE_SHIELDED) &&
                         game->enemies[e].stateData.shieldHealth > 0;
        
        // Batch processing for particles
        const int BATCH_SIZE = 1000;
        
        for (int batchStart = 0; batchStart < PARTICLE_COUNT; batchStart += BATCH_SIZE) {
            int batchEnd = batchStart + BATCH_SIZE;
            if (batchEnd > PARTICLE_COUNT) batchEnd = PARTICLE_COUNT;
            
            for (int p = batchStart; p < batchEnd; p++) {
                // Quick distance check
                float dx = game->enemies[e].position.x - game->particles[p].position.x;
                float dy = game->enemies[e].position.y - game->particles[p].position.y;
                float distSquared = dx*dx + dy*dy;
                float radiusSum = game->enemies[e].radius + 1.0f;
                
                // Skip if too far
                if (distSquared > radiusSum * radiusSum) continue;
                
                // Check actual collision
                if (CheckCollisionEnemyParticle(game->enemies[e], game->particles[p])) {
                    // Calculate damage based on enemy type
                    float damage = PARTICLE_ENEMY_DAMAGE;
                    
                    // Modify damage based on enemy properties
                    if (hasShield) {
                        damage *= 0.5f; // Shield reduces damage
                    }
                    if (game->enemies[e].type == ENEMY_TYPE_BOSS_1 || 
                        game->enemies[e].type == ENEMY_TYPE_BOSS_FINAL) {
                        damage *= 0.3f; // Bosses take less damage
                    }
                    if (HasState(game->enemies[e].stateFlags, ENEMY_STATE_INVULNERABLE)) {
                        damage = 0.0f; // No damage during invulnerability
                    }
                    
                    totalDamage += damage;
                    collisionCount++;
                    
                    // Apply particle physics based on enemy type
                    if (isRepulsor) {
                        // Repel the particle
                        Vector2 repelDir = Vector2Subtract(game->particles[p].position, game->enemies[e].position);
                        repelDir = Vector2Normalize(repelDir);
                        game->particles[p].velocity.x += repelDir.x * 3.0f;
                        game->particles[p].velocity.y += repelDir.y * 3.0f;
                    }
                }
            }
        }
        
        // Apply damage
        if (totalDamage > 0) {
            DamageEnemy(&game->enemies[e], totalDamage);
        }
        
        // Emit collision event if hits occurred
        if (collisionCount > 0) {
            CollisionEventData* collisionData = MemoryPool_Alloc(&g_collisionEventPool);
            if (collisionData) {
                collisionData->entityAIndex = -1;
                collisionData->entityBIndex = e;
                collisionData->entityAPtr = NULL;
                collisionData->entityBPtr = &game->enemies[e];
                collisionData->entityAType = 0; // Particle
                collisionData->entityBType = 1; // Enemy
                collisionData->impact = totalDamage;
                PublishEvent(EVENT_COLLISION_PARTICLE_ENEMY, collisionData);
            }
        }
        
        // Emit health change event
        if (game->enemies[e].health != prevHealth) {
            EnemyHealthEventData* data = MemoryPool_Alloc(&g_enemyHealthEventPool);
            if (data) {
                data->enemyIndex = e;
                data->oldHealth = prevHealth;
                data->newHealth = game->enemies[e].health;
                data->enemyPtr = &game->enemies[e];
                PublishEvent(EVENT_ENEMY_HEALTH_CHANGED, data);
            }
            
            // Check for boss phase changes
            if (game->enemies[e].type == ENEMY_TYPE_BOSS_1 ||
                game->enemies[e].type == ENEMY_TYPE_BOSS_FINAL) {
                int oldPhase = game->enemies[e].stateData.phase;
                float healthPercent = game->enemies[e].health / game->enemies[e].maxHealth;

                // Emit boss phase event if phase changed
                if (oldPhase != game->enemies[e].stateData.phase) {
                    BossPhaseEventData* phaseData = MemoryPool_Alloc(&g_bossPhaseEventPool);
                    if (phaseData) {
                        phaseData->enemyIndex = e;
                        phaseData->enemyPtr = &game->enemies[e];
                        phaseData->oldPhase = oldPhase;
                        phaseData->newPhase = game->enemies[e].stateData.phase;
                        phaseData->healthPercentage = healthPercent;
                        PublishEvent(EVENT_BOSS_PHASE_CHANGED, phaseData);
                    }
                }
            }
        }
        
        // Handle enemy destruction
        if (game->enemies[e].health <= 0.0f) {
            // Special handling for different enemy types
            Enemy* dyingEnemy = &game->enemies[e];
            
            // Handle splitting enemies
            if (dyingEnemy->type == ENEMY_TYPE_SPLITTER) {
                HandleEnemySplit(game, dyingEnemy);
            }
            
            // Handle cluster explosions
            if (dyingEnemy->type == ENEMY_TYPE_CLUSTER) {
                HandleClusterExplosion(game, dyingEnemy);
            }
            
            // Create explosion effect
            SpawnExplosion(game->explosionParticles, &game->explosionParticleCount, 
                          dyingEnemy->position, dyingEnemy->color, dyingEnemy->radius);
            
            // Calculate score based on enemy type
            int scoreValue = 100;
            switch (dyingEnemy->type) {
                case ENEMY_TYPE_TRACKER: scoreValue = 150; break;
                case ENEMY_TYPE_SPEEDY: scoreValue = 200; break;
                case ENEMY_TYPE_SPLITTER: scoreValue = 250; break;
                case ENEMY_TYPE_ORBITER: scoreValue = 180; break;
                case ENEMY_TYPE_TELEPORTER: scoreValue = 300; break;
                case ENEMY_TYPE_REPULSOR: scoreValue = 350; break;
                case ENEMY_TYPE_CLUSTER: scoreValue = 220; break;
                case ENEMY_TYPE_BOSS_1: scoreValue = 1000; break;
                case ENEMY_TYPE_BOSS_FINAL: scoreValue = 2000; break;
                default: break;
            }
            
            // Apply stage multiplier
            if (game->currentStageNumber > 0) {
                scoreValue = (int)(scoreValue * (1.0f + game->currentStageNumber * 0.1f));
            }
            
            game->score += scoreValue;
            game->totalEnemiesKilled++;
            game->enemiesKilledThisStage++;
            
            // Emit enemy destroyed event
            EnemyEventData* data = MemoryPool_Alloc(&g_enemyEventPool);
            if (data) {
                data->enemyIndex = e;
                data->enemyPtr = dyingEnemy;
                PublishEvent(EVENT_ENEMY_DESTROYED, data);
            }
            
            // Remove enemy by shifting array
            for (int j = e; j < game->enemyCount - 1; j++) {
                game->enemies[j] = game->enemies[j+1];
            }
            game->enemyCount--;
            continue;
        }
        e++;
    }
}