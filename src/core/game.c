#include "game.h"
#include "input_handler.h"
#include "physics.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raymath.h"
#include "event/event_system.h"
#include "event/event_types.h"
#include "memory_pool.h"
#include "../entities/managers/stage_manager.h"

#define SCOREBOARD_FILENAME "scoreboard.txt"

// Define global screen dimensions
int g_screenWidth = 800;
int g_screenHeight = 800;

// 외부 메모리 풀 참조 선언
extern MemoryPool g_collisionEventPool;
extern MemoryPool g_enemyEventPool;
extern MemoryPool g_enemyHealthEventPool;
extern MemoryPool g_gameStateEventPool;
extern MemoryPool g_enemyStateEventPool;

// 추가 메모리 풀 정의
static MemoryPool g_stageChangeEventPool;
static MemoryPool g_specialAbilityEventPool;
static MemoryPool g_particleEffectEventPool;
static bool g_additionalPoolsInitialized = false;

// Initialize game state and resources
Game InitGame(int screenWidth, int screenHeight) {
    // Set global screen dimensions
    g_screenWidth = screenWidth;
    g_screenHeight = screenHeight;
    
    // 랜덤 시드 초기화
    SetRandomSeed(time(NULL));
    
    // 이벤트 시스템 초기화
    InitEventSystem();
    
    // 물리 시스템 메모리 풀 초기화
    InitPhysicsMemoryPools();
    
    // 추가 메모리 풀 초기화 (한 번만)
    if (!g_additionalPoolsInitialized) {
        MemoryPool_Init(&g_stageChangeEventPool, sizeof(StageChangeEventData), 32);
        MemoryPool_Init(&g_specialAbilityEventPool, sizeof(SpecialAbilityEventData), 32);
        MemoryPool_Init(&g_particleEffectEventPool, sizeof(ParticleEffectEventData), 32);
        g_additionalPoolsInitialized = true;
    }
    
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
        .gameState = GAME_STATE_TUTORIAL,
        .playerName[0] = '\0',
        .nameLength = 0,
        .scoreboardCount = 0,
        .useEventSystem = true,  // 이벤트 시스템 사용 설정
        .currentStageNumber = 0,  // Start with no stage
        .stageTimer = 0.0f,
        .stageTransition = false,
        .totalEnemiesKilled = 0,
        .enemiesKilledThisStage = 0
    };

    // 파티클 배열 동적 할당
    game.particles = (Particle*)malloc(PARTICLE_COUNT * sizeof(Particle));
    
    // 모든 파티클 초기화
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        game.particles[i] = InitParticle(screenWidth, screenHeight);
    }

    // 적(enemy) 배열 동적 할당
    game.enemies = (Enemy*)malloc(MAX_ENEMIES * sizeof(Enemy));
    
    // Initialize item manager
    InitItemManager();
    game.itemManager = &g_itemManager;
    
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
// game.c 파일에서 UpdateGame 함수 내 수정
void UpdateGame(Game* game) {
    game->deltaTime = GetFrameTime();
    
    // 이벤트 시스템 사용 시 입력 이벤트 처리 - main.c에서 처리하므로 제거
    // if (game->useEventSystem) {
    //     ProcessInputEvents();
    // }

    // Test mode update
    if (game->gameState == GAME_STATE_TEST_MODE) {
        // Update test mode logic
        UpdateTestMode(&game->testModeState, game);

        // Update player
        UpdatePlayer(&game->player, game->screenWidth, game->screenHeight, game->moveSpeed, game->deltaTime);

        // Update particles
        UpdateAllParticles(game, IsKeyDown(KEY_SPACE));

        // Update enemies
        UpdateAllEnemies(game);

        // Handle collisions
        ProcessEnemyCollisions(game);

        // Exit test mode with ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->gameState = GAME_STATE_TUTORIAL;
        }

        return;
    }

    if (game->gameState == GAME_STATE_TUTORIAL) {
        if (IsKeyPressed(KEY_ENTER)) {
            // 여기에 새로운 코드 추가: TUTORIAL → PLAYING 전환 시 게임 리소스 리셋
            game->player = InitPlayer(game->screenWidth, game->screenHeight);
            game->score = 0;
            game->enemyCount = 0;
            game->explosionParticleCount = 0;
            game->lastEnemySpawnTime = GetTime();
            game->totalEnemiesKilled = 0;
            game->enemiesKilledThisStage = 0;
            
            // 파티클 재초기화
            for (int i = 0; i < PARTICLE_COUNT; i++) {
                game->particles[i] = InitParticle(game->screenWidth, game->screenHeight);
            }
            
            // Reset spawn timing for new game
            ResetSpawnTiming();
            
            // set first stage (starts from 1) - Blackhole stage
            LoadStage(game, 1);
            
            // 게임 상태 직접 설정 (LoadStage에서도 설정하지만 명시적으로)
            game->gameState = GAME_STATE_STAGE_INTRO;
            
            // 선택적: 상태 변경 이벤트 발행
            if (game->useEventSystem) {
                GameStateEventData* stateData = MemoryPool_Alloc(&g_gameStateEventPool);
                if (stateData) {
                    stateData->oldState = GAME_STATE_TUTORIAL;
                    stateData->newState = GAME_STATE_STAGE_INTRO;
                    PublishEvent(EVENT_GAME_STATE_CHANGED, stateData);
                }
            }
        }
        return;
    }
    
    if (game->gameState == GAME_STATE_STAGE_INTRO) {
        // Debug output
        // printf("STAGE_INTRO: state=%d, stateTimer=%.1f\n", game->currentStage.state, game->currentStage.stateTimer);
        
        // Phase 1: Stage introduction (3 seconds)
        if (game->currentStage.state == STAGE_STATE_INTRO && game->currentStage.stateTimer > 3.0f) {
            // printf("Transitioning from INTRO to COUNTDOWN\n");
            game->currentStage.state = STAGE_STATE_COUNTDOWN;
            game->currentStage.stateTimer = 0.0f; // Reset timer for countdown phase
        }
        // Phase 2: Countdown (3 seconds) 
        else if (game->currentStage.state == STAGE_STATE_COUNTDOWN && game->currentStage.stateTimer > 3.0f) {
            // printf("Transitioning from COUNTDOWN to PLAYING\n");
            game->gameState = GAME_STATE_PLAYING;
            game->currentStage.state = STAGE_STATE_ACTIVE;
        }
        // Boss warning handling
        else if (game->currentStage.state == STAGE_STATE_BOSS_WARNING && game->currentStage.stateTimer > 2.0f) {
            // printf("Transitioning from BOSS_WARNING to COUNTDOWN\n");
            game->currentStage.state = STAGE_STATE_COUNTDOWN;
            game->currentStage.stateTimer = 0.0f; // Reset timer for countdown phase
        }
        
        // Update stage timer manually instead of calling UpdateStage to avoid conflicts
        game->currentStage.stateTimer += game->deltaTime;
        return;
    }
    
    if (game->gameState == GAME_STATE_STAGE_COMPLETE) {
        if (IsKeyPressed(KEY_ENTER)) {
            TransitionToNextStage(game);
        }
        return;
    }
    
    if (game->gameState == GAME_STATE_VICTORY) {
        if (IsKeyPressed(KEY_ENTER)) {
            game->gameState = GAME_STATE_SCORE_ENTRY;
        }
        return;
    }
    
    if (game->gameState == GAME_STATE_OVER) {
        // Enter name state on any key
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            game->gameState = GAME_STATE_SCORE_ENTRY;
            game->playerName[0] = '\0';
            game->nameLength = 0;
            
            // 게임 상태 변경 이벤트 발행
            GameStateEventData* stateData = MemoryPool_Alloc(&g_gameStateEventPool);
            if (stateData) {
                stateData->oldState = GAME_STATE_OVER;
                stateData->newState = GAME_STATE_SCORE_ENTRY;
                PublishEvent(EVENT_GAME_STATE_CHANGED, stateData);
            }
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
            // 새로운 코드: 점수 저장 후 단순히 상태만 변경
            AddScoreToScoreboard(game);
            
            // 단순히 상태만 TUTORIAL로 변경 (리소스 리셋은 TUTORIAL → PLAYING 전환 시 수행)
            game->gameState = GAME_STATE_TUTORIAL;
            // Reset stage number to ensure proper enemy spawning on restart
            game->currentStageNumber = 0;
            
            // 선택적: 상태 변경 이벤트 발행
            if (game->useEventSystem) {
                GameStateEventData* stateData = MemoryPool_Alloc(&g_gameStateEventPool);
                if (stateData) {
                    stateData->oldState = GAME_STATE_SCORE_ENTRY;
                    stateData->newState = GAME_STATE_TUTORIAL;
                    PublishEvent(EVENT_GAME_STATE_CHANGED, stateData);
                }
            }
        }
        return;
    }
    
    if (game->gameState == GAME_STATE_PLAYING) {
        // Update stage system
        UpdateStageSystem(game);
        
        // 이벤트 시스템을 사용하지 않을 경우에만 직접 입력 처리
        if (!game->useEventSystem) {
            // 직접 입력 방식에서만 키 상태 직접 설정
            bool isSpacePressed = IsKeyDown(KEY_SPACE);
            bool isShiftPressed = IsKeyDown(KEY_LEFT_SHIFT);
            game->player.isBoosting = isSpacePressed;
            game->player.isSpeedBoosting = isShiftPressed;
        }
        
        // 플레이어 업데이트 (방향키로 이동)
        UpdatePlayer(&game->player, game->screenWidth, game->screenHeight, game->moveSpeed, game->deltaTime);
        
        // Update enemies with AI
        for (int i = 0; i < game->enemyCount; i++) {
            UpdateEnemyAI(&game->enemies[i], game->player.position, game->deltaTime);
            UpdateEnemyMovement(&game->enemies[i], game->player.position, game->deltaTime);
            UpdateEnemy(&game->enemies[i], game->screenWidth, game->screenHeight, game->deltaTime);
            
            // Apply special field effects
            if (game->enemies[i].type == ENEMY_TYPE_REPULSOR) {
                // Apply repulsion to nearby particles
                for (int p = 0; p < PARTICLE_COUNT; p++) {
                    float dx = game->particles[p].position.x - game->enemies[i].position.x;
                    float dy = game->particles[p].position.y - game->enemies[i].position.y;
                    float dist = sqrtf(dx*dx + dy*dy);
                    Vector2 repulseDir = {0, 0};
                    if (dist > 0.0f) {
                        repulseDir.x = dx / dist;
                        repulseDir.y = dy / dist;
                    }
                    if (dist < REPULSE_RADIUS && dist > 1.0f) {
                        // Invert direction for repulsion
                        repulseDir.x = -repulseDir.x;
                        repulseDir.y = -repulseDir.y;
                        float repulseForce = (1.0f - dist / REPULSE_RADIUS) * 2.0f;
                        game->particles[p].velocity.x += repulseDir.x * repulseForce;
                        game->particles[p].velocity.y += repulseDir.y * repulseForce;
                    }
                }
            } else if (game->enemies[i].type == ENEMY_TYPE_BLACKHOLE) {
                // Check if other enemies exist
                int otherEnemiesCount = 0;
                for (int j = 0; j < game->enemyCount; j++) {
                    if (j != i && game->enemies[j].health > 0) {
                        otherEnemiesCount++;
                    }
                }
                
                // Debug output
                // static float lastDebugBlackhole = 0;
                // if (game->stageTimer - lastDebugBlackhole > 1.0f) {
                //     printf("BLACKHOLE: otherEnemies=%d, isInvuln=%d, hasPulsed=%d, stormTimer=%.1f\n", 
                //            otherEnemiesCount, game->enemies[i].isInvulnerable, 
                //            game->enemies[i].hasPulsed, game->enemies[i].stormCycleTimer);
                //     lastDebugBlackhole = game->stageTimer;
                // }
                
                // Update blackhole state based on other enemies
                if (otherEnemiesCount == 0 &&
                    HasState(game->enemies[i].stateFlags, ENEMY_STATE_INVULNERABLE) &&
                    !HasState(game->enemies[i].stateFlags, ENEMY_STATE_PULSED)) {
                    // printf("BLACKHOLE TRANSFORMATION TRIGGERED!\n");
                    // All other enemies are dead, perform pulse and transform immediately
                    SetState(&game->enemies[i].stateFlags, ENEMY_STATE_PULSED);
                    ClearState(&game->enemies[i].stateFlags, ENEMY_STATE_INVULNERABLE);
                    game->enemies[i].movePattern = MOVE_PATTERN_TRACKING;
                    game->enemies[i].color = (Color){150, 0, 50, 255};  // Reddish color when active
                    game->enemies[i].aiState = AI_STATE_CHASE;
                    // Increase speed
                    game->enemies[i].velocity.x *= 3.0f;
                    game->enemies[i].velocity.y *= 3.0f;
                    
                    // Create a powerful radial pulse that pushes all particles away
                    #define PULSE_RADIUS 400.0f
                    #define PULSE_FORCE 20.0f
                    for (int p = 0; p < PARTICLE_COUNT; p++) {
                        float dx = game->particles[p].position.x - game->enemies[i].position.x;
                        float dy = game->particles[p].position.y - game->enemies[i].position.y;
                        float dist = sqrtf(dx*dx + dy*dy);
                        Vector2 pulseDir = {0, 0};
                        if (dist > 0.0f) {
                            pulseDir.x = dx / dist;
                            pulseDir.y = dy / dist;
                        }
                        if (dist < PULSE_RADIUS && dist > 1.0f) {
                            // Push particles away from blackhole center (invert direction)
                            pulseDir.x = -pulseDir.x;
                            pulseDir.y = -pulseDir.y;
                            float pulsePower = (1.0f - dist / PULSE_RADIUS) * PULSE_FORCE;
                            game->particles[p].velocity.x += pulseDir.x * pulsePower;
                            game->particles[p].velocity.y += pulseDir.y * pulsePower;
                        }
                    }
                }
                
                // Apply semi-magnetic storm after transformation (cycles every 5 seconds)
                if (HasState(game->enemies[i].stateFlags, ENEMY_STATE_PULSED) && game->enemies[i].type == ENEMY_TYPE_BLACKHOLE) {
                    // Update storm cycle timer
                    game->enemies[i].stateData.stormCycleTimer += game->deltaTime;
                    if (game->enemies[i].stateData.stormCycleTimer >= 6.0f) {
                        game->enemies[i].stateData.stormCycleTimer = 0.0f;  // Reset every 6 seconds (5 on, 1 off)
                    }

                    // Check if storm is active (first 5 seconds of cycle)
                    bool stormActive = game->enemies[i].stateData.stormCycleTimer < 5.0f;

                    // Update color based on storm state
                    if (stormActive) {
                        // Calculate storm strength for color interpolation
                        float stormStrength = 1.0f - (game->enemies[i].stateData.stormCycleTimer / 5.0f);
                        // Interpolate from bright red to dark red as storm weakens
                        int redValue = 100 + (int)(100 * stormStrength);  // 200 to 100
                        int greenValue = (int)(50 * (1.0f - stormStrength));  // 0 to 50
                        game->enemies[i].color = (Color){redValue, greenValue, 50, 255};
                    } else {
                        game->enemies[i].color = (Color){100, 150, 50, 255};  // Greenish when vulnerable
                    }
                    
                    // Apply magnetic storm only when active
                    if (stormActive) {
                        #define SEMI_STORM_RADIUS 150.0f
                        #define SEMI_STORM_FORCE 3.0f
                        
                        // Calculate storm strength that decreases over time (1.0 to 0.0 over 5 seconds)
                        // float stormStrength = 1.0f - (game->enemies[i].stormCycleTimer / 5.0f);
                        
                        // Alternative: Use sine wave for smoother transition
                        // float stormStrength = fmaxf(cosf((game->enemies[i].stormCycleTimer / 5.0f) * PI * 0.5f), 0.5f);
                        float stormStrength = 1.0f;
                        
                        
                        for (int p = 0; p < PARTICLE_COUNT; p++) {
                            float dx = game->particles[p].position.x - game->enemies[i].position.x;
                            float dy = game->particles[p].position.y - game->enemies[i].position.y;
                            float dist = sqrtf(dx*dx + dy*dy);
                            Vector2 repelDir = {0, 0};
                            if (dist > 0.0f) {
                                repelDir.x = dx / dist;
                                repelDir.y = dy / dist;
                            }
                            if (dist < SEMI_STORM_RADIUS && dist > 1.0f) {
                                // 70% chance to repel each particle when storm is active
                                if (GetRandomValue(1, 10) <= 7) {
                                    // repelDir is already enemy->particle direction, so use it directly for repulsion
                                    float distanceFactor = 1.0f - (dist / SEMI_STORM_RADIUS);
                                    float repelForce = distanceFactor * SEMI_STORM_FORCE * stormStrength;
                                    game->particles[p].velocity.x += repelDir.x * repelForce;
                                    game->particles[p].velocity.y += repelDir.y * repelForce;
                                }
                            }
                        }
                    }
                }
                
                // Only apply gravity if still has invulnerability and hasn't pulsed yet
                if (HasState(game->enemies[i].stateFlags, ENEMY_STATE_INVULNERABLE) &&
                    !HasState(game->enemies[i].stateFlags, ENEMY_STATE_PULSED)) {
                    // Apply strong attraction to nearby particles
                    #define BLACKHOLE_RADIUS 200.0f
                    #define BLACKHOLE_FORCE 5.0f
                    for (int p = 0; p < PARTICLE_COUNT; p++) {
                        float dx = game->enemies[i].position.x - game->particles[p].position.x;
                        float dy = game->enemies[i].position.y - game->particles[p].position.y;
                        float dist = sqrtf(dx*dx + dy*dy);
                        Vector2 attractDir = {0, 0};
                        if (dist > 0.0f) {
                            attractDir.x = dx / dist;
                            attractDir.y = dy / dist;
                        }
                        if (dist < BLACKHOLE_RADIUS && dist > game->enemies[i].radius) {
                            float attractForce = (1.0f - dist / BLACKHOLE_RADIUS) * BLACKHOLE_FORCE;
                            game->particles[p].velocity.x += attractDir.x * attractForce;
                            game->particles[p].velocity.y += attractDir.y * attractForce;
                        }
                    }
                }
            }
        }
        
        // Legacy enemy spawn (only if not using stage system)
        if (game->currentStageNumber == 0) {
            SpawnEnemyIfNeeded(game);
            UpdateAllEnemies(game);
        }
        
        // 모든 파티클 업데이트 (이벤트 처리된 isBoosting 값 사용)
        UpdateAllParticles(game, game->player.isBoosting);

        // Enemy-Particle 충돌 체크 및 이벤트 발행
        ProcessEnemyCollisions(game);
        
        // Update items and check item collisions
        UpdateItemManager(game->deltaTime, game->screenWidth, game->screenHeight);
        CheckItemCollisions(&game->player);
        
        // 플레이어-적 충돌 체크
        for (int i = 0; i < game->enemyCount; i++) {
            float px = game->player.position.x + game->player.size/2;
            float py = game->player.position.y + game->player.size/2;
            // Ignore collision for first 0.5s after enemy spawn
            if (GetTime() - game->enemies[i].spawnTime < 0.5f) continue;
            if (CheckCollisionCircles((Vector2){px, py}, game->player.size/2, game->enemies[i].position, game->enemies[i].radius)) {
                // 플레이어-적 충돌 이벤트 발행 (메모리 풀 사용)
                CollisionEventData* collisionData = MemoryPool_Alloc(&g_collisionEventPool);
                if (collisionData) {
                    collisionData->entityAIndex = 0; // 플레이어는 단일 엔티티이므로 인덱스는 0
                    collisionData->entityBIndex = i;
                    collisionData->entityAPtr = &game->player;
                    collisionData->entityBPtr = &game->enemies[i];
                    collisionData->entityAType = 2; // 2: 플레이어
                    collisionData->entityBType = 1; // 1: 적
                    collisionData->impact = 1.0f; // 플레이어-적 충돌은 치명적
                    PublishEvent(EVENT_COLLISION_PLAYER_ENEMY, collisionData);
                }
            }
        }
        
        // 폭발 파티클 업데이트
        UpdateAllExplosionParticles(game);
    }
}

