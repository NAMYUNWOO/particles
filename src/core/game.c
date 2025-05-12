#include "game.h"
#include "input_handler.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "event/event_system.h"
#include "event/event_types.h"

#define SCOREBOARD_FILENAME "scoreboard.txt"

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
        .enemyCount = 0,
        .explosionParticleCount = 0,
        .score = 0,
        .gameState = GAME_STATE_PLAYING,
        .playerName[0] = '\0',
        .nameLength = 0,
        .scoreboardCount = 0,
        .useEventSystem = true  // 이벤트 시스템 사용 설정
    };

    // 파티클 배열 동적 할당
    game.particles = (Particle*)malloc(PARTICLE_COUNT * sizeof(Particle));
    
    // 모든 파티클 초기화
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        game.particles[i] = InitParticle(screenWidth, screenHeight);
    }

    // 적(enemy) 배열 동적 할당
    game.enemies = (Enemy*)malloc(MAX_ENEMIES * sizeof(Enemy));

    LoadScoreboard(&game, SCOREBOARD_FILENAME);

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
    
    // 이벤트 시스템 사용 시 입력 이벤트 처리
    if (game->useEventSystem) {
        ProcessInputEvents();
    }
    
    if (game->gameState == GAME_STATE_OVER) {
        // Enter name state on any key
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            game->gameState = GAME_STATE_SCORE_ENTRY;
            game->playerName[0] = '\0';
            game->nameLength = 0;
        }
        return;
    }
    if (game->gameState == GAME_STATE_SCORE_ENTRY) {
        int key = GetCharPressed();
        while (key > 0) {
            if (((key >= 32 && key <= 126) || (key >= 128 && key < 255)) && game->nameLength < MAX_NAME_LENGTH-1) {
                game->playerName[game->nameLength++] = (char)key;
                game->playerName[game->nameLength] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && game->nameLength > 0) {
            game->nameLength--;
            game->playerName[game->nameLength] = '\0';
        }
        if (IsKeyPressed(KEY_ENTER) && game->nameLength > 0) {
            AddScoreToScoreboard(game);
            game->gameState = GAME_STATE_PLAYING;
            *game = InitGame(game->screenWidth, game->screenHeight); // Restart game
        }
        return;
    }
    if (game->gameState == GAME_STATE_PLAYING) {
    // 이벤트 시스템을 사용하지 않을 경우에만 직접 입력 처리
    if (!game->useEventSystem) {
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
    }
    // 플레이어 업데이트 (방향키로 이동)
        UpdatePlayer(&game->player, game->screenWidth, game->screenHeight, game->moveSpeed, game->deltaTime);
        // Enemy spawn and update
        SpawnEnemyIfNeeded(game);
        UpdateAllEnemies(game);
    // 스페이스바를 누르고 있는지 확인 (이벤트 시스템 미사용 시)
    bool isSpacePressed = false;
    if (!game->useEventSystem) {
        isSpacePressed = IsKeyDown(KEY_SPACE);
        game->player.isBoosting = isSpacePressed;
    }
    // 모든 파티클 업데이트
        UpdateAllParticles(game, game->player.isBoosting);
        // Enemy-Particle 충돌 체크 및 health 감소/삭제
        ProcessEnemyCollisions(game);
        // 플레이어-적 충돌 체크
        for (int i = 0; i < game->enemyCount; i++) {
            float px = game->player.position.x + game->player.size/2;
            float py = game->player.position.y + game->player.size/2;
            // Ignore collision for first 0.5s after enemy spawn
            if (GetTime() - game->enemies[i].spawnTime < 0.5f) continue;
            if (CheckCollisionCircles((Vector2){px, py}, game->player.size/2, game->enemies[i].position, game->enemies[i].radius)) {
                // 플레이어-적 충돌 이벤트 발행
                CollisionEventData* collisionData = malloc(sizeof(CollisionEventData));
                collisionData->entityAIndex = 0; // 플레이어는 단일 엔티티이므로 인덱스는 0
                collisionData->entityBIndex = i;
                collisionData->entityAPtr = &game->player;
                collisionData->entityBPtr = &game->enemies[i];
                collisionData->entityAType = 2; // 2: 플레이어
                collisionData->entityBType = 1; // 1: 적
                collisionData->impact = 1.0f; // 플레이어-적 충돌은 치명적
                PublishEvent(EVENT_COLLISION_PLAYER_ENEMY, collisionData);
                
                // 기존 처리 로직은 이벤트 핸들러로 이동할 예정이므로 주석 처리
                // DamagePlayer(&game->player);
                // if (game->player.health <= 0) {
                //     game->gameState = GAME_STATE_OVER;
                //     break;
                // }
            }
        }
    // 폭발 파티클 업데이트
        UpdateAllExplosionParticles(game);
    }
}

