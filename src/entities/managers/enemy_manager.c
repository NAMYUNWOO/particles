#include "enemy_manager.h"
#include "../../core/game.h"
#include "../../core/event/event_system.h"
#include "../../core/event/event_types.h"
#include "../../core/memory_pool.h"
#include <stdlib.h>
#include "raymath.h"

// 외부 메모리 풀 참조 선언
extern MemoryPool g_enemyEventPool;
extern MemoryPool g_enemyStateEventPool;
extern MemoryPool g_specialAbilityEventPool;

// Legacy spawn function (kept for compatibility)
void SpawnEnemyIfNeeded(Game* game) {
    // In stage mode, spawning is handled by the stage system
    if (game->currentStageNumber > 0) {
        return;  // Let stage system handle spawning
    }
    
    // Original spawning logic for non-stage mode
    float currentTime = GetTime();
    if (currentTime - game->lastEnemySpawnTime >= ENEMY_SPAWN_TIME && game->enemyCount < MAX_ENEMIES) {
        game->enemies[game->enemyCount] = InitEnemy(game->screenWidth, game->screenHeight);
        
        EnemyEventData* data = MemoryPool_Alloc(&g_enemyEventPool);
        if (data) {
            data->enemyIndex = game->enemyCount;
            data->enemyPtr = &game->enemies[game->enemyCount];
            PublishEvent(EVENT_ENEMY_SPAWNED, data);
        }
        
        game->enemyCount++;
        game->lastEnemySpawnTime = currentTime;
    }
}

// Enhanced update function with AI and special abilities
void UpdateAllEnemies(Game* game) {
    for (int i = 0; i < game->enemyCount; i++) {
        Enemy* enemy = &game->enemies[i];
        float prevVx = enemy->velocity.x;
        float prevVy = enemy->velocity.y;
        
        // Update AI state
        UpdateEnemyAI(enemy, game->player.position, game->deltaTime);
        
        // Update movement pattern
        UpdateEnemyMovement(enemy, game->player.position, game->deltaTime);
        
        // Update base enemy properties
        UpdateEnemy(enemy, game->screenWidth, game->screenHeight, game->deltaTime);
        
        // Execute special abilities
        if (enemy->aiState == AI_STATE_SPECIAL) {
            ExecuteEnemySpecialAbility(enemy, game->player.position);
        }
        
        // Handle teleporter special case
        if (enemy->type == ENEMY_TYPE_TELEPORTER && enemy->specialTimer > TELEPORT_COOLDOWN) {
            SpecialAbilityEventData* data = MemoryPool_Alloc(&g_specialAbilityEventPool);
            if (data) {
                data->enemyIndex = i;
                data->enemyPtr = enemy;
                data->abilityType = 0; // Teleport
                data->position = enemy->position;
                PublishEvent(EVENT_ENEMY_TELEPORTED, data);
            }
        }
        
        // Emit state change event if velocity changed significantly
        if ((prevVx * enemy->velocity.x < 0) || (prevVy * enemy->velocity.y < 0)) {
            EnemyStateEventData* data = MemoryPool_Alloc(&g_enemyStateEventPool);
            if (data) {
                data->enemyIndex = i;
                data->oldState = 0;
                data->newState = 1;
                data->enemyPtr = enemy;
                PublishEvent(EVENT_ENEMY_STATE_CHANGED, data);
            }
        }
        
        // Update AI state based on conditions
        switch (enemy->type) {
            case ENEMY_TYPE_TRACKER:
                // Always chase unless too close
                if (Vector2Distance(enemy->position, game->player.position) < 50.0f) {
                    ChangeEnemyAIState(enemy, AI_STATE_FLEE);
                } else {
                    ChangeEnemyAIState(enemy, AI_STATE_CHASE);
                }
                break;
            case ENEMY_TYPE_BOSS_1:
            case ENEMY_TYPE_BOSS_FINAL:
                // Boss behavior based on phase
                if (enemy->stateData.phase == 0) {
                    ChangeEnemyAIState(enemy, AI_STATE_ATTACK);
                } else if (enemy->stateData.phase == 1) {
                    // Alternate between attack and special
                    if ((int)(GetTime() * 0.5f) % 2 == 0) {
                        ChangeEnemyAIState(enemy, AI_STATE_ATTACK);
                    } else {
                        ChangeEnemyAIState(enemy, AI_STATE_SPECIAL);
                    }
                } else if (enemy->stateData.phase == 2) {
                    // Rage mode - always aggressive
                    ChangeEnemyAIState(enemy, AI_STATE_ATTACK);
                }
                break;
            case ENEMY_TYPE_REPULSOR:
                // Repulsors maintain distance
                if (Vector2Distance(enemy->position, game->player.position) < 200.0f) {
                    ChangeEnemyAIState(enemy, AI_STATE_FLEE);
                } else {
                    ChangeEnemyAIState(enemy, AI_STATE_PATROL);
                }
                break;
            case ENEMY_TYPE_BLACKHOLE:
                // Blackholes slowly drift and occasionally pulse
                if ((int)(GetTime() * 0.3f) % 3 == 0) {
                    ChangeEnemyAIState(enemy, AI_STATE_SPECIAL);  // Pulse phase
                } else {
                    ChangeEnemyAIState(enemy, AI_STATE_PATROL);   // Slow drift
                }
                break;
            case ENEMY_TYPE_BASIC:
            case ENEMY_TYPE_SPEEDY:
            case ENEMY_TYPE_SPLITTER:
            case ENEMY_TYPE_ORBITER:
            case ENEMY_TYPE_TELEPORTER:
            case ENEMY_TYPE_CLUSTER:
            case ENEMY_TYPE_COUNT:
                // No special AI state logic needed
                break;
            default:
                break;
        }
    }
}