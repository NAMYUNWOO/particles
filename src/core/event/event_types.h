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

// 스테이지 변경 이벤트 데이터
typedef struct {
    int oldStageNumber;
    int newStageNumber;
    int enemiesKilled;
    int score;
} StageChangeEventData;

// 스테이지 웨이브 이벤트 데이터
typedef struct {
    int stageNumber;
    int waveNumber;
    int enemyCount;
} StageWaveEventData;

// 특수 능력 이벤트 데이터
typedef struct {
    int enemyIndex;
    void* enemyPtr;
    int abilityType;  // 0: teleport, 1: split, 2: repulse, etc.
    Vector2 position;
} SpecialAbilityEventData;

// 보스 페이즈 변경 이벤트 데이터
typedef struct {
    int enemyIndex;
    void* enemyPtr;
    int oldPhase;
    int newPhase;
    float healthPercentage;
} BossPhaseEventData;

// 파티클 효과 이벤트 데이터
typedef struct {
    Vector2 position;
    int effectType;  // 0: explosion, 1: teleport flash, 2: shield break, etc.
    float radius;
    Color color;
} ParticleEffectEventData;

// 점수 변경 이벤트 데이터
typedef struct {
    int oldScore;
    int newScore;
    int pointsAdded;
    int enemyType;
} ScoreChangeEventData;

// 향후 다른 이벤트 데이터 구조체는 여기에 추가

// Item event data
typedef struct {
    int itemType;    // 0: HP Potion, future: other items
    Vector2 position;
    void* itemPtr;
} ItemEventData;

// Health restoration event data
typedef struct {
    int oldHealth;
    int newHealth;
    int amountRestored;
} HealthRestoredEventData;

#endif // EVENT_TYPES_H 