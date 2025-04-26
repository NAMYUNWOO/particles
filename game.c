#include "game.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

// Initialize game state and resources
Game InitGame(int screenWidth, int screenHeight) {
    // 랜덤 시드 초기화
    SetRandomSeed(time(NULL));
    
    Game game = {
        .screenWidth = screenWidth,
        .screenHeight = screenHeight,
        .moveSpeed = 2,
        .player = InitPlayer(screenWidth, screenHeight),
        .particles = NULL,  // 초기화는 아래에서
        .deltaTime = 0,
        .lastEnemySpawnTime = GetTime(),
        .enemyCount = 0
    };

    // 파티클 배열 동적 할당
    game.particles = (Particle*)malloc(PARTICLE_COUNT * sizeof(Particle));
    
    // 모든 파티클 초기화
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        game.particles[i] = InitParticle(screenWidth, screenHeight);
    }

    // 적(enemy) 배열 동적 할당
    game.enemies = (Enemy*)malloc(MAX_ENEMIES * sizeof(Enemy));

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
    
    // Enemy spawn and update logic
    float currentTime = GetTime();
    if (currentTime - game->lastEnemySpawnTime >= ENEMY_SPAWN_TIME && game->enemyCount < MAX_ENEMIES) {
        // 동적 배열에 적 추가
        game->enemies[game->enemyCount] = InitEnemy(game->screenWidth, game->screenHeight);
        game->enemyCount++;
        game->lastEnemySpawnTime = currentTime;
    }
    
    // Update all enemies
    for (int i = 0; i < game->enemyCount; i++) {
        UpdateEnemy(&game->enemies[i], game->screenWidth, game->screenHeight);
    }
    
    // 스페이스바를 누르고 있는지 확인
    bool isSpacePressed = IsKeyDown(KEY_SPACE);
    
    // 모든 파티클 업데이트
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

    // Enemy-Particle 충돌 체크 및 health 감소/삭제
    int e = 0;
    while (e < game->enemyCount) {
        for (int p = 0; p < PARTICLE_COUNT; p++) {
            if (CheckCollisionCircles(game->enemies[e].position, game->enemies[e].radius,
                                      game->particles[p].position, 1.0f)) {
                // printf("Enemy %d collided with Particle %d\n", e, p);
                game->enemies[e].health -= 0.001f;
            }
        }
        if (game->enemies[e].health <= 0.0f) {
            // Remove enemy by shifting
            for (int j = e; j < game->enemyCount - 1; j++) {
                game->enemies[j] = game->enemies[j+1];
            }
            game->enemyCount--;
            // 메모리 해제는 필요 없음(동적 배열 전체는 CleanupGame에서 해제)
            continue; // 같은 인덱스에서 다시 검사
        }
        e++;
    }
}

void DrawGame(Game game) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // 모든 파티클 그리기
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            DrawParticlePixel(game.particles[i]);
        }
        
        // Draw all enemies
        for (int i = 0; i < game.enemyCount; i++) {
            DrawEnemy(game.enemies[i]);
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
    if (game->enemies != NULL) {
        free(game->enemies);
        game->enemies = NULL;
    }
} 