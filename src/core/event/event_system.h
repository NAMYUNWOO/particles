#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <stdbool.h>
#include "raylib.h"

// 이벤트 타입 정의 (점진적으로 확장 가능)
typedef enum {
    // 시스템 이벤트 (프레임 시작/종료)
    EVENT_FRAME_START,
    EVENT_FRAME_END,
    
    // 입력 이벤트
    EVENT_KEY_PRESSED,
    EVENT_KEY_RELEASED,
    
    // 적 관련 이벤트
    EVENT_ENEMY_SPAWNED,
    EVENT_ENEMY_DESTROYED,
    EVENT_ENEMY_HEALTH_CHANGED,
    EVENT_ENEMY_STATE_CHANGED,
    
    // 충돌 이벤트
    EVENT_COLLISION_PARTICLE_ENEMY,
    EVENT_COLLISION_PLAYER_ENEMY,
    EVENT_COLLISION_PARTICLE_PARTICLE,
    
    // 향후 추가 이벤트를 위한 자리
    
    EVENT_COUNT  // 이벤트 총 개수
} EventType;

// 이벤트 데이터 구조체
typedef struct {
    EventType type;    // 이벤트 유형
    void* data;        // 이벤트 관련 데이터
    double timestamp;  // 이벤트 발생 시간
} Event;

// 이벤트 핸들러 함수 포인터 타입
typedef void (*EventHandler)(const Event* event, void* context);

// 이벤트 시스템 초기화
void InitEventSystem(void);

// 이벤트 시스템 정리
void CleanupEventSystem(void);

// 이벤트 발행
void PublishEvent(EventType type, void* data);

// 이벤트 구독
int SubscribeToEvent(EventType type, EventHandler handler, void* context);

// 이벤트 구독 취소
void UnsubscribeFromEvent(EventType type, int listenerId);

// 이벤트 큐 처리 (게임 루프에서 호출)
void ProcessEventQueue(void);

#endif // EVENT_SYSTEM_H