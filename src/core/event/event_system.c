#include "event_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"

#define MAX_LISTENERS_PER_EVENT 16
#define MAX_EVENT_QUEUE_SIZE 64

// 이벤트 리스너 구조체
typedef struct {
    EventHandler handler;
    void* context;
    bool active;
} EventListener;

// 이벤트 큐
typedef struct {
    Event events[MAX_EVENT_QUEUE_SIZE];
    int head;
    int tail;
    int count;
} EventQueue;

// 이벤트 시스템 내부 상태
static struct {
    EventListener listeners[EVENT_COUNT][MAX_LISTENERS_PER_EVENT];
    int listenerCount[EVENT_COUNT];
    EventQueue queue;
    bool initialized;
} eventSystem;

// 이벤트 시스템 초기화
void InitEventSystem(void) {
    memset(&eventSystem, 0, sizeof(eventSystem));
    eventSystem.initialized = true;
    printf("Event system initialized\n");
}

// 이벤트 시스템 정리
void CleanupEventSystem(void) {
    memset(&eventSystem, 0, sizeof(eventSystem));
    printf("Event system cleaned up\n");
}

// 이벤트 큐에 이벤트 추가
static bool EnqueueEvent(const Event* event) {
    if (!eventSystem.initialized) {
        printf("Warning: Event system not initialized\n");
        return false;
    }
    
    if (eventSystem.queue.count >= MAX_EVENT_QUEUE_SIZE) {
        printf("Warning: Event queue overflow\n");
        return false;
    }
    
    eventSystem.queue.events[eventSystem.queue.tail] = *event;
    eventSystem.queue.tail = (eventSystem.queue.tail + 1) % MAX_EVENT_QUEUE_SIZE;
    eventSystem.queue.count++;
    
    return true;
}

// 이벤트 큐에서 이벤트 가져오기
static bool DequeueEvent(Event* event) {
    if (!eventSystem.initialized || eventSystem.queue.count <= 0) {
        return false;
    }
    
    *event = eventSystem.queue.events[eventSystem.queue.head];
    eventSystem.queue.head = (eventSystem.queue.head + 1) % MAX_EVENT_QUEUE_SIZE;
    eventSystem.queue.count--;
    
    return true;
}

// 이벤트 발행
void PublishEvent(EventType type, void* data) {
    if (!eventSystem.initialized) {
        printf("Warning: Event system not initialized\n");
        return;
    }
    
    if (type < 0 || type >= EVENT_COUNT) {
        printf("Warning: Invalid event type %d\n", type);
        return;
    }
    
    Event event;
    event.type = type;
    event.data = data;
    event.timestamp = GetTime();
    
    EnqueueEvent(&event);
}

// 이벤트 즉시 처리
static void DispatchEvent(const Event* event) {
    EventType type = event->type;
    
    for (int i = 0; i < eventSystem.listenerCount[type]; i++) {
        if (eventSystem.listeners[type][i].active && eventSystem.listeners[type][i].handler) {
            eventSystem.listeners[type][i].handler(event, eventSystem.listeners[type][i].context);
        }
    }
}

// 이벤트 큐 처리
void ProcessEventQueue(void) {
    if (!eventSystem.initialized) {
        return;
    }
    
    Event event;
    while (DequeueEvent(&event)) {
        DispatchEvent(&event);
    }
}

// 이벤트 구독
int SubscribeToEvent(EventType type, EventHandler handler, void* context) {
    if (!eventSystem.initialized) {
        printf("Warning: Event system not initialized\n");
        return -1;
    }
    
    if (type < 0 || type >= EVENT_COUNT) {
        printf("Warning: Invalid event type %d\n", type);
        return -1;
    }
    
    if (eventSystem.listenerCount[type] >= MAX_LISTENERS_PER_EVENT) {
        printf("Warning: Too many listeners for event type %d\n", type);
        return -1;
    }
    
    int id = eventSystem.listenerCount[type];
    eventSystem.listeners[type][id].handler = handler;
    eventSystem.listeners[type][id].context = context;
    eventSystem.listeners[type][id].active = true;
    eventSystem.listenerCount[type]++;
    
    return id;
}

// 이벤트 구독 취소
void UnsubscribeFromEvent(EventType type, int listenerId) {
    if (!eventSystem.initialized) {
        return;
    }
    
    if (type < 0 || type >= EVENT_COUNT || 
        listenerId < 0 || listenerId >= eventSystem.listenerCount[type]) {
        return;
    }
    
    eventSystem.listeners[type][listenerId].active = false;
} 