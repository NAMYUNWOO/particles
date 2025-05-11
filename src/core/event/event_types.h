#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

#include "raylib.h"

// 키보드 입력 이벤트 데이터
typedef struct {
    int keyCode;      // 키 코드 (KEY_A, KEY_SPACE 등)
    bool isPressed;   // 눌림 여부 (true: 눌림, false: 뗌)
} KeyEventData;

// 향후 다른 이벤트 데이터 구조체는 여기에 추가

#endif // EVENT_TYPES_H 