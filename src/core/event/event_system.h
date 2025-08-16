#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <stdbool.h>
#include <stddef.h>
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
    EVENT_ENEMY_SPLIT,           // New: Enemy splitting
    
    // 충돌 이벤트
    EVENT_COLLISION_PARTICLE_ENEMY,
    EVENT_COLLISION_PLAYER_ENEMY,
    EVENT_COLLISION_PARTICLE_PARTICLE,
    EVENT_COLLISION_CLUSTER_EXPLOSION, // New: Cluster enemy explosion
    
    // 게임 상태 이벤트
    EVENT_GAME_STATE_CHANGED,
    
    // 스테이지 관련 이벤트
    EVENT_STAGE_STARTED,         // New: Stage started
    EVENT_STAGE_COMPLETED,       // New: Stage completed
    EVENT_STAGE_WAVE_STARTED,    // New: Wave started
    EVENT_STAGE_WAVE_COMPLETED,  // New: Wave completed
    EVENT_STAGE_BOSS_WARNING,    // New: Boss warning
    
    // 특수 능력 이벤트
    EVENT_ENEMY_TELEPORTED,      // New: Enemy teleported
    EVENT_REPULSION_FIELD,       // New: Repulsion field activated
    EVENT_BOSS_PHASE_CHANGED,    // New: Boss phase changed
    
    // 시각 효과 이벤트
    EVENT_PARTICLE_EFFECT,       // New: Particle effect spawned
    EVENT_SCREEN_SHAKE,          // New: Screen shake effect
    
    // 점수 및 통계 이벤트
    EVENT_SCORE_CHANGED,         // New: Score changed
    EVENT_MILESTONE_REACHED,     // New: Achievement/milestone
    
    // Item-related events
    EVENT_ITEM_SPAWNED,          // When any item spawns
    EVENT_ITEM_COLLECTED,        // When player collects item
    EVENT_ITEM_EXPIRED,          // When item disappears
    EVENT_HP_RESTORED,           // When player health is restored
    
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

// 메모리 풀 기반 이벤트 데이터 관리 함수
void InitEventMemoryPool(void);
void DestroyEventMemoryPool(void);
void* AllocEventData(size_t size);
void FreeEventData(void* ptr);

#endif // EVENT_SYSTEM_H