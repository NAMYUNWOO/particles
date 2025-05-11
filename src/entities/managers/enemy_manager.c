#include "enemy_manager.h"
#include "../../core/game.h"

void SpawnEnemyIfNeeded(Game* game) {
    float currentTime = GetTime();
    if (currentTime - game->lastEnemySpawnTime >= ENEMY_SPAWN_TIME && game->enemyCount < MAX_ENEMIES) {
        // 동적 배열에 적 추가
        game->enemies[game->enemyCount] = InitEnemy(game->screenWidth, game->screenHeight);
        game->enemyCount++;
        game->lastEnemySpawnTime = currentTime;
    }
}

void UpdateAllEnemies(Game* game) {
    for (int i = 0; i < game->enemyCount; i++) {
        UpdateEnemy(&game->enemies[i], game->screenWidth, game->screenHeight, game->deltaTime);
    }
}