void DrawGame(Game game) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // 모든 파티클 그리기
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            DrawParticlePixel(game.particles[i]);
        }
        
        // 폭발 파티클 그리기
        for (int i = 0; i < game.explosionParticleCount; i++) {
            DrawExplosionParticle(game.explosionParticles[i]);
        }
        
        // Draw all enemies
        for (int i = 0; i < game.enemyCount; i++) {
            DrawEnemy(game.enemies[i]);
        }
        
        // 점수 표시
        char scoreText[32];
        sprintf(scoreText, "Score: %d", game.score);
        DrawText(scoreText, 10, 10, 20, BLACK);
        // 부스트 게이지 표시 (우상단)
        int barW = 120, barH = 12;
        int barX = game.screenWidth - barW - 10;
        int barY = 10;
        DrawRectangle(barX-2, barY-2, barW+4, barH+4, GRAY); // border
        int boostW = (int)(barW * (game.player.boostGauge/BOOST_GAUGE_MAX));
        DrawRectangle(barX, barY, boostW, barH, SKYBLUE);
        DrawRectangleLines(barX-2, barY-2, barW+4, barH+4, DARKBLUE);
        // UX: If boostGauge <= 50, gray out right half and show lock
        if (game.player.boostGauge <= 50.0f) {
            DrawRectangle(barX + barW/2, barY, barW/2, barH, (Color){180,180,180,180});
            DrawText("BOOST LOCKED", barX + barW/2 - 8, barY - 18, 14, DARKGRAY);
        }
        // 체력(하트) 표시
        for (int i = 0; i < game.player.health; i++) {
            DrawRectangle(10 + i * 30, 40, 20, 20, RED);
        }
        
        // 플레이어 그리기 (무적 시 깜빡임)
        if (!game.player.isInvincible || ((int)(GetTime() * 10) % 2 == 0)) {
            DrawRectangle(game.player.position.x, game.player.position.y, game.player.size, game.player.size, RED);
        }
        
        // FPS 표시
        DrawFPS(10, 70);
        
        // 게임 오버 화면
        if (game.gameState == GAME_STATE_OVER) {
            int sw = game.screenWidth;
            int sh = game.screenHeight;
            DrawText("GAME OVER", sw/2 - 100, sh/2 - 90, 40, RED);
            char scoreText[64];
            sprintf(scoreText, "Final Score: %d", game.score);
            DrawText(scoreText, sw/2 - 100, sh/2 - 40, 30, BLACK);
            DrawText("Press Enter to register your score!", sw/2 - 180, sh/2 + 10, 20, DARKGRAY);
        }
        if (game.gameState == GAME_STATE_SCORE_ENTRY) {
            int sw = game.screenWidth;
            int sh = game.screenHeight;
            DrawText("Enter your name:", sw/2 - 120, sh/2 - 60, 30, BLACK);
            DrawRectangle(sw/2 - 120, sh/2 - 20, 300, 40, LIGHTGRAY);
            DrawText(game.playerName, sw/2 - 110, sh/2 - 10, 30, MAROON);
            if ((int)(GetTime()*2)%2 == 0 && game.nameLength < MAX_NAME_LENGTH-1) {
                DrawText("_", sw/2 - 110 + MeasureText(game.playerName, 30), sh/2 - 10, 30, MAROON);
            }
            DrawText("Press Enter to save", sw/2 - 120, sh/2 + 30, 20, DARKGRAY);
            // Scoreboard display
            DrawText("SCOREBOARD", sw/2 - 100, sh/2 + 70, 28, BLUE);
            for (int i = 0; i < game.scoreboardCount; i++) {
                char entry[64];
                sprintf(entry, "%2d. %-15s %6d", i+1, game.scoreboard[i].name, game.scoreboard[i].score);
                DrawText(entry, sw/2 - 100, sh/2 + 100 + i*28, 24, (i==0)?GOLD:BLACK);
            }
        }
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

ScoreboardResult LoadScoreboard(Game* game, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return SCOREBOARD_FILE_ERROR;
    game->scoreboardCount = 0;
    while (fscanf(f, "%15s %d", game->scoreboard[game->scoreboardCount].name, &game->scoreboard[game->scoreboardCount].score) == 2) {
        game->scoreboardCount++;
        if (game->scoreboardCount >= MAX_SCOREBOARD_ENTRIES) break;
    }
    fclose(f);
    return SCOREBOARD_OK;
}

ScoreboardResult SaveScoreboard(Game* game, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) return SCOREBOARD_FILE_ERROR;
    for (int i = 0; i < game->scoreboardCount; i++) {
        fprintf(f, "%s %d\n", game->scoreboard[i].name, game->scoreboard[i].score);
    }
    fclose(f);
    return SCOREBOARD_OK;
}

