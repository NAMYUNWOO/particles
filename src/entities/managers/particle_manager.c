#include "particle_manager.h"
#include "../../core/game.h"
#include "../explosion.h"
#include <stdio.h>

void UpdateAllParticles(Game* game, bool isSpacePressed) {
    // 디버그 메시지 제거
    
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        if (isSpacePressed) {
            AttractParticle(&game->particles[i], game->player.position, BOOSTED_ATTRACTION_FORCE);
        } else {
            AttractParticle(&game->particles[i], game->player.position, DEFAULT_ATTRACTION_FORCE);
        }
        
        // 마찰 적용 (0.99 = 약간의 감속)
        ApplyFriction(&game->particles[i], 0.99f);
        
        // 파티클 이동 및 화면 경계 처리
        MoveParticle(&game->particles[i], game->screenWidth, game->screenHeight);
    }
}

void UpdateAllExplosionParticles(Game* game) {
    int i = 0;
    while (i < game->explosionParticleCount) {
        UpdateExplosionParticle(&game->explosionParticles[i], game->deltaTime);
        
        if (game->explosionParticles[i].timeToLive <= 0.0f) {
            // Remove particle by shifting
            for (int j = i; j < game->explosionParticleCount - 1; j++) {
                game->explosionParticles[j] = game->explosionParticles[j+1];
            }
            game->explosionParticleCount--;
            continue;
        }
        i++;
    }
}