void DrawGame(Game* game) {
    BeginDrawing();

    if (game->gameState == GAME_STATE_PLAYING || game->gameState == GAME_STATE_STAGE_INTRO || game->gameState == GAME_STATE_TEST_MODE) {
        ClearBackground(game->currentStage.backgroundColor);
    } else {
        ClearBackground(RAYWHITE);
    }

    // Test mode rendering
    if (game->gameState == GAME_STATE_TEST_MODE) {
        // Draw particles
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            DrawParticlePixel(game->particles[i]);
        }

        // Draw enemies
        for (int i = 0; i < game->enemyCount; i++) {
            DrawEnemy(game->enemies[i]);
        }

        // Draw player
        if (!game->player.isInvincible || ((int)(GetTime() * 10) % 2 == 0)) {
            DrawRectangle(game->player.position.x, game->player.position.y,
                         game->player.size, game->player.size, RED);
        }

        // Draw test mode UI
        DrawTestModeUI(&game->testModeState, game->screenWidth, game->screenHeight);

        // Draw enemy state debug info (for nearest enemy to cursor)
        DrawEnemyStateDebug(game, game->screenWidth, game->screenHeight);

        // Draw FPS
        DrawFPS(10, 10);

        EndDrawing();
        return;
    }

    if (game->gameState == GAME_STATE_TUTORIAL) {
        DrawText("How to Play", 320, 200, 32, DARKBLUE);
        DrawText("Move: Arrow keys", 260, 260, 24, BLACK);
        DrawText("Attract particles: SPACE", 260, 300, 24, BLACK);
        DrawText("Speed boost: Shift", 260, 340, 24, BLACK);
        DrawText("Press Enter to Start", 260, 380, 24, RED);
        EndDrawing();
        return;
    }
    
    // Draw stage-specific UI
    if (game->gameState == GAME_STATE_STAGE_INTRO) {
        if (game->currentStage.state == STAGE_STATE_INTRO) {
            // Phase 1: Show stage introduction/description
            DrawStageIntro(&game->currentStage, game->screenWidth, game->screenHeight);
        } else if (game->currentStage.state == STAGE_STATE_COUNTDOWN) {
            // Phase 2: White background with countdown
            ClearBackground(RAYWHITE);
            
            float remainingTime = 3.0f - game->currentStage.stateTimer;
            if (remainingTime > 0) {
                int countdown = (int)ceilf(remainingTime);
                char countdownText[32];
                
                // Determine countdown text and color
                Color textColor = WHITE;
                int fontSize = 120;
                
                if (countdown >= 1) {
                    sprintf(countdownText, "%d", countdown);
                    // Color based on countdown number
                    switch(countdown) {
                        case 3: textColor = RED; break;
                        case 2: textColor = ORANGE; break;
                        case 1: textColor = YELLOW; break;
                        default: textColor = WHITE; break;
                    }
                } else {
                    strcpy(countdownText, "START!");
                    textColor = GREEN;
                    fontSize = 80;
                }
                
                // Add intense pulsing effect based on countdown urgency
                float pulseSpeed = (countdown <= 1) ? 12.0f : 8.0f;
                float pulseIntensity = (countdown <= 1) ? 0.4f : 0.2f;
                float pulseScale = 1.0f + sinf(GetTime() * pulseSpeed) * pulseIntensity;
                int adjustedFontSize = (int)(fontSize * pulseScale);
                
                // Center the text
                int textWidth = MeasureText(countdownText, adjustedFontSize);
                int textX = game->screenWidth / 2 - textWidth / 2;
                int textY = game->screenHeight / 2 - adjustedFontSize / 2;
                
                // Add glowing background circle with color matching countdown
                Color circleColor = Fade(textColor, 0.3f);
                float circleRadius = 120 + sinf(GetTime() * pulseSpeed) * 20;
                DrawCircle(game->screenWidth / 2, game->screenHeight / 2, circleRadius, circleColor);
                
                // Draw multiple shadow layers for glow effect
                for (int i = 8; i >= 1; i--) {
                    Color shadowColor = Fade(BLACK, 0.1f);
                    DrawText(countdownText, textX + i, textY + i, adjustedFontSize, shadowColor);
                }
                
                // Draw main text with outline
                DrawText(countdownText, textX + 1, textY + 1, adjustedFontSize, BLACK);
                DrawText(countdownText, textX, textY, adjustedFontSize, textColor);
                
                // Add screen flash effect for "1" and "START!"
                if (countdown == 1 || strcmp(countdownText, "START!") == 0) {
                    float flashAlpha = (sinf(GetTime() * 15.0f) + 1.0f) * 0.1f;
                    DrawRectangle(0, 0, game->screenWidth, game->screenHeight, Fade(WHITE, flashAlpha));
                }
            }
        } else if (game->currentStage.state == STAGE_STATE_BOSS_WARNING) {
            // Boss warning screen
            DrawBossWarning(game->screenWidth, game->screenHeight);
            
            // Boss warning countdown
            float remainingTime = 2.0f - game->currentStage.stateTimer;
            if (remainingTime > 0) {
                int countdown = (int)ceilf(remainingTime);
                char countdownText[32];
                
                if (countdown >= 1) {
                    sprintf(countdownText, "BOSS IN %d", countdown);
                } else {
                    strcpy(countdownText, "BOSS FIGHT!");
                }
                
                // Pulsing red warning
                float pulseScale = 1.0f + sinf(GetTime() * 10.0f) * 0.3f;
                int fontSize = (int)(60 * pulseScale);
                Color textColor = Fade(RED, 0.8f + sinf(GetTime() * 15.0f) * 0.2f);
                
                int textWidth = MeasureText(countdownText, fontSize);
                int textX = game->screenWidth / 2 - textWidth / 2;
                int textY = game->screenHeight / 2 + 100;
                
                DrawText(countdownText, textX + 2, textY + 2, fontSize, BLACK);
                DrawText(countdownText, textX, textY, fontSize, textColor);
            }
        }
        EndDrawing();
        return;
    }
    
    if (game->gameState == GAME_STATE_STAGE_COMPLETE) {
        DrawStageComplete(&game->currentStage, game->screenWidth, game->screenHeight);
        EndDrawing();
        return;
    }
    
    if (game->gameState == GAME_STATE_VICTORY) {
        DrawRectangle(0, 0, game->screenWidth, game->screenHeight, Fade(GOLD, 0.7f));
        const char* victoryText = "VICTORY!";
        int fontSize = 72;
        int textWidth = MeasureText(victoryText, fontSize);
        DrawText(victoryText, game->screenWidth/2 - textWidth/2, game->screenHeight/2 - 100, fontSize, WHITE);
        
        char scoreText[64];
        sprintf(scoreText, "Final Score: %d", game->score);
        fontSize = 36;
        textWidth = MeasureText(scoreText, fontSize);
        DrawText(scoreText, game->screenWidth/2 - textWidth/2, game->screenHeight/2, fontSize, WHITE);
        
        const char* continueText = "Press Enter to save your score";
        fontSize = 24;
        textWidth = MeasureText(continueText, fontSize);
        DrawText(continueText, game->screenWidth/2 - textWidth/2, game->screenHeight/2 + 60, fontSize, WHITE);
        EndDrawing();
        return;
    }

    // Only draw game objects during PLAYING state
    if (game->gameState == GAME_STATE_PLAYING) {
        // 모든 파티클 그리기
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            DrawParticlePixel(game->particles[i]);
        }
        
        // 폭발 파티클 그리기
        for (int i = 0; i < game->explosionParticleCount; i++) {
            DrawExplosionParticle(game->explosionParticles[i]);
        }
        
        // Draw all enemies
        for (int i = 0; i < game->enemyCount; i++) {
            DrawEnemy(game->enemies[i]);
        }
        
        // Draw items
        DrawItems();
        
        // 점수 표시
        char scoreText[32];
        sprintf(scoreText, "Score: %d", game->score);
        DrawText(scoreText, 10, 10, 20, BLACK);
        
        // Draw stage progress if in a stage
        if (game->currentStageNumber > 0) {
            DrawStageProgress(&game->currentStage, game->screenWidth);
        }
        // 부스트 게이지 표시 (우상단)
        int barW = 120, barH = 12;
        int barX = game->screenWidth - barW - 10;
        int barY = 10;
        DrawRectangle(barX-2, barY-2, barW+4, barH+4, GRAY); // border
        int boostW = (int)(barW * (game->player.boostGauge/BOOST_GAUGE_MAX));
        DrawRectangle(barX, barY, boostW, barH, SKYBLUE);
        DrawRectangleLines(barX-2, barY-2, barW+4, barH+4, DARKBLUE);
        // UX: If boostGauge <= 50, gray out right half and show lock
        if (game->player.boostGauge <= 50.0f) {
            DrawRectangle(barX + barW/2, barY, barW/2, barH, (Color){180,180,180,180});
            DrawText("BOOST LOCKED", barX + barW/2 - 8, barY - 18, 14, DARKGRAY);
        }
        // 체력(하트) 표시
        for (int i = 0; i < game->player.health; i++) {
            DrawRectangle(10 + i * 30, 40, 20, 20, RED);
        }
        
        // 플레이어 그리기 (무적 시 깜빡임)
        if (!game->player.isInvincible || ((int)(GetTime() * 10) % 2 == 0)) {
            DrawRectangle(game->player.position.x, game->player.position.y, game->player.size, game->player.size, RED);
        }
        
        // FPS 표시
        DrawFPS(10, 70);
    }
    
    // 게임 오버 화면
    if (game->gameState == GAME_STATE_OVER) {
        int sw = game->screenWidth;
        int sh = game->screenHeight;
        DrawText("GAME OVER", sw/2 - 100, sh/2 - 90, 40, RED);
        char scoreText[64];
        sprintf(scoreText, "Final Score: %d", game->score);
        DrawText(scoreText, sw/2 - 100, sh/2 - 40, 30, BLACK);
        DrawText("Press Enter to register your score!", sw/2 - 180, sh/2 + 10, 20, DARKGRAY);
    }
    if (game->gameState == GAME_STATE_SCORE_ENTRY) {
        int sw = game->screenWidth;
        int sh = game->screenHeight;
        DrawText("Enter your name:", sw/2 - 120, sh/2 - 60, 30, BLACK);
        DrawRectangle(sw/2 - 120, sh/2 - 20, 300, 40, LIGHTGRAY);
        DrawText(game->playerName, sw/2 - 110, sh/2 - 10, 30, MAROON);
        if ((int)(GetTime()*2)%2 == 0 && game->nameLength < MAX_NAME_LENGTH-1) {
            DrawText("_", sw/2 - 110 + MeasureText(game->playerName, 30), sh/2 - 10, 30, MAROON);
        }
        DrawText("Press Enter to save", sw/2 - 120, sh/2 + 30, 20, DARKGRAY);
        // Scoreboard display
        DrawText("SCOREBOARD", sw/2 - 100, sh/2 + 70, 28, BLUE);
        for (int i = 0; i < game->scoreboardCount; i++) {
            char entry[64];
            sprintf(entry, "%2d. %-15s %6d", i+1, game->scoreboard[i].name, game->scoreboard[i].score);
            DrawText(entry, sw/2 - 100, sh/2 + 100 + i*28, 24, (i==0)?GOLD:BLACK);
        }
    }
    EndDrawing();
}