void AddScoreToScoreboard(Game* game) {
    // Add new entry
    if (game->nameLength == 0) return;
    ScoreEntry newEntry;
    strncpy(newEntry.name, game->playerName, MAX_NAME_LENGTH);
    newEntry.name[MAX_NAME_LENGTH-1] = '\0';
    newEntry.score = game->score;
    // Insert in sorted order (descending)
    int pos = game->scoreboardCount;
    for (int i = 0; i < game->scoreboardCount; i++) {
        if (newEntry.score > game->scoreboard[i].score) {
            pos = i;
            break;
        }
    }
    if (game->scoreboardCount < MAX_SCOREBOARD_ENTRIES) game->scoreboardCount++;
    for (int i = game->scoreboardCount-1; i > pos; i--) {
        game->scoreboard[i] = game->scoreboard[i-1];
    }
    game->scoreboard[pos] = newEntry;
    SaveScoreboard(game, SCOREBOARD_FILENAME);
}

// 충돌 이벤트 핸들러
static void OnParticleEnemyCollision(const Event* event, void* context) {
    Game* game = (Game*)context;
    CollisionEventData* data = (CollisionEventData*)event->data;
    
    Enemy* enemy = (Enemy*)data->entityBPtr;
    // 파티클-적 충돌 처리
    enemy->health -= data->impact;
    
    // printf("[이벤트] 파티클-적 충돌: particle=%d, enemy=%d, impact=%.3f\n", 
    //       data->entityAIndex, data->entityBIndex, data->impact);
    
    free(data);
}

static void OnPlayerEnemyCollision(const Event* event, void* context) {
    Game* game = (Game*)context;
    CollisionEventData* data = (CollisionEventData*)event->data;
    
    Player* player = (Player*)data->entityAPtr;
    // 플레이어-적 충돌 처리
    DamagePlayer(player);
    if (player->health <= 0) {
        game->gameState = GAME_STATE_OVER;
    }
    
    // printf("[이벤트] 플레이어-적 충돌: playerHealth=%d\n", player->health);
    
    free(data);
}

static void OnParticleParticleCollision(const Event* event, void* context) {
    CollisionEventData* data = (CollisionEventData*)event->data;
    // 파티클-파티클 충돌 처리 (필요한 경우)
    // printf("[이벤트] 파티클-파티클 충돌: particleA=%d, particleB=%d\n", 
    //       data->entityAIndex, data->entityBIndex);
    free(data);
}

void RegisterCollisionEventHandlers(Game* game) {
    SubscribeToEvent(EVENT_COLLISION_PARTICLE_ENEMY, OnParticleEnemyCollision, game);
    SubscribeToEvent(EVENT_COLLISION_PLAYER_ENEMY, OnPlayerEnemyCollision, game);
    SubscribeToEvent(EVENT_COLLISION_PARTICLE_PARTICLE, OnParticleParticleCollision, game);
}

// 적 이벤트 샘플 핸들러
static void OnEnemySpawned(const Event* event, void* context) {
    EnemyEventData* data = (EnemyEventData*)event->data;
    // printf("[이벤트] 적 생성: index=%d, ptr=%p\n", data->enemyIndex, data->enemyPtr);
    free(data);
}
static void OnEnemyDestroyed(const Event* event, void* context) {
    EnemyEventData* data = (EnemyEventData*)event->data;
    // printf("[이벤트] 적 파괴: index=%d, ptr=%p\n", data->enemyIndex, data->enemyPtr);
    free(data);
}
static void OnEnemyHealthChanged(const Event* event, void* context) {
    EnemyHealthEventData* data = (EnemyHealthEventData*)event->data;
    // printf("[이벤트] 적 체력 변화: index=%d, %.1f -> %.1f\n", data->enemyIndex, data->oldHealth, data->newHealth);
    free(data);
}
static void OnEnemyStateChanged(const Event* event, void* context) {
    EnemyStateEventData* data = (EnemyStateEventData*)event->data;
    // printf("[이벤트] 적 상태 변화: index=%d, %d -> %d\n", data->enemyIndex, data->oldState, data->newState);
    free(data);
}

void RegisterEnemyEventHandlers(void) {
    SubscribeToEvent(EVENT_ENEMY_SPAWNED, OnEnemySpawned, NULL);
    SubscribeToEvent(EVENT_ENEMY_DESTROYED, OnEnemyDestroyed, NULL);
    SubscribeToEvent(EVENT_ENEMY_HEALTH_CHANGED, OnEnemyHealthChanged, NULL);
    SubscribeToEvent(EVENT_ENEMY_STATE_CHANGED, OnEnemyStateChanged, NULL);
} 