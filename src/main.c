#include "raylib.h"
#include "core/game.h"
#include "core/event/event_system.h"
#include "core/input_handler.h"
#include "entities/managers/stage_manager.h"
#include "entities/managers/stages/stage_common.h"
#include <stdlib.h>
#include <string.h>

// RegisterEnemyEventHandlers 함수 선언
void RegisterEnemyEventHandlers(void);

/**
 * Parse command line arguments for starting stage
 *
 * @param argc Argument count
 * @param argv Argument values
 * @return Starting stage number (0 = normal game, 1-10 = jump to stage)
 */
int ParseStartingStage(int argc, char *argv[]) {
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "--start-stage") == 0) {
            int stage = atoi(argv[i + 1]);
            if (stage >= 1 && stage <= 10) {
                return stage;
            }
        }
    }
    return 0;  // Normal game start
}

/**
 * Parse command line arguments for test mode
 *
 * @param argc Argument count
 * @param argv Argument values
 * @return true if test mode requested
 */
bool ParseTestMode(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--test-mode") == 0) {
            return true;
        }
    }
    return false;
}

int main(int argc, char *argv[])
{
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Particle Storm - 10 Stages");
    SetTargetFPS(60);

    // Parse command-line arguments for stage selection
    int startingStage = ParseStartingStage(argc, argv);
    bool testMode = ParseTestMode(argc, argv);

    // 이벤트 시스템 초기화
    InitEventSystem();
    
    // 스테이지 매니저 초기화
    InitStageManager();

    Game game = InitGame(screenWidth, screenHeight);

    // Jump to specific stage if requested (for testing)
    if (startingStage > 0) {
        game.currentStageNumber = startingStage - 1;  // Will be incremented to startingStage
        game.gameState = GAME_STATE_STAGE_INTRO;
        // Load the requested stage
        switch(startingStage) {
            case 1: game.currentStage = CreateStage1(); break;
            case 2: game.currentStage = CreateStage2(); break;
            case 3: game.currentStage = CreateStage3(); break;
            case 4: game.currentStage = CreateStage4(); break;
            case 5: game.currentStage = CreateStage5(); break;
            case 6: game.currentStage = CreateStage6(); break;
            case 7: game.currentStage = CreateStage7(); break;
            case 8: game.currentStage = CreateStage8(); break;
            case 9: game.currentStage = CreateStage9(); break;
            case 10: game.currentStage = CreateStage10(); break;
        }
        game.currentStage.state = STAGE_STATE_INTRO;
        game.currentStage.stateTimer = 0.0f;
        ResetSpawnTiming();
    }

    // Jump to test mode if requested
    if (testMode) {
        game.currentStageNumber = 0;
        game.gameState = GAME_STATE_TEST_MODE;
        game.currentStage = CreateStageTest();
        game.currentStage.state = STAGE_STATE_ACTIVE;
        game.testModeState = InitTestMode();
    }

    // 이벤트 시스템 사용 시 입력 핸들러와 충돌 핸들러 등록
    if (game.useEventSystem) {
        // 적 이벤트 샘플 핸들러 등록
        RegisterEnemyEventHandlers();
        // 충돌 이벤트 핸들러 등록
        RegisterCollisionEventHandlers(&game);
        // 스테이지 이벤트 핸들러 등록
        RegisterStageEventHandlers(&game);
        // 입력 핸들러 초기화
        InitInputHandler(&game);
    }

    while (!WindowShouldClose())
    {
        // 프레임 시작 이벤트 발행
        PublishEvent(EVENT_FRAME_START, NULL);
        
        // 키보드 입력 이벤트 처리 (이벤트 발행)
        if (game.useEventSystem) {
            ProcessInputEvents();
        }
        
        // 이벤트 큐 처리 - 게임 업데이트 전에 처리하여 입력 이벤트가 즉시 반영되도록 함
        ProcessEventQueue();
        
        // 게임 업데이트
        UpdateGame(&game);
        
        DrawGame(&game);
        
        // 프레임 종료 이벤트 발행
        PublishEvent(EVENT_FRAME_END, NULL);
    }

    // 입력 핸들러 정리
    if (game.useEventSystem) {
        CleanupInputHandler();
    }
    
    // 이벤트 시스템 정리
    CleanupEventSystem();
    CleanupGame(&game);
    
    CloseWindow();
    return 0;
} 