// 게임 종료 시 메모리 해제
void CleanupGame(Game* game) {
    if (game->particles) {
        free(game->particles);
        game->particles = NULL;
    }
    
    if (game->enemies) {
        free(game->enemies);
        game->enemies = NULL;
    }
    
    // 물리 시스템 메모리 풀 정리
    CleanupPhysicsMemoryPools();
    
    // 추가 메모리 풀 정리
    if (g_additionalPoolsInitialized) {
        MemoryPool_Destroy(&g_stageChangeEventPool);
        MemoryPool_Destroy(&g_specialAbilityEventPool);
        MemoryPool_Destroy(&g_particleEffectEventPool);
        g_additionalPoolsInitialized = false;
    }
    
    // Cleanup item manager
    CleanupItemManager();
    
    // 이벤트 시스템 정리
    CleanupEventSystem();
}

ScoreboardResult LoadScoreboard(Game* game, const char* filename) {
    if (!game) return SCOREBOARD_FILE_ERROR;
    
    // 스코어보드 초기화
    game->scoreboardCount = 0;
    memset(game->scoreboard, 0, sizeof(ScoreEntry) * MAX_SCOREBOARD_ENTRIES);
    
    FILE* f = fopen(filename, "r");
    if (!f) return SCOREBOARD_FILE_ERROR;
    
    char name[MAX_NAME_LENGTH];
    int score;
    
    while (game->scoreboardCount < MAX_SCOREBOARD_ENTRIES) {
        if (fscanf(f, "%15s %d", name, &score) != 2) break;
        
        // 안전하게 복사
        strncpy(game->scoreboard[game->scoreboardCount].name, name, MAX_NAME_LENGTH-1);
        game->scoreboard[game->scoreboardCount].name[MAX_NAME_LENGTH-1] = '\0';
        game->scoreboard[game->scoreboardCount].score = score;
        
        game->scoreboardCount++;
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
    // NULL 체크 및 조기 반환
    if (!game || game->nameLength == 0) return;
    
    // 새 항목 초기화
    ScoreEntry newEntry;
    memset(&newEntry, 0, sizeof(ScoreEntry)); // 명시적 초기화
    
    // 이름 복사 (안전하게)
    strncpy(newEntry.name, game->playerName, MAX_NAME_LENGTH - 1);
    newEntry.name[MAX_NAME_LENGTH - 1] = '\0'; // NULL 종료 보장
    newEntry.score = game->score;
    
    // 삽입 위치 찾기 (내림차순)
    int pos = game->scoreboardCount;
    for (int i = 0; i < game->scoreboardCount; i++) {
        if (newEntry.score > game->scoreboard[i].score) {
            pos = i;
            break;
        }
    }
    
    // 배열 크기 증가 (경계 검사)
    if (game->scoreboardCount < MAX_SCOREBOARD_ENTRIES) {
        // 공간 확보를 위해 항목 이동 (안전한 경계 확인 추가)
        for (int i = game->scoreboardCount; i > pos; i--) {
            if (i < MAX_SCOREBOARD_ENTRIES && i-1 >= 0) {
                game->scoreboard[i] = game->scoreboard[i-1];
            }
        }
        // 크기 증가는 이동 후에
        game->scoreboardCount++;
    } else if (pos < MAX_SCOREBOARD_ENTRIES - 1) {
        // 이미 최대 크기인 경우, 마지막 항목 버리고 이동
        for (int i = MAX_SCOREBOARD_ENTRIES - 1; i > pos; i--) {
            game->scoreboard[i] = game->scoreboard[i-1];
        }
    } else {
        // 최대 크기이고 삽입 위치가 범위 밖이면 무시
        return;
    }
    
    // 새 항목 삽입 (경계 검사)
    if (pos >= 0 && pos < MAX_SCOREBOARD_ENTRIES) {
        game->scoreboard[pos] = newEntry;
    }
    
    // 변경사항 저장
    SaveScoreboard(game, SCOREBOARD_FILENAME);
}

// 전방 선언
static void OnGameStateChanged(const Event* event, void* context);
static void OnParticleEnemyCollision(const Event* event, void* context);
static void OnPlayerEnemyCollision(const Event* event, void* context);
static void OnEnemySpawned(const Event* event, void* context);
static void OnEnemyDestroyed(const Event* event, void* context);
static void OnEnemyHealthChanged(const Event* event, void* context);
static void OnEnemyStateChanged(const Event* event, void* context);

// 메모리 풀 유효성 확인 함수
static bool IsFromMemoryPool(MemoryPool* pool, void* ptr) {
    if (!pool || !ptr || !pool->blocks) return false;
    
    char* poolStart = (char*)pool->blocks;
    char* poolEnd = poolStart + (pool->capacity * pool->blockSize);
    
    return (char*)ptr >= poolStart && (char*)ptr < poolEnd;
}

// 게임 상태 변경 이벤트 핸들러
static void OnGameStateChanged(const Event* event, void* context) {
    GameStateEventData* data = (GameStateEventData*)event->data;
    
    // 메모리 풀 체크 후 반환
    if (IsFromMemoryPool(&g_gameStateEventPool, data)) {
        MemoryPool_Free(&g_gameStateEventPool, data);
    } else {
        free(data);
    }
}

// 충돌 이벤트 핸들러 - 파티클-적 충돌
static void OnParticleEnemyCollision(const Event* event, void* context) {
    CollisionEventData* data = (CollisionEventData*)event->data;
    
    // 누적된 충돌 영향력 처리 (체력은 physics.c에서 이미 감소시켰으므로 여기서는 처리하지 않음)
    // 추가적인 특수 효과나 로직이 필요하면 여기에 구현
    
    // 메모리 풀 체크 후 반환
    if (IsFromMemoryPool(&g_collisionEventPool, data)) {
        MemoryPool_Free(&g_collisionEventPool, data);
    } else {
        free(data); // 풀에서 할당되지 않은 경우 일반 free 사용
    }
}

// 플레이어-적 충돌 이벤트 핸들러
static void OnPlayerEnemyCollision(const Event* event, void* context) {
    Game* game = (Game*)context;
    CollisionEventData* data = (CollisionEventData*)event->data;
    
    // 플레이어 피해 적용
    DamagePlayer(&game->player);
    
    // 플레이어 사망 처리
    if (game->player.health <= 0) {
        // 게임 오버 상태로 전환
        game->gameState = GAME_STATE_OVER;
        
        // 게임 상태 변경 이벤트 발행
        GameStateEventData* stateData = MemoryPool_Alloc(&g_gameStateEventPool);
        if (stateData) {
            stateData->oldState = GAME_STATE_PLAYING;
            stateData->newState = GAME_STATE_OVER;
            PublishEvent(EVENT_GAME_STATE_CHANGED, stateData);
        }
    }
    
    // 메모리 풀 체크 후 반환
    if (IsFromMemoryPool(&g_collisionEventPool, data)) {
        MemoryPool_Free(&g_collisionEventPool, data);
    } else {
        free(data);
    }
}

void RegisterCollisionEventHandlers(Game* game) {
    SubscribeToEvent(EVENT_COLLISION_PARTICLE_ENEMY, OnParticleEnemyCollision, game);
    SubscribeToEvent(EVENT_COLLISION_PLAYER_ENEMY, OnPlayerEnemyCollision, game);
    SubscribeToEvent(EVENT_GAME_STATE_CHANGED, OnGameStateChanged, game);
}

// 적 이벤트 샘플 핸들러
static void OnEnemySpawned(const Event* event, void* context) {
    EnemyEventData* data = (EnemyEventData*)event->data;
    
    // 메모리 풀 체크 후 반환
    if (IsFromMemoryPool(&g_enemyEventPool, data)) {
        MemoryPool_Free(&g_enemyEventPool, data);
    } else {
        free(data);
    }
}

static void OnEnemyDestroyed(const Event* event, void* context) {
    EnemyEventData* data = (EnemyEventData*)event->data;
    
    // 메모리 풀 체크 후 반환
    if (IsFromMemoryPool(&g_enemyEventPool, data)) {
        MemoryPool_Free(&g_enemyEventPool, data);
    } else {
        free(data);
    }
}

static void OnEnemyHealthChanged(const Event* event, void* context) {
    EnemyHealthEventData* data = (EnemyHealthEventData*)event->data;
    
    // 메모리 풀 체크 후 반환
    if (IsFromMemoryPool(&g_enemyHealthEventPool, data)) {
        MemoryPool_Free(&g_enemyHealthEventPool, data);
    } else {
        free(data);
    }
}

// 적 상태 변경 이벤트 핸들러
static void OnEnemyStateChanged(const Event* event, void* context) {
    EnemyStateEventData* data = (EnemyStateEventData*)event->data;
    
    // 메모리 풀 체크 후 반환
    if (IsFromMemoryPool(&g_enemyStateEventPool, data)) {
        MemoryPool_Free(&g_enemyStateEventPool, data);
    } else {
        free(data);
    }
}

void RegisterEnemyEventHandlers(void) {
    SubscribeToEvent(EVENT_ENEMY_SPAWNED, OnEnemySpawned, NULL);
    SubscribeToEvent(EVENT_ENEMY_DESTROYED, OnEnemyDestroyed, NULL);
    SubscribeToEvent(EVENT_ENEMY_HEALTH_CHANGED, OnEnemyHealthChanged, NULL);
    SubscribeToEvent(EVENT_ENEMY_STATE_CHANGED, OnEnemyStateChanged, NULL);
}

// Load a specific stage
void LoadStage(Game* game, int stageNumber) {
    // Reset stage-specific counters
    game->enemiesKilledThisStage = 0;
    game->stageTimer = 0.0f;
    game->currentStageNumber = stageNumber;
    
    // Create the appropriate stage
    switch (stageNumber) {
        case 1: game->currentStage = CreateStage1(); break;
        case 2: game->currentStage = CreateStage2(); break;
        case 3: game->currentStage = CreateStage3(); break;
        case 4: game->currentStage = CreateStage4(); break;
        case 5: game->currentStage = CreateStage5(); break;
        case 6: game->currentStage = CreateStage6(); break;
        case 7: game->currentStage = CreateStage7(); break;
        case 8: game->currentStage = CreateStage8(); break;
        case 9: game->currentStage = CreateStage9(); break;
        case 10: game->currentStage = CreateStage10(); break;
        default: game->currentStage = CreateStage1(); break;
    }
    // Ensure all stage progression fields are reset
    game->currentStage.currentWave = 0;
    game->currentStage.waveTimer = 0.0f;
    game->currentStage.stateTimer = 0.0f;
    game->currentStage.totalEnemiesSpawned = 0;
    
    // Clear existing enemies
    game->enemyCount = 0;
    
    // Apply stage modifiers
    if (game->currentStage.particleAttractionMultiplier > 0) {
        // This would affect particle attraction force
    }
    
    // Update particle colors to match stage theme
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        game->particles[i].color = game->currentStage.particleColor;
    }
    
    // Publish stage started event
    StageChangeEventData* stageData = MemoryPool_Alloc(&g_stageChangeEventPool);
    if (stageData) {
        stageData->oldStageNumber = stageNumber - 1;
        stageData->newStageNumber = stageNumber;
        stageData->enemiesKilled = game->totalEnemiesKilled;
        stageData->score = game->score;
        PublishEvent(EVENT_STAGE_STARTED, stageData);
    }
    
    // Set appropriate game state
    if (stageNumber == 6 || stageNumber == 10) {
        game->gameState = GAME_STATE_STAGE_INTRO;
        game->currentStage.state = STAGE_STATE_BOSS_WARNING;
    } else {
        game->gameState = GAME_STATE_STAGE_INTRO;
        game->currentStage.state = STAGE_STATE_INTRO;
    }
}

