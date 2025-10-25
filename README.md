Gravity System Implementation Plan

 🎯 Goal

 범용 중력 시스템 구현 - 모든 게임 오브젝트(Particles, Enemies, Player, Items)에 적용 가능하며, 다양한 중력 발생원(BLACKHOLE, 환경 오브젝트,
  플레이어 스킬 등)을 지원

 📋 Current State Analysis

 현재 문제점:
 - 중력 코드가 game.c의 main loop에 하드코딩됨 (479-499번 줄)
 - Test mode에서 중력이 작동하지 않음
 - BLACKHOLE, REPULSOR 등이 각자 다른 방식으로 구현됨
 - 확장성 없음 (새로운 중력 타입 추가 어려움)

 현재 중력 구현:
 1. BLACKHOLE attraction (game.c:479-499) - particles만 영향
 2. REPULSOR repulsion (game.c:354-361) - particles만 영향
 3. BLACKHOLE storm (game.c:420-476) - particles만 영향

 🏗️ Architecture Desig

 1. Core Components

 신규 파일:
 - src/core/gravity_system.h - 중력 시스템 인터페이스
 - src/core/gravity_system.c - 중력 시스템 구현

 2. Data Structures

 // Gravity field type
 typedef enum {
     GRAVITY_TYPE_NONE = 0,
     GRAVITY_TYPE_ATTRACTION,  // 끌어당김
     GRAVITY_TYPE_REPULSION,   // 밀어냄
     GRAVITY_TYPE_ORBITAL,     // 궤도 (향후 확장)
     GRAVITY_TYPE_DIRECTIONAL  // 방향성 (향후 확장)
 } GravityType;

 // Gravity source (누가 중력을 발생시키는가)
 typedef struct {
     Vector2 position;      // 중력 중심점
     float radius;          // 영향 범위
     float strength;        // 중력 강도
     GravityType type;      // 중력 타입
     bool active;           // 활성화 여부
     void* sourcePtr;       // 발생원 포인터 (Enemy*, etc)
     int sourceType;        // 발생원 타입 (0=enemy, 1=environment, 2=player_skill)
 } GravitySource;

 // Gravity target (무엇이 중력을 받는가)
 typedef struct {
     Vector2* position;     // 위치 포인터
     Vector2* velocity;     // 속도 포인터
     float mass;            // 질량 (중력 영향 정도)
     bool affectedByGravity; // 중력 영향 받는지 여부
 } GravityTarget;

 3. Core Functions

 // 중력 소스 관리
 void InitGravitySystem(void);
 void CleanupGravitySystem(void);
 int RegisterGravitySource(GravitySource source);
 void UnregisterGravitySource(int sourceId);
 void UpdateGravitySource(int sourceId, GravitySource source);

 // 중력 적용 (핵심 함수)
 void ApplyGravityToTarget(GravityTarget target, float deltaTime);
 void ApplyAllGravitySources(Game* game, float deltaTime);

 // 헬퍼 함수
 Vector2 CalculateGravityForce(Vector2 targetPos, GravitySource source);
 bool IsInGravityRange(Vector2 targetPos, GravitySource source);

 📝 Implementation Phases

 Phase 1: 기본 시스템 구축 (우선순위: 높음)

 목표: 중력 시스템 기반 구조 생성

 1. src/core/gravity_system.h 생성
   - GravityType enum 정의
   - GravitySource, GravityTarget 구조체 정의
   - 함수 선언
 2. src/core/gravity_system.c 생성
   - 중력 소스 배열 관리 (최대 32개)
   - InitGravitySystem(), CleanupGravitySystem()
   - RegisterGravitySource(), UnregisterGravitySource()
   - CalculateGravityForce() - 거리 기반 중력 계산
 3. Makefile 업데이트
   - src/core/gravity_system.c 추가
   - Makefile.web에도 추가

 Phase 2: BLACKHOLE 중력 마이그레이션 (우선순위: 높음)

 목표: 기존 BLACKHOLE 코드를 새 시스템으로 전환

 1. enemy.c의 UpdateEnemy() 수정
   - BLACKHOLE이 업데이트될 때 GravitySource 등록/갱신
   - stateFlags 기반으로 active 상태 결정
 2. game.c의 하드코딩된 중력 제거
   - 479-499번 줄 BLACKHOLE 중력 삭제
   - ApplyAllGravitySources() 호출로 대체
 3. Test mode 수정
   - Test mode 업데이트 루프에 ApplyAllGravitySources() 추가
   - 이것이 Test mode에서 중력이 작동하지 않던 문제 해결

 Phase 3: Particle 통합 (우선순위: 높음)

 목표: 모든 particle이 중력 시스템 사용

 1. particle.h 확장
   - Particle 구조체에 mass 필드 추가 (기본값 1.0f)
   - affectedByGravity 플래그 추가
 2. particle_manager.c 수정
   - UpdateAllParticles()에서 각 particle을 GravityTarget으로 변환
   - ApplyGravityToTarget() 호출
 3. REPULSOR 마이그레이션
   - ENEMY_TYPE_REPULSOR도 GravitySource로 등록
   - type = GRAVITY_TYPE_REPULSION

 Phase 4: Enemy 통합 (우선순위: 중간)

 목표: Enemy들도 중력 영향 받도록

 1. enemy.h 확장
   - Enemy 구조체에 mass, affectedByGravity 추가
 2. enemy_manager.c 수정
   - UpdateAllEnemies()에서 중력 적용
   - 특정 enemy 타입만 중력 영향 받도록 설정 가능

 Phase 5: Player & Items 통합 (우선순위: 낮음)

 목표: Player와 Item도 중력 영향

 1. Player 통합
   - player.h에 중력 관련 필드 추가
   - 중력이 player 이동속도에 영향
 2. Items 통합
   - HP 포션 등이 중력에 끌리거나 밀림

 Phase 6: 환경 오브젝트 & 스킬 지원 (우선순위: 낮음)

 목표: Enemy 외 다른 중력 발생원

 1. 환경 중력 소스
   - Stage에 고정 중력 포인트 배치 가능
   - stage_common.h에 GravitySource 배열 추가
 2. 플레이어 스킬
   - 플레이어가 일시적으로 중력 발생
   - 쿨다운, 지속시간 관리

 Phase 7: 최적화 & 디버깅 (우선순위: 중간)

 목표: 성능 최적화 및 디버깅 도구

 1. 성능 최적화
   - Spatial partitioning (격자 기반 최적화)
   - 거리 기반 조기 컬링
   - 배치 처리
 2. Test mode 시각화
   - G 키로 중력장 시각화 토글
   - 중력 소스를 원으로 표시
   - 영향 범위를 반투명 원으로 표시
 3. 디버그 정보
   - DrawEnemyStateDebug에 중력 정보 추가
   - 활성 중력 소스 개수, 강도 표시

 🎨 Benefits

 확장성:
 - 새로운 중력 타입 쉽게 추가 (GRAVITY_TYPE_ORBITAL 등)
 - 새로운 발생원 쉽게 등록 (환경 오브젝트, 스킬 등)
 - 새로운 타겟 타입 쉽게 통합

 성능:
 - 한 곳에서 모든 중력 계산 관리
 - 최적화 적용 용이 (spatial partitioning 등)

 유지보수:
 - 중력 관련 버그를 한 곳에서 수정
 - Test mode에서 모든 중력 효과 자동 작동
 - 명확한 책임 분리 (gravity_system이 모든 중력 담당)

 디버깅:
 - 중력 시각화로 문제 쉽게 파악
 - Test mode에서 I/S/P 키 + G키 조합으로 완벽한 테스트

 📦 Deliverables

 1. 새 파일:
   - src/core/gravity_system.h (~150 lines)
   - src/core/gravity_system.c (~400 lines)
 2. 수정 파일:
   - src/core/game.c - 중력 코드 제거, ApplyAllGravitySources 호출 추가
   - src/entities/enemy.c - BLACKHOLE/REPULSOR를 GravitySource로 등록
   - src/entities/particle.h - mass, affectedByGravity 추가
   - src/entities/managers/particle_manager.c - 중력 적용 통합
   - Makefile, Makefile.web - gravity_system.c 추가
   - PLANNING.md - Gravity System 섹션 추가
 3. 문서:
   - PLANNING.md에 "Gravity System" 섹션 추가
   - 중력 시스템 사용 예제 및 가이드

 ⚠️ Risks & Mitigation

 위험: 성능 저하 (100,000 particles * 최대 32 중력 소스)
 완화: Phase 1부터 거리 기반 조기 컬링, 배치 처리 적용

 위험: 기존 게임플레이 변경
 완화: 기존 BLACKHOLE/REPULSOR 동작 정확히 재현, 충분한 테스트

 위험: 복잡도 증가
 완화: 명확한 API 설계, 충분한 주석, 예제 코드

