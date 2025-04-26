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

// 특정 방향에서 가장 가까운 파티클 찾기
int FindNearestParticleInDirection(Game* game, Vector2 direction) {
    int nearestIndex = -1;
    float nearestDistance = INFINITY;
    float maxAngleDiff = PI / 4.0f;  // 45도 각도 내의 파티클만 고려

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Vector2 toParticle = {
            game->particles[i].position.x - game->player.position.x,
            game->particles[i].position.y - game->player.position.y
        };

        // 파티클까지의 거리 계산
        float distance = sqrtf(toParticle.x * toParticle.x + toParticle.y * toParticle.y);
        
        if (distance < 1.0f) continue;  // 너무 가까운 파티클은 제외

        // 방향 벡터 정규화
        Vector2 normalizedToParticle = {
            toParticle.x / distance,
            toParticle.y / distance
        };

        // 두 벡터 사이의 각도 계산
        float dotProduct = direction.x * normalizedToParticle.x + direction.y * normalizedToParticle.y;
        float angle = acosf(dotProduct);

        // 지정된 각도 내에 있고 가장 가까운 파티클 선택
        if (angle < maxAngleDiff && distance < nearestDistance) {
            nearestDistance = distance;
            nearestIndex = i;
        }
    }

    return nearestIndex;
}

// 플레이어와 파티클 교체
void SwapPlayerWithParticle(Game* game, int particleIndex) {
    if (particleIndex < 0 || particleIndex >= PARTICLE_COUNT) return;

    // 현재 플레이어의 위치를 저장
    Vector2 playerPos = game->player.position;
    
    // 플레이어를 파티클 위치로 이동
    game->player.position = game->particles[particleIndex].position;
    
    // 파티클을 이전 플레이어 위치로 이동
    game->particles[particleIndex].position = playerPos;
    
    // 파티클 속도 초기화
    game->particles[particleIndex].velocity = (Vector2){0, 0};
}

void UpdateGame(Game* game) {
    game->deltaTime = GetFrameTime();
    
    // WASD 키 입력 처리 - 키를 누르고 있는 동안 계속 작동
    Vector2 direction = {0, 0};
    if (IsKeyDown(KEY_W)) direction = (Vector2){0, -1};
    if (IsKeyDown(KEY_S)) direction = (Vector2){0, 1};
    if (IsKeyDown(KEY_A)) direction = (Vector2){-1, 0};
    if (IsKeyDown(KEY_D)) direction = (Vector2){1, 0};

    // 방향키가 눌려있다면 해당 방향의 가장 가까운 파티클과 교체
    if (direction.x != 0 || direction.y != 0) {
        int nearestIndex = FindNearestParticleInDirection(game, direction);
        if (nearestIndex != -1) {
            SwapPlayerWithParticle(game, nearestIndex);
        }
    }
    
    // 플레이어 업데이트 (방향키로 이동)
    UpdatePlayer(&game->player, game->screenWidth, game->screenHeight, game->moveSpeed);
    
    // 스페이스바를 누르고 있는지 확인
    bool isSpacePressed = IsKeyDown(KEY_SPACE);
    
    // 모든 파티클 업데이트
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        // 플레이어와 파티클 사이의 거리 계산
        float dx = game->particles[i].position.x - game->player.position.x;
        float dy = game->particles[i].position.y - game->player.position.y;
        float distance = sqrtf(dx * dx + dy * dy);
        
        // 마우스 위치로 끌어당김
        AttractParticle(&game->particles[i], game->player.position, 1.0f);
        
        // 스페이스바가 눌려있고 거리가 30 이내인 경우 속도 5배 증가
        if (isSpacePressed && distance <= 30.0f) {
            game->particles[i].velocity.x *= 5.0f;
            game->particles[i].velocity.y *= 5.0f;
        }
        
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