// Update stage system
void UpdateStageSystem(Game* game) {
    if (game->gameState != GAME_STATE_PLAYING) return;
    
    game->stageTimer += game->deltaTime;
    
    // Update current stage
    UpdateStage(&game->currentStage, game->deltaTime);
    
    // Debug output every 2 seconds
    /*
    static float lastDebugTime = 0;
    if (game->stageTimer - lastDebugTime >= 2.0f) {
        printf("UpdateStageSystem: stageTimer=%.1f, waveTimer=%.1f, currentWave=%d, totalEnemiesSpawned=%d, enemyCount=%d\n", 
               game->stageTimer, game->currentStage.waveTimer, game->currentStage.currentWave, 
               game->currentStage.totalEnemiesSpawned, game->enemyCount);
        
        bool shouldSpawn = ShouldSpawnEnemy(&game->currentStage, game->stageTimer);
        printf("ShouldSpawnEnemy=%s, maxEnemiesAlive=%d\n", shouldSpawn ? "true" : "false", game->currentStage.maxEnemiesAlive);
        lastDebugTime = game->stageTimer;
    }
    */
    
    // Check if we need to spawn enemies
    if (ShouldSpawnEnemy(&game->currentStage, game->stageTimer) && 
        game->enemyCount < game->currentStage.maxEnemiesAlive) {
        SpawnEnemyFromStage(game);
    }
    
    // Debug: Print spawn info every 2 seconds
    // static float lastDebugPrint = 0;
    // if (game->stageTimer - lastDebugPrint > 2.0f) {
    //     printf("DEBUG: Stage %d, Timer: %.1f, Enemies: %d, State: %d, Wave: %d\n", 
    //            game->currentStageNumber, game->stageTimer, game->enemyCount, 
    //            game->currentStage.state, game->currentStage.currentWave);
    //     lastDebugPrint = game->stageTimer;
    // }
    
    // Check stage completion
    CheckStageCompletion(game);
}

