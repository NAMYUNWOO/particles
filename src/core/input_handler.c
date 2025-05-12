#include "input_handler.h"
#include "event/event_system.h"
#include "event/event_types.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

// 방향키 목록
static const int DIRECTION_KEYS[] = { KEY_W, KEY_A, KEY_S, KEY_D, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };
static const int DIRECTION_KEY_COUNT = 8;

// 액션키 목록 (스페이스바와 Left Shift 키)
static const int ACTION_KEYS[] = { KEY_SPACE, KEY_LEFT_SHIFT };
static const int ACTION_KEY_COUNT = 2;

// 키 입력 이벤트 핸들러 (플레이어 이동)
static void HandlePlayerMovementInput(const Event* event, void* context) {
    Game* game = (Game*)context;
    if (!game || game->gameState != GAME_STATE_PLAYING) return;
    
    KeyEventData* keyData = (KeyEventData*)event->data;
    
    // 키가 눌렸을 때만 처리
    if (!keyData->isPressed) return;
    
    // 주의: 이 함수는 키가 처음 눌렸을 때만 호출됩니다.
    // 실제 이동은 UpdatePlayer에서 IsKeyDown으로 처리됩니다.
    // 따라서 여기서는 별도의 이동 처리를 하지 않습니다.
}

// 스페이스바와 Left Shift 이벤트 핸들러
static void HandleActionInput(const Event* event, void* context) {
    Game* game = (Game*)context;
    if (!game || game->gameState != GAME_STATE_PLAYING) {
        return;
    }
    
    KeyEventData* keyData = (KeyEventData*)event->data;
    
    if (keyData->keyCode == KEY_SPACE) {
        // 부스트 상태 변경
        game->player.isBoosting = keyData->isPressed;
    } else if (keyData->keyCode == KEY_LEFT_SHIFT) {
        // 스피드 부스트 상태 변경
        game->player.isSpeedBoosting = keyData->isPressed;
    }
}

// 키보드 입력 이벤트를 처리한 후 메모리 해제하는 핸들러
static void CleanupEventData(const Event* event, void* context) {
    // 이벤트 데이터가 있으면 해제
    if (event->data) {
        free(event->data);
    }
}

// 이벤트 리스너 등록
void InitInputHandler(Game* game) {
    // 입력 이벤트 핸들러 등록
    SubscribeToEvent(EVENT_KEY_PRESSED, HandlePlayerMovementInput, game);
    SubscribeToEvent(EVENT_KEY_PRESSED, HandleActionInput, game);
    SubscribeToEvent(EVENT_KEY_RELEASED, HandleActionInput, game);
    
    // 이벤트 데이터 정리를 위한 이벤트 핸들러 등록
    // 이 핸들러는 다른 모든 핸들러가 실행된 후에 실행되어야 함
    SubscribeToEvent(EVENT_KEY_PRESSED, CleanupEventData, NULL);
    SubscribeToEvent(EVENT_KEY_RELEASED, CleanupEventData, NULL);
}

// 키 상태를 확인하고 이벤트 발행
void ProcessInputEvents(void) {
    // 방향키 처리
    for (int i = 0; i < DIRECTION_KEY_COUNT; i++) {
        int key = DIRECTION_KEYS[i];
        if (IsKeyPressed(key)) {
            KeyEventData* keyData = (KeyEventData*)malloc(sizeof(KeyEventData));
            keyData->keyCode = key;
            keyData->isPressed = true;
            PublishEvent(EVENT_KEY_PRESSED, keyData);
        }
        else if (IsKeyReleased(key)) {
            KeyEventData* keyData = (KeyEventData*)malloc(sizeof(KeyEventData));
            keyData->keyCode = key;
            keyData->isPressed = false;
            PublishEvent(EVENT_KEY_RELEASED, keyData);
        }
    }
    
    // 액션키 처리
    for (int i = 0; i < ACTION_KEY_COUNT; i++) {
        int key = ACTION_KEYS[i];
        if (IsKeyPressed(key)) {
            KeyEventData* keyData = (KeyEventData*)malloc(sizeof(KeyEventData));
            keyData->keyCode = key;
            keyData->isPressed = true;
            PublishEvent(EVENT_KEY_PRESSED, keyData);
        }
        else if (IsKeyReleased(key)) {
            KeyEventData* keyData = (KeyEventData*)malloc(sizeof(KeyEventData));
            keyData->keyCode = key;
            keyData->isPressed = false;
            PublishEvent(EVENT_KEY_RELEASED, keyData);
        }
    }
}