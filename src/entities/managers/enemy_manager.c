#include "enemy_manager.h"
#include "../../core/game.h"
#include "../../core/event/event_system.h"
#include "../../core/event/event_types.h"
#include <stdlib.h>

void SpawnEnemyIfNeeded(Game* game) {
    float currentTime = GetTime();
    if (currentTime - game->lastEnemySpawnTime >= ENEMY_SPAWN_TIME && game->enemyCount < MAX_ENEMIES) {
        // 동적 배열에 적 추가
        game->enemies[game->enemyCount] = InitEnemy(game->screenWidth, game->screenHeight);
        // 이벤트 발행 (적 생성)
        EnemyEventData* data = malloc(sizeof(EnemyEventData));
        data->enemyIndex = game->enemyCount;
        data->enemyPtr = &game->enemies[game->enemyCount];
        PublishEvent(EVENT_ENEMY_SPAWNED, data);
        game->enemyCount++;
        game->lastEnemySpawnTime = currentTime;
    }
}

void UpdateAllEnemies(Game* game) {
    for (int i = 0; i < game->enemyCount; i++) {
        float prevVx = game->enemies[i].velocity.x;
        float prevVy = game->enemies[i].velocity.y;
        UpdateEnemy(&game->enemies[i], game->screenWidth, game->screenHeight, game->deltaTime);
        // 예시: 속도가 반전되면 상태 변화 이벤트 발행
        if ((prevVx * game->enemies[i].velocity.x < 0) || (prevVy * game->enemies[i].velocity.y < 0)) {
            EnemyStateEventData* data = malloc(sizeof(EnemyStateEventData));
            data->enemyIndex = i;
            data->oldState = 0; // 예시: 0=정상, 1=반사 등
            data->newState = 1;
            data->enemyPtr = &game->enemies[i];
            PublishEvent(EVENT_ENEMY_STATE_CHANGED, data);
        }
    }
}