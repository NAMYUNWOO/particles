#include "physics.h"
#include "game.h"
#include "../entities/explosion.h"
#include "event/event_system.h"
#include "event/event_types.h"
#include <stdlib.h>

float PARTICLE_ENEMY_DAMAGE = 0.001f;

bool CheckCollisionEnemyParticle(Enemy enemy, Particle particle) {
    return CheckCollisionCircles(enemy.position, enemy.radius, particle.position, 1.0f);
}

void ProcessEnemyCollisions(Game* game) {
    int e = 0;
    while (e < game->enemyCount) {
        float prevHealth = game->enemies[e].health;
        int collisionCount = 0;  // 충돌 발생 횟수 추적
        
        for (int p = 0; p < PARTICLE_COUNT; p++) {
            if (CheckCollisionEnemyParticle(game->enemies[e], game->particles[p])) {
                // 기존: 매 충돌마다 이벤트 발행 대신 직접 체력 감소
                game->enemies[e].health -= PARTICLE_ENEMY_DAMAGE;
                collisionCount++;
            }
        }
        
        // 충돌이 발생했다면 한 번의 누적 이벤트만 발행
        if (collisionCount > 0) {
            CollisionEventData* collisionData = malloc(sizeof(CollisionEventData));
            collisionData->entityAIndex = -1;  // 다수의 파티클이므로 -1로 설정
            collisionData->entityBIndex = e;
            collisionData->entityAPtr = NULL;  // 다수의 파티클이므로 NULL
            collisionData->entityBPtr = &game->enemies[e];
            collisionData->entityAType = 0; // 0: 파티클
            collisionData->entityBType = 1; // 1: 적
            collisionData->impact = PARTICLE_ENEMY_DAMAGE * collisionCount; // 누적 충돌 강도
            PublishEvent(EVENT_COLLISION_PARTICLE_ENEMY, collisionData);
        }
        
        // 체력 변화 이벤트 발행 (기존 코드 유지)
        if (game->enemies[e].health != prevHealth) {
            EnemyHealthEventData* data = malloc(sizeof(EnemyHealthEventData));
            data->enemyIndex = e;
            data->oldHealth = prevHealth;
            data->newHealth = game->enemies[e].health;
            data->enemyPtr = &game->enemies[e];
            PublishEvent(EVENT_ENEMY_HEALTH_CHANGED, data);
        }
        
        if (game->enemies[e].health <= 0.0f) {
            // 폭발 파티클 생성
            SpawnExplosion(game->explosionParticles, &game->explosionParticleCount, 
                          game->enemies[e].position, game->enemies[e].color, game->enemies[e].radius);
            // 점수 증가 (고정 100점)
            game->score += 100;
            // 적 파괴 이벤트 발행
            EnemyEventData* data = malloc(sizeof(EnemyEventData));
            data->enemyIndex = e;
            data->enemyPtr = &game->enemies[e];
            PublishEvent(EVENT_ENEMY_DESTROYED, data);
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