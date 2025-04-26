#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "particle.h"

// 파티클 수 (많이 생성하므로 10만개로 설정)
#define PARTICLE_COUNT 100000

typedef struct {
    int screenWidth;
    int screenHeight;
    int moveSpeed;
    Player player;
    Particle* particles;  // 파티클 배열 포인터 (동적 할당)
    float deltaTime;
} Game;

// 게임 초기화
Game InitGame(int screenWidth, int screenHeight);

// 게임 상태 업데이트
void UpdateGame(Game* game);

// 게임 그리기
void DrawGame(Game game);

// 게임 종료 시 메모리 해제
void CleanupGame(Game* game);

#endif // GAME_H 