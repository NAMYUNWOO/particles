#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"

// 파티클 구조체 (repomix-output.txt 기반)
typedef struct {
    Vector2 position;  // 파티클 위치
    Vector2 velocity;  // 파티클 속도
    Color color;       // 파티클 색상
} Particle;

// 파티클 초기화 (화면 크기 지정)
Particle InitParticle(int screenWidth, int screenHeight);

// 위치, 속도, 색상으로 파티클 초기화
Particle InitParticleCustom(Vector2 pos, Vector2 vel, Color color);

// 파티클과 지정 위치 사이의 거리 계산
float GetParticleDistance(Particle particle, Vector2 otherPos);

// 파티클에서 지정 위치까지의 방향 벡터 구하기
Vector2 GetParticleNormal(Particle particle, Vector2 otherPos);

// 파티클을 특정 위치로 끌어당기기
void AttractParticle(Particle* particle, Vector2 posToAttract, float multiplier);

// 파티클에 마찰 적용
void ApplyFriction(Particle* particle, float amount);

// 파티클 이동 (화면 경계 처리 포함)
void MoveParticle(Particle* particle, int screenWidth, int screenHeight);

// 파티클 그리기
void DrawParticlePixel(Particle particle);

#endif // PARTICLE_H 