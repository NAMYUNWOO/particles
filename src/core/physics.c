#include "physics.h"
#include "game.h"
#include "../entities/explosion.h"

bool CheckCollisionEnemyParticle(Enemy enemy, Particle particle) {
    return CheckCollisionCircles(enemy.position, enemy.radius, particle.position, 1.0f);
}

void ProcessEnemyCollisions(Game* game) {
    int e = 0;
    while (e < game->enemyCount) {
        for (int p = 0; p < PARTICLE_COUNT; p++) {
            if (CheckCollisionEnemyParticle(game->enemies[e], game->particles[p])) {
                game->enemies[e].health -= 0.001f;
            }
        }
        
        if (game->enemies[e].health <= 0.0f) {
            // 폭발 파티클 생성
            SpawnExplosion(game->explosionParticles, &game->explosionParticleCount, 
                          game->enemies[e].position, game->enemies[e].color, game->enemies[e].radius);
            
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