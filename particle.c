#include "particle.h"
#include <math.h>
#include <stdlib.h>

// 랜덤 범위 내의 float 값을 반환
float RandomFloat(float min, float max) {
    float scale = rand() / (float) RAND_MAX;
    return min + scale * (max - min);
}

// 파티클 초기화 (화면 크기 지정)
Particle InitParticle(int screenWidth, int screenHeight) {
    Particle particle;
    
    // 화면 내 랜덤한 위치 지정
    particle.position.x = GetRandomValue(0, screenWidth-1);
    particle.position.y = GetRandomValue(0, screenHeight-1);
    
    // 랜덤한 초기 속도 (-1.0 ~ 1.0 범위)
    particle.velocity.x = GetRandomValue(-100, 100) / 100.0f;
    particle.velocity.y = GetRandomValue(-100, 100) / 100.0f;
    
    // 검은색 파티클 (투명도 100)
    particle.color = (Color){0, 0, 0, 100};
    
    return particle;
}

// 위치, 속도, 색상으로 파티클 초기화
Particle InitParticleCustom(Vector2 pos, Vector2 vel, Color color) {
    Particle particle;
    
    particle.position = pos;
    particle.velocity = vel;
    particle.color = color;
    
    return particle;
}

// 파티클과 지정 위치 사이의 거리 계산
float GetParticleDistance(Particle particle, Vector2 otherPos) {
    const float dx = particle.position.x - otherPos.x;
    const float dy = particle.position.y - otherPos.y;
    return sqrt((dx*dx) + (dy*dy));
}

// 파티클에서 지정 위치까지의 방향 벡터 구하기
Vector2 GetParticleNormal(Particle particle, Vector2 otherPos) {
    float dist = GetParticleDistance(particle, otherPos);
    if (dist == 0.0f) dist = 1.0f;
    
    const float dx = particle.position.x - otherPos.x;
    const float dy = particle.position.y - otherPos.y;
    
    Vector2 normal;
    normal.x = dx * (1.0f/dist);
    normal.y = dy * (1.0f/dist);
    
    return normal;
}

// 파티클을 특정 위치로 끌어당기기
void AttractParticle(Particle* particle, Vector2 posToAttract, float multiplier) {
    float dist = fmaxf(GetParticleDistance(*particle, posToAttract), 0.5f);
    Vector2 normal = GetParticleNormal(*particle, posToAttract);
    
    // 거리에 반비례하는 인력 적용
    particle->velocity.x -= normal.x/dist * multiplier;
    particle->velocity.y -= normal.y/dist * multiplier;
}

// 파티클에 마찰 적용
void ApplyFriction(Particle* particle, float amount) {
    particle->velocity.x *= amount;
    particle->velocity.y *= amount;
}

// 파티클 이동 (화면 경계 처리 포함)
void MoveParticle(Particle* particle, int screenWidth, int screenHeight) {
    // 위치 업데이트
    particle->position.x += particle->velocity.x;
    particle->position.y += particle->velocity.y;
    
    // 화면 경계 처리 (화면 끝에 닿으면 반대편에서 나타남)
    if (particle->position.x < 0)
        particle->position.x += screenWidth;
    if (particle->position.x >= screenWidth)
        particle->position.x -= screenWidth;
    if (particle->position.y < 0)
        particle->position.y += screenHeight;
    if (particle->position.y >= screenHeight)
        particle->position.y -= screenHeight;
}

// 파티클 그리기 (단일 픽셀)
void DrawParticlePixel(Particle particle) {
    DrawPixelV(particle.position, particle.color);
}