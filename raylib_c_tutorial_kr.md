# Raylib과 함께 배우는 C 프로그래밍

## 📚 목차
1. [소개](#1-소개)
2. [개발 환경 설정](#2-개발-환경-설정)
3. [Raylib 기초](#3-raylib-기초)
4. [C 언어 핵심 개념](#4-c-언어-핵심-개념)
5. [파티클 스톰 프로젝트 구조](#5-파티클-스톰-프로젝트-구조)
6. [메모리 관리](#6-메모리-관리)
7. [이벤트 시스템](#7-이벤트-시스템)
8. [게임 물리학](#8-게임-물리학)
9. [실전 예제](#9-실전-예제)
10. [프로젝트 빌드 및 테스트](#10-프로젝트-빌드-및-테스트)

---

## 1. 소개

### Raylib이란?
Raylib은 게임 개발을 위한 간단하고 사용하기 쉬운 C 프로그래밍 라이브러리입니다. OpenGL을 기반으로 하며, 2D/3D 그래픽, 사운드, 입력 처리 등 게임 개발에 필요한 모든 기능을 제공합니다.

### 이 프로젝트에 대해
**Particle Storm**은 100,000개의 파티클을 실시간으로 시뮬레이션하는 물리 기반 게임입니다. 이 프로젝트를 통해 다음을 배울 수 있습니다:
- C 언어의 핵심 개념
- Raylib 라이브러리 사용법
- 효율적인 메모리 관리
- 이벤트 기반 아키텍처
- 게임 물리학 구현

---

## 2. 개발 환경 설정

### macOS에서 Raylib 설치
```bash
# Homebrew를 통한 설치
brew install raylib

# 설치 경로 확인
ls /opt/homebrew/Cellar/raylib/
```

### 프로젝트 빌드
```bash
# 프로젝트 클린 빌드
make clean
make

# 게임 실행
make run

# 한 번에 빌드와 실행
make clean && make && make run
```

---

## 3. Raylib 기초

### 3.1 기본 윈도우 생성

```c
#include "raylib.h"

int main(void)
{
    // 화면 크기 설정
    const int screenWidth = 800;
    const int screenHeight = 450;

    // 윈도우 초기화
    InitWindow(screenWidth, screenHeight, "나의 첫 Raylib 프로그램");

    // FPS 설정 (초당 프레임 수)
    SetTargetFPS(60);

    // 메인 게임 루프
    while (!WindowShouldClose())    // ESC 키나 닫기 버튼 감지
    {
        // 업데이트 로직
        // 여기에 게임 로직을 작성합니다

        // 그리기 시작
        BeginDrawing();
            ClearBackground(RAYWHITE);  // 화면을 흰색으로 지우기
            
            DrawText("안녕하세요, Raylib!", 190, 200, 20, LIGHTGRAY);
            DrawFPS(10, 10);  // FPS 표시

        EndDrawing();
    }

    // 윈도우 종료
    CloseWindow();

    return 0;
}
```

### 3.2 기본 도형 그리기

```c
// 원 그리기
DrawCircle(x좌표, y좌표, 반지름, 색상);
DrawCircle(50, 100, 30, RED);

// 사각형 그리기
DrawRectangle(x좌표, y좌표, 너비, 높이, 색상);
DrawRectangle(100, 80, 60, 60, GREEN);

// 삼각형 그리기
DrawTriangle(점1, 점2, 점3, 색상);
DrawTriangle((Vector2){200, 100}, (Vector2){250, 150}, (Vector2){300, 100}, BLUE);

// 선 그리기
DrawLine(시작x, 시작y, 끝x, 끝y, 색상);
DrawLine(50, 200, 300, 200, RED);
```

### 3.3 입력 처리

```c
// 키보드 입력
if (IsKeyPressed(KEY_SPACE)) {
    // 스페이스바를 눌렀을 때
}

if (IsKeyDown(KEY_RIGHT)) {
    // 오른쪽 화살표를 누르고 있는 동안
    playerX += 5;
}

// 마우스 입력
Vector2 mousePos = GetMousePosition();
if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    // 마우스 왼쪽 버튼 클릭
}
```

---

## 4. C 언어 핵심 개념

### 4.1 구조체 (Structures)

구조체는 관련된 데이터를 하나의 단위로 묶는 C 언어의 핵심 기능입니다:

```c
// 기본 구조체 정의
struct Point {
    int x;
    int y;
};

// typedef를 사용한 구조체 정의 (권장)
typedef struct {
    Vector2 position;   // 위치 (x, y)
    Vector2 velocity;   // 속도 (x, y)
    Color color;        // 색상
} Particle;

// 구조체 사용 예제
Particle particle;
particle.position.x = 100;
particle.position.y = 200;
particle.velocity = (Vector2){1.0f, 0.5f};
particle.color = BLACK;

// 구조체 포인터 사용
Particle* pParticle = &particle;
pParticle->position.x = 150;  // 화살표 연산자 사용
(*pParticle).position.y = 250;  // 역참조 후 점 연산자 사용 (같은 효과)

// 구조체 배열
Particle particles[100];
for (int i = 0; i < 100; i++) {
    particles[i].position = (Vector2){i * 10.0f, 0};
}
```

### 4.2 포인터와 동적 메모리

포인터는 메모리 주소를 저장하는 변수입니다:

```c
// 포인터 기초
int num = 42;
int* ptr = &num;  // num의 주소를 ptr에 저장
printf("값: %d, 주소: %p\n", *ptr, ptr);

// 포인터 연산
int arr[5] = {10, 20, 30, 40, 50};
int* p = arr;  // 배열 이름은 첫 번째 요소의 포인터
printf("첫 번째: %d\n", *p);      // 10
printf("두 번째: %d\n", *(p+1));  // 20
p++;  // 다음 요소로 이동
printf("현재 값: %d\n", *p);      // 20

// 동적 메모리 할당
Particle* particles = (Particle*)malloc(100 * sizeof(Particle));
if (particles == NULL) {
    printf("메모리 할당 실패!\n");
    return -1;
}

// 메모리 사용
for (int i = 0; i < 100; i++) {
    particles[i].position.x = GetRandomValue(0, 800);
    particles[i].position.y = GetRandomValue(0, 600);
}

// 메모리 재할당
particles = (Particle*)realloc(particles, 200 * sizeof(Particle));

// 메모리 해제 (중요!)
free(particles);
particles = NULL;  // 댕글링 포인터 방지
```

### 4.3 함수 포인터 (상세 설명)

함수 포인터는 함수의 메모리 주소를 저장하는 포인터입니다. 이를 통해 함수를 변수처럼 다룰 수 있습니다:

#### 4.3.1 함수 포인터 기초

```c
// 일반 함수 정의
int Add(int a, int b) {
    return a + b;
}

int Subtract(int a, int b) {
    return a - b;
}

// 함수 포인터 선언과 사용
int (*operation)(int, int);  // 함수 포인터 선언
operation = Add;              // 함수 주소 할당
int result = operation(5, 3); // 함수 포인터로 호출 (결과: 8)

operation = Subtract;          // 다른 함수 할당
result = operation(5, 3);      // 결과: 2

// 함수 포인터 배열
int (*operations[2])(int, int) = {Add, Subtract};
printf("덧셈: %d\n", operations[0](10, 5));  // 15
printf("뺄셈: %d\n", operations[1](10, 5));  // 5
```

#### 4.3.2 typedef를 사용한 함수 포인터

```c
// typedef로 함수 포인터 타입 정의 (읽기 쉬움)
typedef int (*MathOperation)(int, int);

// 사용이 훨씬 간단해짐
MathOperation op = Add;
int result = op(10, 20);  // 30

// 함수 포인터를 매개변수로 받는 함수
int Calculate(int a, int b, MathOperation operation) {
    return operation(a, b);
}

// 사용 예제
int sum = Calculate(5, 3, Add);       // 8
int diff = Calculate(5, 3, Subtract); // 2
```

#### 4.3.3 콜백 함수 구현

```c
// 버튼 클릭 콜백 예제
typedef void (*ButtonCallback)(int buttonId);

typedef struct {
    int id;
    char name[32];
    ButtonCallback onClick;  // 콜백 함수 포인터
} Button;

// 콜백 함수들
void OnPlayButtonClick(int id) {
    printf("게임 시작! (버튼 ID: %d)\n", id);
}

void OnQuitButtonClick(int id) {
    printf("게임 종료! (버튼 ID: %d)\n", id);
}

// 버튼 생성과 사용
Button playButton = {1, "Play", OnPlayButtonClick};
Button quitButton = {2, "Quit", OnQuitButtonClick};

// 버튼 클릭 시뮬레이션
playButton.onClick(playButton.id);  // "게임 시작! (버튼 ID: 1)"
quitButton.onClick(quitButton.id);  // "게임 종료! (버튼 ID: 2)"
```

#### 4.3.4 이벤트 시스템에서의 함수 포인터

```c
// 이벤트 타입 정의
typedef enum {
    EVENT_ENEMY_DESTROYED,
    EVENT_PLAYER_HIT,
    EVENT_STAGE_COMPLETED,
    EVENT_COUNT  // 총 이벤트 개수
} EventType;

// 이벤트 구조체
typedef struct {
    EventType type;
    void* data;
    double timestamp;
} Event;

// 이벤트 핸들러 타입 정의
typedef void (*EventHandler)(const Event* event, void* context);

// 이벤트 리스너 구조체
typedef struct {
    EventHandler handler;
    void* context;
} EventListener;

// 이벤트 시스템 (이벤트 타입별로 리스너 관리)
#define MAX_LISTENERS_PER_EVENT 10

typedef struct {
    EventListener listeners[EVENT_COUNT][MAX_LISTENERS_PER_EVENT];
    int listenerCount[EVENT_COUNT];
} EventSystem;

// 이벤트 데이터 구조체
typedef struct {
    Vector2 position;
    int scoreValue;
    int enemyType;
} EnemyDestroyedData;

// 이벤트 핸들러 함수들
void OnEnemyDestroyed(const Event* event, void* context) {
    Game* game = (Game*)context;
    EnemyDestroyedData* data = (EnemyDestroyedData*)event->data;
    
    game->score += data->scoreValue;
    printf("적 파괴! 위치: (%.1f, %.1f), 점수: %d\n", 
           data->position.x, data->position.y, game->score);
}

void OnPlayerHit(const Event* event, void* context) {
    Game* game = (Game*)context;
    int* damage = (int*)event->data;
    
    game->player.health -= *damage;
    printf("플레이어 피격! 데미지: %d, 남은 체력: %d\n", 
           *damage, game->player.health);
}

void OnEnemyDestroyedEffect(const Event* event, void* context) {
    EnemyDestroyedData* data = (EnemyDestroyedData*)event->data;
    printf("폭발 효과 생성: 위치 (%.1f, %.1f)\n", 
           data->position.x, data->position.y);
    // CreateExplosion(data->position);
}

// 이벤트 구독 (특정 이벤트 타입에 대해서만)
void SubscribeToEvent(EventSystem* system, EventType type, 
                     EventHandler handler, void* context) {
    if (type >= EVENT_COUNT) return;
    
    int count = system->listenerCount[type];
    if (count >= MAX_LISTENERS_PER_EVENT) return;
    
    system->listeners[type][count].handler = handler;
    system->listeners[type][count].context = context;
    system->listenerCount[type]++;
}

// 이벤트 발행 (해당 타입의 리스너만 호출)
void PublishEvent(EventSystem* system, EventType type, void* data) {
    if (type >= EVENT_COUNT) return;
    
    Event event = {
        .type = type,
        .data = data,
        .timestamp = GetTime()  // raylib의 GetTime() 함수
    };
    
    // 해당 이벤트 타입에 구독한 리스너만 호출
    for (int i = 0; i < system->listenerCount[type]; i++) {
        system->listeners[type][i].handler(&event, 
                                          system->listeners[type][i].context);
    }
}

// 사용 예제
EventSystem eventSystem = {0};
Game game = {0};
game.score = 0;
game.player.health = 100;

// EVENT_ENEMY_DESTROYED에 여러 핸들러 구독
SubscribeToEvent(&eventSystem, EVENT_ENEMY_DESTROYED, OnEnemyDestroyed, &game);
SubscribeToEvent(&eventSystem, EVENT_ENEMY_DESTROYED, OnEnemyDestroyedEffect, NULL);

// EVENT_PLAYER_HIT에 핸들러 구독
SubscribeToEvent(&eventSystem, EVENT_PLAYER_HIT, OnPlayerHit, &game);

// 적 파괴 이벤트 발행
EnemyDestroyedData enemyData = {
    .position = {100.0f, 200.0f},
    .scoreValue = 150,
    .enemyType = 1
};
PublishEvent(&eventSystem, EVENT_ENEMY_DESTROYED, &enemyData);
// 출력: "적 파괴! 위치: (100.0, 200.0), 점수: 150"
// 출력: "폭발 효과 생성: 위치 (100.0, 200.0)"

// 플레이어 피격 이벤트 발행
int damage = 25;
PublishEvent(&eventSystem, EVENT_PLAYER_HIT, &damage);
// 출력: "플레이어 피격! 데미지: 25, 남은 체력: 75"
// OnPlayerHit만 호출됨 (EVENT_PLAYER_HIT 타입이므로)
```

#### 4.3.5 함수 포인터 고급 활용

##### 전방 선언과 순환 참조 해결

함수 포인터와 구조체가 서로를 참조하는 경우, **전방 선언(forward declaration)**을 사용해야 합니다:

```c
// 순환 참조 문제 상황:
// - StateFunction은 GameStateMachine*를 매개변수로 사용
// - GameStateMachine은 StateFunction을 멤버로 포함
// - 서로가 서로를 참조!

// ❌ 잘못된 방법 1: GameStateMachine이 아직 정의되지 않음
typedef void (*StateFunction)(GameStateMachine* fsm);  // 컴파일 에러!
struct GameStateMachine {
    StateFunction currentState;
};

// ❌ 잘못된 방법 2: StateFunction이 아직 정의되지 않음
struct GameStateMachine {
    StateFunction currentState;  // 컴파일 에러!
    int health;
};
typedef void (*StateFunction)(GameStateMachine* fsm);

// ✅ 올바른 방법: 전방 선언 사용
// 1단계: 구조체 타입 이름만 먼저 선언
typedef struct GameStateMachine GameStateMachine;

// 2단계: 이제 GameStateMachine*를 사용할 수 있음
typedef void (*StateFunction)(GameStateMachine* fsm);

// 3단계: 실제 구조체 정의 (StateFunction 사용 가능)
struct GameStateMachine {
    StateFunction currentState;
    int health;
    int score;
};
```

**전방 선언이 작동하는 이유:**
- 컴파일러에게 "GameStateMachine이라는 구조체 타입이 있을 것"이라고 미리 알려줌
- 포인터는 크기가 고정(64비트 시스템에서 8바이트)이므로 구조체의 세부 내용을 몰라도 컴파일 가능
- 실제 구조체 내용은 나중에 정의해도 됨

##### 상태 머신 구현 예제

```c
// 상태 머신 구현
typedef struct GameStateMachine GameStateMachine;

typedef void (*StateFunction)(GameStateMachine* fsm);

struct GameStateMachine {
    StateFunction currentState;
    int health;
    int score;
};

// 상태 함수들 (전방 선언)
void MenuState(GameStateMachine* fsm);
void PlayingState(GameStateMachine* fsm);
void GameOverState(GameStateMachine* fsm);

// 상태 함수 구현
void MenuState(GameStateMachine* fsm) {
    printf("메뉴 상태\n");
    if (IsKeyPressed(KEY_ENTER)) {
        fsm->currentState = PlayingState;
    }
}

void PlayingState(GameStateMachine* fsm) {
    printf("플레이 중 (체력: %d, 점수: %d)\n", fsm->health, fsm->score);
    if (fsm->health <= 0) {
        fsm->currentState = GameOverState;
    }
}

void GameOverState(GameStateMachine* fsm) {
    printf("게임 오버! 최종 점수: %d\n", fsm->score);
    if (IsKeyPressed(KEY_R)) {
        fsm->health = 100;
        fsm->score = 0;
        fsm->currentState = MenuState;
    }
}

// 상태 머신 사용
GameStateMachine fsm = {MenuState, 100, 0};
while (gameRunning) {
    fsm.currentState(&fsm);  // 현재 상태 실행
}
```

### 4.4 열거형 (Enums)

열거형은 관련된 상수들을 그룹화하는 방법입니다:

```c
// 기본 열거형
enum Color {
    RED,    // 0
    GREEN,  // 1
    BLUE    // 2
};

// typedef를 사용한 열거형 (권장)
typedef enum {
    GAME_STATE_TUTORIAL,    // 0
    GAME_STATE_STAGE_INTRO, // 1
    GAME_STATE_PLAYING,     // 2
    GAME_STATE_OVER,        // 3
    GAME_STATE_SCORE_ENTRY  // 4
} GameState;

// 값을 지정한 열거형
typedef enum {
    ENEMY_BASIC = 100,
    ENEMY_TRACKER = 150,
    ENEMY_BOSS = 1000
} EnemyScoreValue;

// 비트 플래그로 사용
typedef enum {
    FLAG_NONE = 0,
    FLAG_INVINCIBLE = 1 << 0,  // 1
    FLAG_SPEED_BOOST = 1 << 1, // 2
    FLAG_DOUBLE_DAMAGE = 1 << 2 // 4
} PlayerFlags;

// 사용 예제
GameState currentState = GAME_STATE_TUTORIAL;
if (currentState == GAME_STATE_PLAYING) {
    // 게임 플레이 로직
}

// 비트 플래그 사용
int playerFlags = FLAG_INVINCIBLE | FLAG_SPEED_BOOST;
if (playerFlags & FLAG_INVINCIBLE) {
    printf("플레이어는 무적 상태입니다\n");
}
```

### 4.5 전처리기 지시문 (Preprocessor Directives)

```c
// 매크로 정의
#define PARTICLE_COUNT 100000
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// 조건부 컴파일
#ifdef DEBUG
    #define LOG(msg) printf("[DEBUG] %s\n", msg)
#else
    #define LOG(msg) // 릴리즈에서는 아무것도 하지 않음
#endif

// Include 가드
#ifndef PARTICLE_H
#define PARTICLE_H
    // 헤더 파일 내용
#endif

// 매크로 함수 (인라인 대체)
#define SQUARE(x) ((x) * (x))
#define DISTANCE(x1, y1, x2, y2) \
    sqrt(SQUARE((x2) - (x1)) + SQUARE((y2) - (y1)))
```

### 4.6 타입 캐스팅과 void 포인터

#### void* 포인터의 정체

`void*`는 **"타입이 지정되지 않은 메모리 주소"**를 가리키는 포인터입니다. 모든 포인터는 결국 메모리 주소(64비트 시스템에서 8바이트 숫자)인데, `void*`는 그 주소가 가리키는 데이터의 타입 정보가 없는 순수한 주소값입니다.

```c
// 메모리 관점에서 이해하기
// 메모리는 단순히 바이트들의 나열입니다
// 주소: 0x1000  0x1001  0x1002  0x1003  0x1004 ...
// 값:   [42]    [00]    [00]    [00]    [3.14의 바이트들...]

int num = 42;           // 0x1000에 4바이트 정수 저장
float fnum = 3.14f;     // 0x1004에 4바이트 실수 저장

// 모든 포인터는 단순히 주소값입니다
int* p1 = &num;         // p1 = 0x1000 (int로 해석하라는 타입 정보 포함)
float* p2 = &fnum;      // p2 = 0x1004 (float로 해석하라는 타입 정보 포함)
void* p3 = &num;        // p3 = 0x1000 (타입 정보 없음!)

// void*는 타입 정보가 없어서 직접 역참조 불가능
// *p3 = 10;  // ❌ 컴파일 에러! 몇 바이트를 읽어야 할지 모름

// 타입 캐스팅으로 "이 주소의 데이터를 int로 해석해"라고 알려줌
int* p4 = (int*)p3;    // p3의 주소값을 int* 타입으로 해석
*p4 = 100;              // ✅ 이제 4바이트를 int로 읽기/쓰기 가능
```

#### 왜 void*를 사용하는가?

```c
// 1. 제네릭 프로그래밍 - 타입에 상관없이 동작하는 함수
void* my_memcpy(void* dest, const void* src, size_t n) {
    // void*를 char*로 변환해 1바이트씩 복사
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];  // 1바이트씩 복사
    }
    return dest;
}

// 어떤 타입이든 복사 가능
int arr1[5] = {1, 2, 3, 4, 5};
int arr2[5];
my_memcpy(arr2, arr1, sizeof(arr1));  // int 배열 복사

float farr1[3] = {1.1f, 2.2f, 3.3f};
float farr2[3];
my_memcpy(farr2, farr1, sizeof(farr1));  // float 배열 복사

// 2. 콜백 함수의 context 전달
typedef struct {
    int score;
    int level;
} GameData;

void OnButtonClick(void* context) {
    GameData* game = (GameData*)context;  // void*를 실제 타입으로 캐스팅
    game->score += 100;
    printf("점수: %d, 레벨: %d\n", game->score, game->level);
}

// 3. malloc의 반환값
void* memory = malloc(100);  // malloc은 void*를 반환
int* intArray = (int*)memory;  // 원하는 타입으로 캐스팅해서 사용
```

#### void*와 함수 포인터의 조합

```c
// 이벤트 시스템에서 void* 활용 예제
typedef struct {
    EventType type;
    void* data;  // 어떤 타입의 데이터든 가리킬 수 있음
    double timestamp;
} Event;

// 콜백 함수도 void* context를 받음
typedef void (*EventHandler)(const Event* event, void* context);

// 다양한 타입의 이벤트 데이터
typedef struct {
    Vector2 position;
    int damage;
} CollisionData;

typedef struct {
    int scoreValue;
    int enemyId;
} ScoreData;

// 이벤트 핸들러에서 void*를 실제 타입으로 캐스팅
void OnCollision(const Event* event, void* context) {
    Game* game = (Game*)context;  // context를 Game*로 캐스팅
    CollisionData* data = (CollisionData*)event->data;  // event->data를 CollisionData*로 캐스팅
    
    game->player.health -= data->damage;
    CreateEffect(data->position);
}

void OnScoreUpdate(const Event* event, void* context) {
    Game* game = (Game*)context;
    ScoreData* data = (ScoreData*)event->data;  // 다른 타입으로 캐스팅
    
    game->score += data->scoreValue;
}

// 사용 예제
Game gameInstance = {100, 0};  // health=100, score=0
CollisionData collision = {{100, 200}, 25};
ScoreData score = {500, 42};

Event collisionEvent = {EVENT_COLLISION, &collision, GetTime()};
Event scoreEvent = {EVENT_SCORE, &score, GetTime()};

OnCollision(&collisionEvent, &gameInstance);  // void*로 전달
OnScoreUpdate(&scoreEvent, &gameInstance);     // 같은 Game 인스턴스를 void*로
```

#### qsort 함수의 void* 활용

```c
// C 표준 라이브러리의 qsort 함수
// void qsort(void* base, size_t num, size_t size, 
//           int (*compare)(const void*, const void*));

// 비교 함수도 void*를 받아서 처리
int compare_ints(const void* a, const void* b) {
    // void*를 int*로 캐스팅 후 역참조
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return int_a - int_b;
}

int compare_floats(const void* a, const void* b) {
    // void*를 float*로 캐스팅
    float float_a = *(const float*)a;
    float float_b = *(const float*)b;
    if (float_a < float_b) return -1;
    if (float_a > float_b) return 1;
    return 0;
}

// 사용 예제
int numbers[] = {5, 2, 8, 1, 9};
qsort(numbers, 5, sizeof(int), compare_ints);  // int 배열 정렬

float prices[] = {3.14f, 1.41f, 2.71f};
qsort(prices, 3, sizeof(float), compare_floats);  // float 배열 정렬
```

#### void* 사용 시 주의사항

```c
// ⚠️ 위험한 캐스팅
void dangerous_example() {
    int num = 1000000;
    void* ptr = &num;
    
    // 잘못된 타입으로 해석 - 위험!
    float* floatPtr = (float*)ptr;
    float value = *floatPtr;  // int 비트를 float로 해석 (엉뚱한 값)
    
    // 더 위험한 예
    char* charPtr = (char*)ptr;
    *charPtr = 255;  // int의 첫 바이트만 변경 (데이터 손상)
}

// ✅ 안전한 사용 패턴
typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_STRING } DataType;

typedef struct {
    DataType type;  // 타입 정보 저장
    void* data;
} SafeData;

void ProcessData(SafeData* safeData) {
    switch (safeData->type) {
        case TYPE_INT:
            printf("정수: %d\n", *(int*)safeData->data);
            break;
        case TYPE_FLOAT:
            printf("실수: %f\n", *(float*)safeData->data);
            break;
        case TYPE_STRING:
            printf("문자열: %s\n", (char*)safeData->data);
            break;
    }
}
```

#### 핵심 정리

1. **void*는 타입 정보가 없는 순수 메모리 주소**
2. **제네릭 프로그래밍을 가능하게 함** (qsort, memcpy 등)
3. **함수 포인터와 조합하여 유연한 콜백 시스템 구현**
4. **반드시 올바른 타입으로 캐스팅 후 사용**
5. **타입 안전성을 포기하므로 신중하게 사용**

---

## 5. 파티클 스톰 프로젝트 구조

### 5.1 프로젝트 디렉토리 구조

```
src/
├── main.c                    # 프로그램 진입점
├── core/                     # 핵심 시스템
│   ├── game.c/h             # 게임 상태 관리
│   ├── physics.c/h          # 물리 시뮬레이션
│   ├── input_handler.c/h    # 입력 처리
│   ├── memory_pool.c/h      # 메모리 풀 시스템
│   └── event/               # 이벤트 시스템
│       ├── event_system.c/h
│       └── event_types.h
└── entities/                 # 게임 엔티티
    ├── player.c/h           # 플레이어
    ├── enemy.c/h            # 적
    ├── particle.c/h         # 파티클
    └── managers/            # 매니저 시스템
        ├── enemy_manager.c/h
        ├── particle_manager.c/h
        └── stage_manager.c/h
```

### 5.2 주요 게임 루프

```c
// main.c의 게임 루프 구조
int main(void)
{
    // 초기화
    InitWindow(800, 800, "Particle Storm");
    SetTargetFPS(60);
    
    InitEventSystem();        // 이벤트 시스템 초기화
    InitStageManager();       // 스테이지 매니저 초기화
    Game game = InitGame(800, 800);  // 게임 초기화
    
    // 메인 루프
    while (!WindowShouldClose())
    {
        // 프레임 시작 이벤트
        PublishEvent(EVENT_FRAME_START, NULL);
        
        // 입력 처리
        if (game.useEventSystem) {
            ProcessInputEvents();
        }
        
        // 이벤트 큐 처리
        ProcessEventQueue();
        
        // 게임 업데이트
        UpdateGame(&game);
        
        // 그리기
        DrawGame(&game);
        
        // 프레임 종료 이벤트
        PublishEvent(EVENT_FRAME_END, NULL);
    }
    
    // 정리
    CleanupEventSystem();
    CleanupGame(&game);
    CloseWindow();
    
    return 0;
}
```

---

## 6. 메모리 관리

### 6.1 메모리 풀 시스템

100,000개의 파티클을 효율적으로 관리하기 위해 메모리 풀을 사용합니다:

```c
// memory_pool.h
typedef struct {
    void* memory;           // 전체 메모리 블록
    void** freeList;        // 사용 가능한 블록 리스트
    size_t blockSize;       // 각 블록의 크기
    size_t blockCount;      // 전체 블록 수
    size_t freeCount;       // 사용 가능한 블록 수
} MemoryPool;

// 메모리 풀 초기화
MemoryPool particlePool;
MemoryPool_Init(&particlePool, sizeof(Particle), 100000);

// 메모리 할당
Particle* particle = (Particle*)MemoryPool_Alloc(&particlePool);

// 메모리 반환
MemoryPool_Free(&particlePool, particle);

// 메모리 풀 해제
MemoryPool_Destroy(&particlePool);
```

### 6.2 메모리 관리 팁

```c
// ❌ 나쁜 예: 게임 루프 내에서 동적 할당
while (gameRunning) {
    Enemy* enemy = malloc(sizeof(Enemy));  // 매 프레임 할당 - 느림!
    // ...
    free(enemy);
}

// ✅ 좋은 예: 메모리 풀 사용
while (gameRunning) {
    Enemy* enemy = MemoryPool_Alloc(&enemyPool);  // 빠른 할당
    // ...
    MemoryPool_Free(&enemyPool, enemy);
}
```

---

## 7. 이벤트 시스템

### 7.1 이벤트 기반 아키텍처

이벤트 시스템을 통해 게임의 각 컴포넌트를 분리하고 유연하게 만듭니다:

```c
// 이벤트 타입 정의
typedef enum {
    EVENT_ENEMY_DESTROYED,
    EVENT_COLLISION_PARTICLE_ENEMY,
    EVENT_STAGE_COMPLETED,
    // ...
} EventType;

// 이벤트 발행 (적이 파괴되었을 때)
void OnEnemyKilled(EventSystem* system, Enemy* enemy) {
    // 이벤트 데이터를 메모리 풀에서 할당
    EnemyDestroyedData data = {
        .position = enemy->position,
        .scoreValue = enemy->scoreValue,
        .enemyType = enemy->type
    };
    
    PublishEvent(system, EVENT_ENEMY_DESTROYED, &data);
}

// 게임 초기화 시 이벤트 핸들러 등록
void RegisterGameEventHandlers(EventSystem* system, Game* game) {
    // 적 파괴 이벤트에 대한 핸들러들 등록
    SubscribeToEvent(system, EVENT_ENEMY_DESTROYED, HandleScoreUpdate, game);
    SubscribeToEvent(system, EVENT_ENEMY_DESTROYED, HandleExplosionEffect, game);
    // 다른 이벤트 핸들러들도 여기서 등록...
}

// 점수 업데이트 핸들러
void HandleScoreUpdate(const Event* event, void* context) {
    Game* game = (Game*)context;
    EnemyDestroyedData* data = (EnemyDestroyedData*)event->data;
    
    game->score += data->scoreValue;
    printf("점수 획득: +%d (총점: %d)\n", data->scoreValue, game->score);
}

// 폭발 효과 생성 핸들러
void HandleExplosionEffect(const Event* event, void* context) {
    EnemyDestroyedData* data = (EnemyDestroyedData*)event->data;
    
    // 폭발 파티클 효과 생성 (실제 구현은 프로젝트에 따라)
    // CreateExplosionAt(data->position);
    printf("폭발 효과: (%.1f, %.1f)\n", data->position.x, data->position.y);
}
```

### 7.2 이벤트 시스템의 장점

1. **느슨한 결합 (Loose Coupling)**: 컴포넌트들이 직접 참조하지 않음
2. **확장성**: 새로운 기능을 쉽게 추가 가능
3. **디버깅**: 이벤트 로그를 통한 쉬운 디버깅
4. **재사용성**: 이벤트 핸들러를 다양한 곳에서 재사용

---

## 8. 게임 물리학

### 8.1 파티클 인력 시스템

플레이어가 파티클을 끌어당기는 물리 시스템:

```c
// physics.c의 파티클 인력 구현
void ApplyPlayerAttraction(Game* game) {
    float attractionRadius = 100.0f;
    float attractionForce = 2.0f;
    
    // 스페이스바를 누르면 인력 2배
    if (IsKeyDown(KEY_SPACE)) {
        attractionForce *= 2.0f;
    }
    
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Particle* p = &game->particles[i];
        
        // 플레이어와 파티클 사이의 거리 계산
        float dx = game->player.position.x - p->position.x;
        float dy = game->player.position.y - p->position.y;
        float distance = sqrtf(dx*dx + dy*dy);
        
        // 인력 범위 내에 있으면
        if (distance < attractionRadius && distance > 0.1f) {
            // 정규화된 방향 벡터
            float dirX = dx / distance;
            float dirY = dy / distance;
            
            // 거리에 반비례하는 힘 적용
            float force = attractionForce * (1.0f - distance/attractionRadius);
            p->velocity.x += dirX * force * game->deltaTime;
            p->velocity.y += dirY * force * game->deltaTime;
        }
    }
}
```

### 8.2 충돌 감지

```c
// 원형 충돌 감지
bool CheckCircleCollision(Vector2 pos1, float radius1, Vector2 pos2, float radius2) {
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    float distance = sqrtf(dx*dx + dy*dy);
    
    return distance < (radius1 + radius2);
}

// 파티클-적 충돌 처리
void CheckParticleEnemyCollisions(Game* game) {
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        for (int j = 0; j < game->enemyCount; j++) {
            if (CheckCircleCollision(
                game->particles[i].position, 1.0f,
                game->enemies[j].position, game->enemies[j].radius)) {
                
                // 충돌 이벤트 발행
                CollisionData* data = AllocEventData(sizeof(CollisionData));
                data->particleIndex = i;
                data->enemyIndex = j;
                PublishEvent(EVENT_COLLISION_PARTICLE_ENEMY, data);
                
                // 적 체력 감소
                game->enemies[j].health--;
            }
        }
    }
}
```

### 8.3 토로이달 월드 (화면 경계 순환)

```c
// 화면 경계를 넘으면 반대편에서 나타나기
void WrapAroundScreen(Vector2* position, int screenWidth, int screenHeight) {
    if (position->x < 0) position->x += screenWidth;
    if (position->x >= screenWidth) position->x -= screenWidth;
    if (position->y < 0) position->y += screenHeight;
    if (position->y >= screenHeight) position->y -= screenHeight;
}

// 토로이달 거리 계산 (최단 거리)
Vector2 GetToroidalDirection(Vector2 from, Vector2 to, 
                           int screenWidth, int screenHeight) {
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    
    // X축 최단 경로
    if (fabs(dx) > screenWidth/2) {
        dx = dx > 0 ? dx - screenWidth : dx + screenWidth;
    }
    
    // Y축 최단 경로
    if (fabs(dy) > screenHeight/2) {
        dy = dy > 0 ? dy - screenHeight : dy + screenHeight;
    }
    
    // 정규화
    float distance = sqrtf(dx*dx + dy*dy);
    return (Vector2){dx/distance, dy/distance};
}
```

---

## 9. 실전 예제

### 9.1 간단한 파티클 시스템 만들기

```c
#include "raylib.h"
#include <stdlib.h>

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float lifetime;
} SimpleParticle;

#define MAX_PARTICLES 1000

int main(void) {
    InitWindow(800, 600, "간단한 파티클 시스템");
    SetTargetFPS(60);
    
    SimpleParticle particles[MAX_PARTICLES];
    int particleCount = 0;
    
    while (!WindowShouldClose()) {
        // 마우스 클릭시 파티클 생성
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && particleCount < MAX_PARTICLES) {
            Vector2 mousePos = GetMousePosition();
            
            for (int i = 0; i < 10; i++) {  // 한 번에 10개 생성
                particles[particleCount].position = mousePos;
                particles[particleCount].velocity = (Vector2){
                    GetRandomValue(-100, 100) / 50.0f,
                    GetRandomValue(-100, 100) / 50.0f
                };
                particles[particleCount].color = (Color){
                    GetRandomValue(100, 255),
                    GetRandomValue(100, 255),
                    GetRandomValue(100, 255),
                    255
                };
                particles[particleCount].lifetime = 2.0f;
                particleCount++;
            }
        }
        
        // 파티클 업데이트
        float deltaTime = GetFrameTime();
        for (int i = 0; i < particleCount; i++) {
            particles[i].position.x += particles[i].velocity.x;
            particles[i].position.y += particles[i].velocity.y;
            particles[i].velocity.y += 9.8f * deltaTime;  // 중력
            particles[i].lifetime -= deltaTime;
            
            // 수명이 다한 파티클 제거
            if (particles[i].lifetime <= 0) {
                particles[i] = particles[particleCount - 1];
                particleCount--;
                i--;
            }
        }
        
        // 그리기
        BeginDrawing();
        ClearBackground(BLACK);
        
        for (int i = 0; i < particleCount; i++) {
            int alpha = (int)(particles[i].lifetime * 127);
            Color color = particles[i].color;
            color.a = alpha;
            DrawCircleV(particles[i].position, 3, color);
        }
        
        DrawText(TextFormat("파티클 수: %d", particleCount), 10, 10, 20, WHITE);
        DrawText("마우스 클릭으로 파티클 생성", 10, 40, 20, WHITE);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

### 9.2 적 AI 시스템

```c
// 플레이어를 추적하는 적 AI
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float speed;
    int health;
    EnemyType type;
} Enemy;

void UpdateEnemyAI(Enemy* enemy, Vector2 playerPos, float deltaTime) {
    switch (enemy->type) {
        case ENEMY_TYPE_TRACKER:
            // 플레이어 추적
            {
                Vector2 direction = {
                    playerPos.x - enemy->position.x,
                    playerPos.y - enemy->position.y
                };
                float distance = sqrtf(direction.x * direction.x + 
                                     direction.y * direction.y);
                
                if (distance > 0) {
                    direction.x /= distance;
                    direction.y /= distance;
                    
                    enemy->velocity.x = direction.x * enemy->speed;
                    enemy->velocity.y = direction.y * enemy->speed;
                }
            }
            break;
            
        case ENEMY_TYPE_ORBITER:
            // 원형 궤도 이동
            {
                static float angle = 0;
                angle += deltaTime * 2.0f;
                
                float radius = 200.0f;
                enemy->position.x = playerPos.x + cos(angle) * radius;
                enemy->position.y = playerPos.y + sin(angle) * radius;
            }
            break;
            
        case ENEMY_TYPE_ZIGZAG:
            // 지그재그 이동
            {
                static float zigzagTimer = 0;
                zigzagTimer += deltaTime;
                
                enemy->velocity.x = sin(zigzagTimer * 5) * enemy->speed;
                enemy->velocity.y = enemy->speed;
            }
            break;
    }
    
    // 위치 업데이트
    enemy->position.x += enemy->velocity.x * deltaTime;
    enemy->position.y += enemy->velocity.y * deltaTime;
}
```

---

## 10. 프로젝트 빌드 및 테스트

### 10.1 Makefile 이해하기

```makefile
# 컴파일러 설정
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2

# Raylib 경로 (macOS)
RAYLIB_PATH = /opt/homebrew/Cellar/raylib/5.5
INCLUDES = -I$(RAYLIB_PATH)/include
LIBS = -L$(RAYLIB_PATH)/lib -lraylib -framework OpenGL \
       -framework Cocoa -framework IOKit

# 소스 파일
SOURCES = src/main.c \
          src/core/game.c \
          src/core/physics.c \
          src/entities/particle.c \
          src/entities/player.c

# 오브젝트 파일
OBJECTS = $(SOURCES:.c=.o)

# 실행 파일
TARGET = particle_storm

# 빌드 규칙
$(TARGET): $(OBJECTS)
    $(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.c
    $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
    rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
    ./$(TARGET)
```

### 10.2 단위 테스트 작성

minunit 테스트 프레임워크를 사용한 테스트:

```c
#include "minunit/minunit.h"
#include "../src/entities/particle.h"

// 테스트 셋업
void test_setup(void) {
    // 테스트 전 초기화
}

void test_teardown(void) {
    // 테스트 후 정리
}

// 파티클 초기화 테스트
MU_TEST(test_particle_init) {
    Particle p = InitParticle(800, 600);
    
    // 위치가 화면 범위 내에 있는지 확인
    mu_assert(p.position.x >= 0 && p.position.x < 800, 
             "파티클 X 위치가 범위를 벗어남");
    mu_assert(p.position.y >= 0 && p.position.y < 600, 
             "파티클 Y 위치가 범위를 벗어남");
    
    // 속도가 -1.0 ~ 1.0 범위인지 확인
    mu_assert(p.velocity.x >= -1.0f && p.velocity.x <= 1.0f,
             "파티클 X 속도가 범위를 벗어남");
}

// 거리 계산 테스트
MU_TEST(test_particle_distance) {
    Particle p;
    p.position = (Vector2){0, 0};
    
    Vector2 target = {3, 4};
    float distance = GetParticleDistance(p, target);
    
    // 3-4-5 직각삼각형
    mu_assert_double_eq(5.0, distance);
}

// 테스트 스위트
MU_TEST_SUITE(particle_test_suite) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    
    MU_RUN_TEST(test_particle_init);
    MU_RUN_TEST(test_particle_distance);
}

int main(int argc, char *argv[]) {
    MU_RUN_SUITE(particle_test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
```

### 10.3 디버깅 팁

```c
// 디버그 매크로 정의
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", \
                __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) // 릴리즈 모드에서는 아무것도 하지 않음
#endif

// 사용 예제
void UpdateGame(Game* game) {
    DEBUG_PRINT("플레이어 위치: (%.2f, %.2f)", 
                game->player.position.x, game->player.position.y);
    DEBUG_PRINT("적 수: %d", game->enemyCount);
    
    // 게임 로직...
}

// 게임 내 디버그 정보 표시
void DrawDebugInfo(Game* game) {
    if (IsKeyDown(KEY_F3)) {  // F3 키로 디버그 정보 토글
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, GREEN);
        DrawText(TextFormat("파티클: %d", PARTICLE_COUNT), 10, 35, 20, GREEN);
        DrawText(TextFormat("적: %d", game->enemyCount), 10, 60, 20, GREEN);
        DrawText(TextFormat("점수: %d", game->score), 10, 85, 20, GREEN);
        DrawText(TextFormat("스테이지: %d", game->currentStageNumber), 10, 110, 20, GREEN);
        
        // 메모리 사용량 (메모리 풀)
        DrawText(TextFormat("메모리 풀 사용률: %.1f%%", 
                GetMemoryPoolUsage(&g_collisionEventPool)), 10, 135, 20, YELLOW);
    }
}
```

---

## 📝 핵심 요약

### C 프로그래밍 베스트 프랙티스
1. **메모리 관리**: 항상 `malloc`한 메모리는 `free`하기
2. **포인터 안전성**: NULL 체크 습관화
3. **구조체 활용**: 관련 데이터를 논리적으로 그룹화
4. **함수 분리**: 하나의 함수는 하나의 기능만
5. **const 사용**: 변경되지 않는 매개변수는 const로 선언

### Raylib 개발 팁
1. **게임 루프 구조**: Update → Draw 순서 유지
2. **리소스 관리**: Load와 Unload 쌍 맞추기
3. **프레임 독립적 이동**: `deltaTime` 활용
4. **디버그 정보**: 개발 중 FPS와 상태 정보 표시

### 성능 최적화
1. **메모리 풀**: 빈번한 할당/해제 대신 메모리 풀 사용
2. **공간 분할**: 충돌 감지를 위한 그리드/쿼드트리
3. **이벤트 배칭**: 유사한 이벤트 묶어서 처리
4. **컬링**: 화면 밖 객체 처리 생략

---

## 🚀 다음 단계

1. **프로젝트 확장하기**
   - 새로운 적 타입 추가
   - 파워업 시스템 구현
   - 멀티플레이어 기능

2. **고급 기능 학습**
   - 셰이더 프로그래밍
   - 3D 그래픽스
   - 네트워킹

3. **최적화 기법**
   - 멀티스레딩
   - SIMD 명령어
   - GPU 가속

---

## 📚 참고 자료

- [Raylib 공식 문서](https://www.raylib.com/)
- [Raylib 치트시트](https://www.raylib.com/cheatsheet/cheatsheet.html)
- [Raylib 예제](https://github.com/raysan5/raylib/tree/master/examples)
- [C 프로그래밍 표준](https://en.cppreference.com/w/c)

---

**작성자**: Particle Storm 프로젝트 팀  
**최종 업데이트**: 2025년 9월 6일

이 문서는 지속적으로 업데이트됩니다. 질문이나 개선 사항이 있으면 프로젝트 이슈 트래커에 등록해 주세요.