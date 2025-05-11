#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "game.h"

// 입력 처리 초기화 (이벤트 리스너 등록)
void InitInputHandler(Game* game);

// 입력 상태 업데이트 및 이벤트 발행
void ProcessInputEvents(void);

#endif // INPUT_HANDLER_H 