// Spawn enemy based on stage configuration
void SpawnEnemyFromStage(Game* game) {
    EnemyType type = GetNextEnemyType(&game->currentStage);
    Vector2 spawnPos = GetEnemySpawnPosition(&game->currentStage, game->screenWidth, game->screenHeight);
    
    
    // Create enemy with stage modifiers
    Enemy newEnemy = InitEnemyByType(type, game->screenWidth, game->screenHeight, game->player.position);
    newEnemy.position = spawnPos;
    
    // Apply stage modifiers
    newEnemy.health *= game->currentStage.enemyHealthMultiplier;
    newEnemy.maxHealth *= game->currentStage.enemyHealthMultiplier;
    newEnemy.velocity.x *= game->currentStage.enemySpeedMultiplier;
    newEnemy.velocity.y *= game->currentStage.enemySpeedMultiplier;
    newEnemy.radius *= game->currentStage.enemySizeMultiplier;
    
    // Add to game
    game->enemies[game->enemyCount] = newEnemy;
    game->currentStage.totalEnemiesSpawned++;
    
    // Publish enemy spawned event
    EnemyEventData* data = MemoryPool_Alloc(&g_enemyEventPool);
    if (data) {
        data->enemyIndex = game->enemyCount;
        data->enemyPtr = &game->enemies[game->enemyCount];
        PublishEvent(EVENT_ENEMY_SPAWNED, data);
    }
    
    game->enemyCount++;
    game->lastEnemySpawnTime = GetTime();
}

