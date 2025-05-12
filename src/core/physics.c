#include "physics.h"
#include "game.h"
#include "../entities/explosion.h"
#include "event/event_system.h"
#include "event/event_types.h"
#include "memory_pool.h"
#include <stdlib.h>

// 충돌 이벤트 데이터를 위한 메모리 풀
MemoryPool g_collisionEventPool;
MemoryPool g_enemyEventPool;
MemoryPool g_enemyHealthEventPool;
MemoryPool g_gameStateEventPool;
MemoryPool g_enemyStateEventPool;
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
        poolsInitialized = false;
    }
}

bool CheckCollisionEnemyParticle(Enemy enemy, Particle particle) {
    return CheckCollisionCircles(enemy.position, enemy.radius, particle.position, 1.0f);
}

// 이전 방식: 모든 파티클과 모든 적 사이의 충돌 체크
void ProcessEnemyCollisions(Game* game) {
    // 필요 시 메모리 풀 초기화
    if (!poolsInitialized) {
        InitPhysicsMemoryPools();
    }

    int e = 0;
    while (e < game->enemyCount) {
        float prevHealth = game->enemies[e].health;
        int collisionCount = 0;  // 충돌 발생 횟수 추적
        
        // 최적화 1: 파티클을 묶어서 처리 (배치 처리)
        // 모든 파티클을 한 번에 체크하지 않고 배치로 나누어 처리
        const int BATCH_SIZE = 1000;  // 한 번에 처리할 파티클 수
        
        for (int batchStart = 0; batchStart < PARTICLE_COUNT; batchStart += BATCH_SIZE) {
            int batchEnd = batchStart + BATCH_SIZE;
            if (batchEnd > PARTICLE_COUNT) batchEnd = PARTICLE_COUNT;
            
            for (int p = batchStart; p < batchEnd; p++) {
                // 최적화 2: 거리 기반 빠른 필터링 (선택적)
                float dx = game->enemies[e].position.x - game->particles[p].position.x;
                float dy = game->enemies[e].position.y - game->particles[p].position.y;
                float distSquared = dx*dx + dy*dy;
                float radiusSum = game->enemies[e].radius + 1.0f;  // 파티클 반지름은 1.0f
                
                // 빠른 거리 체크로 명백히 충돌하지 않는 경우 건너뛰기
                if (distSquared > radiusSum * radiusSum) continue;
                
                if (CheckCollisionEnemyParticle(game->enemies[e], game->particles[p])) {
                    // 기존: 매 충돌마다 이벤트 발행 대신 직접 체력 감소
                    game->enemies[e].health -= PARTICLE_ENEMY_DAMAGE;
                    collisionCount++;
                }
            }
        }
        
        // 충돌이 발생했다면 한 번의 누적 이벤트만 발행
        if (collisionCount > 0) {
            // 메모리 풀에서 할당
            CollisionEventData* collisionData = MemoryPool_Alloc(&g_collisionEventPool);
            if (collisionData) {
                collisionData->entityAIndex = -1;  // 다수의 파티클이므로 -1로 설정
                collisionData->entityBIndex = e;
                collisionData->entityAPtr = NULL;  // 다수의 파티클이므로 NULL
                collisionData->entityBPtr = &game->enemies[e];
                collisionData->entityAType = 0; // 0: 파티클
                collisionData->entityBType = 1; // 1: 적
                collisionData->impact = PARTICLE_ENEMY_DAMAGE * collisionCount; // 누적 충돌 강도
                PublishEvent(EVENT_COLLISION_PARTICLE_ENEMY, collisionData);
            }
        }
        
        // 체력 변화 이벤트 발행 (메모리 풀 사용)
        if (game->enemies[e].health != prevHealth) {
            EnemyHealthEventData* data = MemoryPool_Alloc(&g_enemyHealthEventPool);
            if (data) {
                data->enemyIndex = e;
                data->oldHealth = prevHealth;
                data->newHealth = game->enemies[e].health;
                data->enemyPtr = &game->enemies[e];
                PublishEvent(EVENT_ENEMY_HEALTH_CHANGED, data);
            }
        }
        
        if (game->enemies[e].health <= 0.0f) {
            // 폭발 파티클 생성
            SpawnExplosion(game->explosionParticles, &game->explosionParticleCount, 
                          game->enemies[e].position, game->enemies[e].color, game->enemies[e].radius);
            // 점수 증가 (고정 100점)
            game->score += 100;
            // 적 파괴 이벤트 발행 (메모리 풀 사용)
            EnemyEventData* data = MemoryPool_Alloc(&g_enemyEventPool);
            if (data) {
                data->enemyIndex = e;
                data->enemyPtr = &game->enemies[e];
                PublishEvent(EVENT_ENEMY_DESTROYED, data);
            }
            // Remove enemy by shifting
            for (int j = e; j < game->enemyCount - 1; j++) {
                game->enemies[j] = game->enemies[j+1];
            }
            game->enemyCount--;
            continue;
        }
        e++;
    }
}