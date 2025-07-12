#ifndef GAME_H
#define GAME_H

#include "../entities/player.h"
#include "../entities/particle.h"
#include "../entities/enemy.h"
#include "../entities/explosion.h"
#include "../entities/managers/enemy_manager.h"
#include "../entities/managers/particle_manager.h"
#include "../entities/managers/stage_manager.h"
#include "event/event_system.h"

// Global screen dimensions
extern int g_screenWidth;
extern int g_screenHeight;

// Constants
#define PARTICLE_COUNT 100000  // Maximum number of particles
#define DEFAULT_ATTRACTION_FORCE 1.0f  // Default force for particle attraction
#define BOOSTED_ATTRACTION_FORCE 5.0f  // Boosted force when space key is pressed
#define MAX_NAME_LENGTH 16
#define MAX_SCOREBOARD_ENTRIES 10

// Game state enum
typedef enum {
    GAME_STATE_TUTORIAL,   // 설명/조작법 화면
    GAME_STATE_PLAYING,
    GAME_STATE_OVER,
    GAME_STATE_SCORE_ENTRY,
    GAME_STATE_STAGE_INTRO,    // New: Stage introduction
    GAME_STATE_STAGE_COMPLETE, // New: Stage completed
    GAME_STATE_VICTORY        // New: All stages completed
} GameState;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
} ScoreEntry;

// Game state structure
typedef struct Game {
    // Window properties
    int screenWidth;
    int screenHeight;
    
    // Game properties
    int moveSpeed;
    float deltaTime;
    float lastEnemySpawnTime;  // Time when last enemy was spawned
    int enemyCount;           // Current number of enemies
    int score;                // Player score
    GameState gameState;      // Current game state
    
    // Stage system
    Stage currentStage;       // Current stage data
    int currentStageNumber;   // Current stage number (1-10)
    float stageTimer;         // Timer for current stage
    bool stageTransition;     // Flag for stage transition
    
    // Game entities
    Player player;
    Particle* particles;  // Dynamic array of particles
    Enemy* enemies;  // Dynamic array of enemies
    ExplosionParticle explosionParticles[MAX_EXPLOSION_PARTICLES];
    int explosionParticleCount;
    
    // Scoreboard
    char playerName[MAX_NAME_LENGTH];
    int nameLength;
    ScoreEntry scoreboard[MAX_SCOREBOARD_ENTRIES];
    int scoreboardCount;
    
    // 이벤트 시스템 관련 플래그
    bool useEventSystem;  // 이벤트 시스템 사용 여부
    
    // Statistics
    int totalEnemiesKilled;
    int enemiesKilledThisStage;
} Game;

// Game initialization and cleanup
Game InitGame(int screenWidth, int screenHeight);
void CleanupGame(Game* game);

// Game loop functions
void UpdateGame(Game* game);
void DrawGame(Game* game);

// Particle management functions
int FindNearestParticleInDirection(Game* game, Vector2 direction);
void SwapPlayerWithParticle(Game* game, int particleIndex);

// Stage management functions
void LoadStage(Game* game, int stageNumber);
void UpdateStageSystem(Game* game);
void CheckStageCompletion(Game* game);
void TransitionToNextStage(Game* game);

// Enemy management functions (updated)
void SpawnEnemyByType(Game* game, EnemyType type);
void SpawnEnemyFromStage(Game* game);
void HandleEnemySplit(Game* game, Enemy* originalEnemy);
void HandleClusterExplosion(Game* game, Enemy* clusterEnemy);

// Managers and physics functions
void SpawnEnemyIfNeeded(Game* game);
void UpdateAllEnemies(Game* game);
void UpdateAllParticles(Game* game, bool isSpacePressed);
void UpdateAllExplosionParticles(Game* game);
bool CheckCollisionEnemyParticle(Enemy enemy, Particle particle);
void ProcessEnemyCollisions(Game* game);

// 이벤트 핸들러 등록 함수
void RegisterEnemyEventHandlers(void);
void RegisterCollisionEventHandlers(Game* game);
void RegisterStageEventHandlers(Game* game);

// Scoreboard functions
typedef enum { SCOREBOARD_OK, SCOREBOARD_FILE_ERROR } ScoreboardResult;
ScoreboardResult LoadScoreboard(Game* game, const char* filename);
ScoreboardResult SaveScoreboard(Game* game, const char* filename);
void AddScoreToScoreboard(Game* game);

#endif // GAME_H 