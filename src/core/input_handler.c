#include "input_handler.h"
#include "event/event_system.h"
#include "event/event_types.h"
#include "raylib.h"
#include <stdlib.h>

// 방향키 목록
static const int DIRECTION_KEYS[] = { KEY_W, KEY_A, KEY_S, KEY_D, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };
static const int DIRECTION_KEY_COUNT = 8;

// 액션키 목록
static const int ACTION_KEYS[] = { KEY_SPACE, KEY_Q };
static const int ACTION_KEY_COUNT = 2;

// 키 입력 이벤트 핸들러 (플레이어 이동)
static void HandlePlayerMovementInput(const Event* event, void* context) {
    Game* game = (Game*)context;
    if (!game || game->gameState != GAME_STATE_PLAYING) return;
    
    KeyEventData* keyData = (KeyEventData*)event->data;
    
    // 키가 눌렸을 때만 처리
    if (!keyData->isPressed) return;
    
    Vector2 direction = {0, 0};
    
    // 방향 결정
    switch (keyData->keyCode) {
        case KEY_W:
        case KEY_UP:
            direction = (Vector2){0, -1};
            break;
        case KEY_S:
        case KEY_DOWN:
            direction = (Vector2){0, 1};
            break;
        case KEY_A:
        case KEY_LEFT:
            direction = (Vector2){-1, 0};
            break;
        case KEY_D:
        case KEY_RIGHT:
            direction = (Vector2){1, 0};
            break;
        default:
            return; // 방향키가 아니면 무시
    }
    
    // 여기서는 기존 함수들을 호출하여 플레이어를 움직임
    // 점진적 마이그레이션 중이므로 아직 기존 코드 사용
    int nearestIndex = FindNearestParticleInDirection(game, direction);
    if (nearestIndex != -1) {
        SwapPlayerWithParticle(game, nearestIndex);
    }
}

// 스페이스바 이벤트 핸들러 (부스트 모드)
static void HandleBoostInput(const Event* event, void* context) {
    Game* game = (Game*)context;
    if (!game || game->gameState != GAME_STATE_PLAYING) return;
    
    KeyEventData* keyData = (KeyEventData*)event->data;
    
    if (keyData->keyCode == KEY_SPACE) {
        game->player.isBoosting = keyData->isPressed;
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
    SubscribeToEvent(EVENT_KEY_PRESSED, HandleBoostInput, game);
    SubscribeToEvent(EVENT_KEY_RELEASED, HandleBoostInput, game);
    
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