// Spawn enemy by type (used for splitting enemies)
void SpawnEnemyByType(Game* game, EnemyType type) {
    if (game->enemyCount >= MAX_ENEMIES) return;
    
    Enemy newEnemy = InitEnemyByType(type, game->screenWidth, game->screenHeight, game->player.position);
    
    // Apply stage modifiers if in a stage
    if (game->gameState == GAME_STATE_PLAYING) {
        newEnemy.health *= game->currentStage.enemyHealthMultiplier;
        newEnemy.maxHealth *= game->currentStage.enemyHealthMultiplier;
        newEnemy.velocity.x *= game->currentStage.enemySpeedMultiplier;
        newEnemy.velocity.y *= game->currentStage.enemySpeedMultiplier;
    }
    
    game->enemies[game->enemyCount++] = newEnemy;
}

// Handle enemy splitting
void HandleEnemySplit(Game* game, Enemy* originalEnemy) {
    if (!ShouldEnemySplit(originalEnemy)) return;
    if (game->enemyCount + 2 > MAX_ENEMIES) return;
    
    // Create two smaller enemies
    for (int i = 0; i < 2; i++) {
        Enemy splitEnemy = InitEnemyByType(ENEMY_TYPE_SPLITTER, game->screenWidth, game->screenHeight, game->player.position);
        
        // Position near original
        splitEnemy.position.x = originalEnemy->position.x + GetRandomValue(-30, 30);
        splitEnemy.position.y = originalEnemy->position.y + GetRandomValue(-30, 30);
        
        // Smaller size and health
        splitEnemy.radius = originalEnemy->radius * SPLIT_SIZE_REDUCTION;
        splitEnemy.health = originalEnemy->maxHealth * 0.5f;
        splitEnemy.maxHealth = splitEnemy.health;
        splitEnemy.stateData.splitCount = originalEnemy->stateData.splitCount - 1;
        
        // Random velocity
        splitEnemy.velocity.x = GetRandomValue(-100, 100) / 50.0f;
        splitEnemy.velocity.y = GetRandomValue(-100, 100) / 50.0f;
        
        game->enemies[game->enemyCount++] = splitEnemy;
        
        // Publish split event
        SpecialAbilityEventData* data = MemoryPool_Alloc(&g_specialAbilityEventPool);
        if (data) {
            data->enemyIndex = game->enemyCount - 1;
            data->enemyPtr = &game->enemies[game->enemyCount - 1];
            data->abilityType = 1; // Split
            data->position = splitEnemy.position;
            PublishEvent(EVENT_ENEMY_SPLIT, data);
        }
    }
}

