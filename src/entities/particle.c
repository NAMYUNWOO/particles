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

// 토로이달 토폴로지를 고려한 최단 거리 및 방향 계산
Vector2 GetToroidalDirection(Vector2 from, Vector2 to, int screenWidth, int screenHeight, float* outDistance) {
    // 직접 경로
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    
    // X축 방향으로 화면 경계를 통과하는 경우
    float dx_wrap_left = to.x - (from.x - screenWidth);   // 왼쪽 경계 통과
    float dx_wrap_right = to.x - (from.x + screenWidth);  // 오른쪽 경계 통과
    
    // Y축 방향으로 화면 경계를 통과하는 경우
    float dy_wrap_up = to.y - (from.y - screenHeight);    // 위쪽 경계 통과
    float dy_wrap_down = to.y - (from.y + screenHeight);  // 아래쪽 경계 통과
    
    // 최단 거리 선택
    if (fabs(dx_wrap_left) < fabs(dx)) dx = dx_wrap_left;
    if (fabs(dx_wrap_right) < fabs(dx)) dx = dx_wrap_right;
    if (fabs(dy_wrap_up) < fabs(dy)) dy = dy_wrap_up;
    if (fabs(dy_wrap_down) < fabs(dy)) dy = dy_wrap_down;
    
    // 거리 계산
    float distance = sqrt(dx*dx + dy*dy);
    if (outDistance) *outDistance = distance;
    
    // 정규화된 방향 벡터
    Vector2 direction = {0, 0};
    if (distance > 0.0f) {
        direction.x = dx / distance;
        direction.y = dy / distance;
    }
    
    return direction;
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

// 파티클을 특정 위치로 끌어당기기 (기존 방식)
void AttractParticle(Particle* particle, Vector2 posToAttract, float multiplier) {
    float dist = fmaxf(GetParticleDistance(*particle, posToAttract), 0.5f);
    Vector2 normal = GetParticleNormal(*particle, posToAttract);
    
    // 거리에 반비례하는 인력 적용
    particle->velocity.x -= normal.x/dist * multiplier;
    particle->velocity.y -= normal.y/dist * multiplier;
}

// 파티클을 특정 위치로 끌어당기기 (토로이달 토폴로지 적용)
void AttractParticleToroidal(Particle* particle, Vector2 posToAttract, float multiplier, int screenWidth, int screenHeight) {
    float dist;
    Vector2 direction = GetToroidalDirection(particle->position, posToAttract, screenWidth, screenHeight, &dist);
    
    // 최소 거리 제한
    dist = fmaxf(dist, 0.5f);
    
    // 거리에 반비례하는 인력 적용 (방향은 최단 경로)
    particle->velocity.x += direction.x / dist * multiplier;
    particle->velocity.y += direction.y / dist * multiplier;
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