 Enemy State System Refactoring: Hybrid Approach (stateFlags + stateData)

 목표

 현재 흩어져 있는 enemy 상태 필드들을 통합된 상태 시스템으로 리팩토링하여:
 1. 일관된 상태 관리
 2. 테스트 모드에서 상태 자동 확인 가능
 3. 디버깅 용이성 향상
 4. 새 enemy 추가 시 확장성

 현재 문제점 분석

 현재 Enemy 구조체의 상태 필드들:
 // Boolean 상태 (비트플래그로 전환)
 bool hasShield;
 bool isInvulnerable;
 bool hasPulsed;

 // 숫자 상태 (stateData로 이동)
 int phase;
 int splitCount;
 float shieldHealth;
 float phaseTimer;
 float transformTimer;
 float stormCycleTimer;

 사용 빈도: 27곳에서 직접 접근 (enemy.c 24곳, enemy_manager.c 3곳)

 구현 계획

 Phase 1: 새로운 상태 시스템 정의 (새 파일 생성)

 1.1 src/entities/enemy_state.h 생성
 // 비트플래그 enum (boolean 상태만)
 typedef enum {
     ENEMY_STATE_NONE          = 0,
     ENEMY_STATE_INVULNERABLE  = 1 << 0,  // isInvulnerable
     ENEMY_STATE_SHIELDED      = 1 << 1,  // hasShield
     ENEMY_STATE_PULSED        = 1 << 2,  // hasPulsed
     ENEMY_STATE_TELEPORTING   = 1 << 3,  // future use
     ENEMY_STATE_STORM_ACTIVE  = 1 << 4,  // blackhole storm cycle
     ENEMY_STATE_SPLITTING     = 1 << 5,  // splitter active split
     ENEMY_STATE_PHASE_CHANGE  = 1 << 6,  // boss phase transition
 } EnemyStateFlags;

 // 숫자 상태 데이터
 typedef struct {
     int phase;              // Boss phase (0, 1, 2)
     int splitCount;         // Splitter splits remaining (0~3)
     float shieldHealth;     // Shield HP (0.0~max)
     float phaseTimer;       // Phase transition timer
     float transformTimer;   // Blackhole transformation timer
     float stormCycleTimer;  // Blackhole storm cycle timer (0~10s)
 } EnemyStateData;

 // Helper functions (inline for performance)
 static inline bool HasState(uint32_t flags, EnemyStateFlags state) {
     return (flags & state) != 0;
 }

 static inline void SetState(uint32_t* flags, EnemyStateFlags state) {
     *flags |= state;
 }

 static inline void ClearState(uint32_t* flags, EnemyStateFlags state) {
     *flags &= ~state;
 }

 static inline void ToggleState(uint32_t* flags, EnemyStateFlags state) {
     *flags ^= state;
 }

 // State name for debugging
 const char* GetStateName(EnemyStateFlags state);

 1.2 src/entities/enemy_state.c 생성
 - GetStateName() 구현
 - State validation 함수들

 Phase 2: Enemy 구조체 수정

 2.1 src/entities/enemy.h 수정
 #include "enemy_state.h"

 typedef struct Enemy {
     // ... 기존 필드들 ...

     // NEW: 통합 상태 시스템
     uint32_t stateFlags;      // 비트플래그
     EnemyStateData stateData; // 숫자 데이터

     // DEPRECATED (제거 예정, 하위 호환을 위해 일단 주석)
     // bool hasShield;         → stateFlags & ENEMY_STATE_SHIELDED
     // bool isInvulnerable;    → stateFlags & ENEMY_STATE_INVULNERABLE
     // bool hasPulsed;         → stateFlags & ENEMY_STATE_PULSED
     // int phase;              → stateData.phase
     // int splitCount;         → stateData.splitCount
     // float shieldHealth;     → stateData.shieldHealth
     // float phaseTimer;       → stateData.phaseTimer
     // float transformTimer;   → stateData.transformTimer
     // float stormCycleTimer;  → stateData.stormCycleTimer
 } Enemy;

 Phase 3: 초기화 코드 수정

 3.1 src/entities/enemy.c - InitEnemyByType() 수정
 - 기존 bool 필드 → stateFlags 설정으로 변경
 - 기존 숫자 필드 → stateData 초기화로 변경

 예시:
 // Before
 enemy.isInvulnerable = true;
 enemy.hasPulsed = false;
 enemy.phase = 0;

 // After
 enemy.stateFlags = ENEMY_STATE_INVULNERABLE;
 enemy.stateData.phase = 0;

 Phase 4: 사용처 전환 (27곳)

 4.1 src/entities/enemy.c (24곳)
 4.2 src/entities/managers/enemy_manager.c (3곳)
 4.3 src/core/game.c (BLACKHOLE transformation)

 전환 패턴:
 // Boolean 접근
 if (enemy->isInvulnerable)           → if (HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE))
 enemy->hasShield = true;             → SetState(&enemy->stateFlags, ENEMY_STATE_SHIELDED)
 enemy->isInvulnerable = false;       → ClearState(&enemy->stateFlags, ENEMY_STATE_INVULNERABLE)

 // 숫자 접근
 enemy->phase = 2;                    → enemy->stateData.phase = 2
 if (enemy->phase >= 2)               → if (enemy->stateData.phase >= 2)
 enemy->transformTimer += deltaTime;  → enemy->stateData.transformTimer += deltaTime

 Phase 5: 테스트 모드 통합

 5.1 src/core/dev_test_mode.c - 상태 표시 UI 추가
 void DrawEnemyStateDebug(Enemy* enemy, Vector2 position) {
     // Flags 비트맵 표시
     DrawText(TextFormat("Flags: 0x%02X", enemy->stateFlags), ...);

     // 활성 상태 목록
     int yOffset = 0;
     if (HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE)) {
         DrawText("[INVULNERABLE]", position.x, position.y + yOffset, 10, YELLOW);
         yOffset += 12;
     }
     if (HasState(enemy->stateFlags, ENEMY_STATE_PULSED)) {
         DrawText("[PULSED]", position.x, position.y + yOffset, 10, GREEN);
         yOffset += 12;
     }
     // ... 다른 상태들

     // 숫자 데이터
     if (enemy->stateData.phase > 0) {
         DrawText(TextFormat("Phase: %d", enemy->stateData.phase), ...);
     }
     if (enemy->stateData.transformTimer > 0) {
         DrawText(TextFormat("Transform: %.1fs", enemy->stateData.transformTimer), ...);
     }
 }

 5.2 테스트 모드 키보드 단축키 추가
 - I 키: Toggle INVULNERABLE (선택된 적)
 - S 키: Toggle SHIELD
 - P 키: Phase +1 (Boss)
 - H 키: Set Health to 70%, 30%, 10% (순환)

 Phase 6: 검증 및 정리

 6.1 컴파일 검증
 - Desktop build (make clean && make)
 - Web build (make -f Makefile.web clean && make -f Makefile.web)

 6.2 기능 테스트
 - 각 enemy 타입별 상태 전환 확인
 - BLACKHOLE transformation 동작 확인
 - BOSS phase change 확인
 - SPLITTER split 확인

 6.3 구조체 정리
 - 주석 처리된 deprecated 필드 완전 제거
 - 메모리 크기 확인 (Enemy 구조체 증가량 < 50 bytes)

 6.4 문서 업데이트
 - PLANNING.md에 새로운 상태 시스템 문서화
 - enemy_state.h에 각 상태의 용도 주석

 예상 변경 파일 목록

 새로 생성 (2개):
 - src/entities/enemy_state.h (~80 lines)
 - src/entities/enemy_state.c (~50 lines)

 수정 (5개):
 - src/entities/enemy.h (+3 lines, ~8 lines deprecated)
 - src/entities/enemy.c (~30 substitutions)
 - src/entities/managers/enemy_manager.c (~5 substitutions)
 - src/core/game.c (~3 substitutions for BLACKHOLE)
 - src/core/dev_test_mode.c (+60 lines for state UI)

 빌드 시스템 (2개):
 - Makefile (+1 source file)
 - Makefile.web (+1 source file)

 예상 소요 시간

 - Phase 1-2: 파일 생성 및 구조체 정의 (10분)
 - Phase 3-4: 사용처 전환 (20분, 27곳)
 - Phase 5: 테스트 모드 UI (15분)
 - Phase 6: 검증 및 문서화 (15분)
 - 총 예상 시간: 60분

 리스크 및 대응

 리스크 1: 기존 코드가 깨질 수 있음
 - 대응: Phase별로 컴파일 확인, 단계별 커밋

 리스크 2: 성능 저하 (함수 호출 오버헤드)
 - 대응: Helper 함수를 inline으로 선언 → 컴파일러가 최적화

 리스크 3: 메모리 증가
 - 대응: Enemy 구조체 크기 측정, 30 bytes 이하 증가 목표

 진행 방식

 순차적으로 Phase 1 → 6까지 진행하며:
 1. 각 Phase 완료 후 컴파일 확인
 2. Phase 4 완료 후 중간 테스트
 3. Phase 6 완료 후 전체 검증