// Handle cluster explosion
void HandleClusterExplosion(Game* game, Enemy* clusterEnemy) {
    if (clusterEnemy->type != ENEMY_TYPE_CLUSTER) return;
    
    // Check for nearby enemies to trigger chain reaction
    for (int i = 0; i < game->enemyCount; i++) {
        float distance = Vector2Distance(game->enemies[i].position, clusterEnemy->position);
        
        if (distance < CLUSTER_EXPLOSION_RADIUS && distance > 0) {
            // Damage nearby enemies
            float damage = (1.0f - distance / CLUSTER_EXPLOSION_RADIUS) * 50.0f;
            DamageEnemy(&game->enemies[i], damage);
            
            // Push them away
            Vector2 pushDir = Vector2Subtract(game->enemies[i].position, clusterEnemy->position);
            pushDir = Vector2Normalize(pushDir);
            game->enemies[i].velocity.x += pushDir.x * 5.0f;
            game->enemies[i].velocity.y += pushDir.y * 5.0f;
        }
    }
    
    // Create explosion effect
    ParticleEffectEventData* effectData = MemoryPool_Alloc(&g_particleEffectEventPool);
    if (effectData) {
        effectData->position = clusterEnemy->position;
        effectData->effectType = 0; // Explosion
        effectData->radius = CLUSTER_EXPLOSION_RADIUS;
        effectData->color = MAGENTA;
        PublishEvent(EVENT_PARTICLE_EFFECT, effectData);
    }
}

