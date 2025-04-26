#include "game.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

Game InitGame(int screenWidth, int screenHeight) {
    // 랜덤 시드 초기화
    SetRandomSeed(time(NULL));
    
    Game game = {
        .screenWidth = screenWidth,
        .screenHeight = screenHeight,
        .moveSpeed = 2,
        .player = InitPlayer(screenWidth, screenHeight),
        .particles = NULL,  // 초기화는 아래에서
        .deltaTime = 0
    };

    // 파티클 배열 동적 할당
    game.particles = (Particle*)malloc(PARTICLE_COUNT * sizeof(Particle));
    
    // 모든 파티클 초기화
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        game.particles[i] = InitParticle(screenWidth, screenHeight);
    }

    return game;
}

void UpdateGame(Game* game) {
    game->deltaTime = GetFrameTime();
    
    // 플레이어 업데이트
    UpdatePlayer(&game->player, game->screenWidth, game->screenHeight, game->moveSpeed);
    
    // 마우스 위치 가져오기 (마우스가 파티클을 끌어당기는 핵심 요소)
    // Vector2 mousePos = GetMousePosition();
    
    // 모든 파티클 업데이트
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        // 마우스 위치로 끌어당김
        AttractParticle(&game->particles[i], game->player.position, 1.0f);
        
        // 마찰 적용 (0.99 = 약간의 감속)
        ApplyFriction(&game->particles[i], 0.99f);
        
        // 파티클 이동 및 화면 경계 처리
        MoveParticle(&game->particles[i], game->screenWidth, game->screenHeight);
    }
}

void DrawGame(Game game) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // 모든 파티클 그리기
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            DrawParticlePixel(game.particles[i]);
        }
        
        // 플레이어 그리기
        DrawPlayer(game.player);
        
        // FPS 표시
        DrawFPS(10, 10);
        
    EndDrawing();
}

// 게임 종료 시 메모리 해제
void CleanupGame(Game* game) {
    if (game->particles != NULL) {
        free(game->particles);
        game->particles = NULL;
    }
} 