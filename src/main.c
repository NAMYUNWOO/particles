#include "raylib.h"
#include "core/game.h"
#include "core/event/event_system.h"
#include "core/input_handler.h"
#include <stdlib.h>

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Particle Simulation");
    SetTargetFPS(60);

    // 이벤트 시스템 초기화
    InitEventSystem();
    
    Game game = InitGame(screenWidth, screenHeight);
    
    // 이벤트 시스템 사용 시 입력 핸들러 초기화
    if (game.useEventSystem) {
        InitInputHandler(&game);
    }

    while (!WindowShouldClose())
    {
        // 프레임 시작 이벤트 발행
        PublishEvent(EVENT_FRAME_START, NULL);
        
        UpdateGame(&game);
        
        // 이벤트 큐 처리
        ProcessEventQueue();
        
        DrawGame(game);
        
        // 프레임 종료 이벤트 발행
        PublishEvent(EVENT_FRAME_END, NULL);
    }

    // 이벤트 시스템 정리
    CleanupEventSystem();
    CleanupGame(&game);
    
    CloseWindow();
    return 0;
} 