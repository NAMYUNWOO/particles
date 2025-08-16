#include "player.h"
#include <math.h>

Player InitPlayer(int screenWidth, int screenHeight) {
    Player player = {
        .position = (Vector2){ screenWidth/2, screenHeight/2 },
        .size = PLAYER_BASE_SIZE,
        .health = 3,
        .invincibleTimer = 0.0f,
        .isInvincible = false,
        .boostGauge = BOOST_GAUGE_MAX,
        .isBoosting = false,
        .isSpeedBoosting = false
    };
    return player;
}

void UpdatePlayer(Player* player, int screenWidth, int screenHeight, int moveSpeed, float deltaTime) {
    // 기본 이동 속도
    float speed = moveSpeed;
    
    // 게이지 상태 체크
    bool hasEnoughGauge = player->boostGauge > 0.0f;
    
    // Boosting 로직 (파티클 끌어당김 또는 이동 속도 부스트)
    bool isAnyBoostActive = false;
    
    // 속도 부스트 로직 (LEFT_SHIFT)
    if (player->isSpeedBoosting && hasEnoughGauge) {
        // 부스트 게이지가 있으면 속도 부스트 적용
        speed = moveSpeed * 2.0f; // 이동 속도 2배
        isAnyBoostActive = true;
    }
    
    // 파티클 부스트 로직 (SPACE)
    if (player->isBoosting && hasEnoughGauge) {
        isAnyBoostActive = true;
    }
    
    // 부스트 게이지 관리 - 활성화된 부스트가 있는 경우
    if (isAnyBoostActive) {
        float consumeRate = 0.0f;
        // 각 부스트별 소모율 적용
        if (player->isBoosting && hasEnoughGauge) {
            consumeRate += PARTICLE_BOOST_CONSUME;
        }
        if (player->isSpeedBoosting && hasEnoughGauge) {
            consumeRate += SPEED_BOOST_CONSUME;
        }
        player->boostGauge -= consumeRate * deltaTime;
        // 게이지가 고갈되면 부스트 중지
        if (player->boostGauge <= 0.0f) {
            player->boostGauge = 0.0f;
            player->isBoosting = false;
            player->isSpeedBoosting = false;
        }
    } else {
        // 부스트 중이 아닐 때 게이지 회복
        player->boostGauge += BOOST_GAUGE_REGEN * deltaTime;
        if (player->boostGauge > BOOST_GAUGE_MAX) {
            player->boostGauge = BOOST_GAUGE_MAX;
        }
    }
    
    // 이동 방향 계산 (벡터)
    Vector2 direction = {0, 0};
    if (IsKeyDown(KEY_RIGHT)) direction.x += 1;
    if (IsKeyDown(KEY_LEFT)) direction.x -= 1;
    if (IsKeyDown(KEY_DOWN)) direction.y += 1;
    if (IsKeyDown(KEY_UP)) direction.y -= 1;
    
    // 대각선 이동 보정 (방향 벡터 정규화)
    if (direction.x != 0 && direction.y != 0) {
        // 벡터 길이 계산
        float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
        // 방향 벡터 정규화
        direction.x /= length;
        direction.y /= length;
    }
    
    // 속도 적용
    player->position.x += direction.x * speed;
    player->position.y += direction.y * speed;

    // Keep player within screen bounds
    if (player->position.x < 0) player->position.x = 0;
    if (player->position.y < 0) player->position.y = 0;
    if (player->position.x > screenWidth - player->size) player->position.x = screenWidth - player->size;
    if (player->position.y > screenHeight - player->size) player->position.y = screenHeight - player->size;

    // Invincibility timer update
    if (player->isInvincible) {
        player->invincibleTimer -= deltaTime;
        if (player->invincibleTimer <= 0.0f) {
            player->isInvincible = false;
            player->invincibleTimer = 0.0f;
        }
    }
}

void DrawPlayer(Player player) {
    DrawRectangle(
        player.position.x,
        player.position.y,
        player.size,
        player.size,
        RED
    );
}

void DamagePlayer(Player* player) {
    if (!player->isInvincible) {
        player->health--;
        player->isInvincible = true;
        player->invincibleTimer = 1.5f;
    }
} 