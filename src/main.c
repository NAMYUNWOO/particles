#include "raylib.h"
#include "core/game.h"
#include "core/event/event_system.h"
#include "core/input_handler.h"
#include "entities/managers/stage_manager.h"
#include <stdlib.h>

// RegisterEnemyEventHandlers 함수 선언
void RegisterEnemyEventHandlers(void);

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Particle Storm - 10 Stages");
    SetTargetFPS(60);

    // 이벤트 시스템 초기화
    InitEventSystem();
    
    // 스테이지 매니저 초기화
    InitStageManager();
    
    Game game = InitGame(screenWidth, screenHeight);
    
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