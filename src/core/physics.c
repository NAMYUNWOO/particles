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
        for (int p = 0; p < PARTICLE_COUNT; p++) {
            if (CheckCollisionEnemyParticle(game->enemies[e], game->particles[p])) {
                game->enemies[e].health -= PARTICLE_ENEMY_DAMAGE;
            }
        }
        // 체력 변화 이벤트 발행
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