// Check stage completion
void CheckStageCompletion(Game* game) {
    if (game->currentStage.state != STAGE_STATE_ACTIVE) return;
    
    // Update kill count
    game->currentStage.enemiesKilled = game->enemiesKilledThisStage;
    
    if (IsStageComplete(&game->currentStage)) {
        game->currentStage.state = STAGE_STATE_COMPLETE;
        game->gameState = GAME_STATE_STAGE_COMPLETE;
        
        // Publish stage complete event
        StageChangeEventData* data = MemoryPool_Alloc(&g_stageChangeEventPool);
        if (data) {
            data->oldStageNumber = game->currentStageNumber;
            data->newStageNumber = game->currentStageNumber + 1;
            data->enemiesKilled = game->enemiesKilledThisStage;
            data->score = game->score;
            PublishEvent(EVENT_STAGE_COMPLETED, data);
        }
        
        // Bonus score for stage completion
        game->score += 500 * game->currentStageNumber;
    }
}

// Transition to next stage
void TransitionToNextStage(Game* game) {
    if (game->currentStageNumber >= 10) {
        // Victory!
        game->gameState = GAME_STATE_VICTORY;
        return;
    }
    
    LoadStage(game, game->currentStageNumber + 1);
}

// Register stage event handlers
void RegisterStageEventHandlers(Game* game) {
    // Stage event handlers would be implemented here
} 