#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

#include "raylib.h"

// 키보드 입력 이벤트 데이터
typedef struct {
    int keyCode;      // 키 코드 (KEY_A, KEY_SPACE 등)
    bool isPressed;   // 눌림 여부 (true: 눌림, false: 뗌)
} KeyEventData;

// 적 이벤트 데이터
typedef struct {
    int enemyIndex;
    void* enemyPtr;
} EnemyEventData;

// 적 체력 변화 이벤트 데이터
typedef struct {
    int enemyIndex;
    float oldHealth;
    float newHealth;
    void* enemyPtr;
} EnemyHealthEventData;

// 적 상태 변화 이벤트 데이터
typedef struct {
    int enemyIndex;
    int oldState;
    int newState;
    void* enemyPtr;
} EnemyStateEventData;

// 충돌 이벤트 데이터
typedef struct {
    int entityAIndex;
    int entityBIndex;
    void* entityAPtr;
    void* entityBPtr;
    int entityAType; // 0: 파티클, 1: 적, 2: 플레이어
    int entityBType;
    float impact; // 충돌 강도(필요시)
} CollisionEventData;

// 게임 상태 변경 이벤트 데이터
typedef struct {
    int oldState;   // 이전 게임 상태
    int newState;   // 새 게임 상태
} GameStateEventData;

// 향후 다른 이벤트 데이터 구조체는 여기에 추가

#endif // EVENT_TYPES_H 