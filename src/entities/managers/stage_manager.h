#ifndef STAGE_MANAGER_H
#define STAGE_MANAGER_H

#include "raylib.h"
#include "../enemy.h"

// Stage states
typedef enum {
    STAGE_STATE_INTRO,        // Stage introduction screen
    STAGE_STATE_COUNTDOWN,    // Countdown before game starts
    STAGE_STATE_ACTIVE,       // Stage is active
    STAGE_STATE_COMPLETE,     // Stage completed
    STAGE_STATE_BOSS_WARNING, // Boss warning before boss stage
    STAGE_STATE_TRANSITION    // Transition to next stage
} StageState;

// Wave structure for enemy spawning
typedef struct {
    float spawnTime;          // Time when this wave starts
    int enemyCount;           // Number of enemies in this wave
    EnemyType enemyTypes[10]; // Types of enemies to spawn
    float spawnDelay;         // Delay between each enemy spawn
    Vector2 spawnPattern[10]; // Specific spawn positions (optional)
    bool useRandomSpawn;      // Use random spawn positions
} EnemyWave;

// Stage definition
typedef struct {
    int stageNumber;
    char name[64];
    char description[256];
    
    // Enemy waves
    EnemyWave waves[10];      // Maximum 10 waves per stage
    int waveCount;
    int currentWave;
    float waveTimer;
    
    // Stage objectives
    int targetKills;          // Enemies to kill to complete stage
    float timeLimit;          // Time limit (0 = no limit)
    int maxEnemiesAlive;      // Maximum enemies alive at once
    
    // Stage modifiers
    float enemyHealthMultiplier;
    float enemySpeedMultiplier;
    float enemySizeMultiplier;
    float particleAttractionMultiplier;
    
    // Stage state
    StageState state;
    float stateTimer;
    int enemiesKilled;
    int totalEnemiesSpawned;
    
    // Visual theme
    Color backgroundColor;
    Color particleColor;
    bool hasSpecialEffect;
} Stage;

// Stage definitions for all 10 stages
Stage CreateStage1(void);  // Tutorial - Basic enemies
Stage CreateStage2(void);  // Tracker enemies introduced
Stage CreateStage3(void);  // Speed challenge
Stage CreateStage4(void);  // Splitter madness
Stage CreateStage5(void);  // Orbital patterns
Stage CreateStage6(void);  // First boss
Stage CreateStage7(void);  // Teleporter chaos
Stage CreateStage8(void);  // Repulsor fields
Stage CreateStage9(void);  // Cluster explosions
Stage CreateStage10(void); // Final boss

// Stage manager functions
void InitStageManager(void);
void UpdateStage(Stage* stage, float deltaTime);
bool ShouldSpawnEnemy(Stage* stage, float currentTime);
EnemyType GetNextEnemyType(Stage* stage);
Vector2 GetEnemySpawnPosition(Stage* stage, int screenWidth, int screenHeight);
bool IsStageComplete(Stage* stage);
void TransitionStageData(Stage* currentStage, Stage* nextStage);

// Stage UI functions
void DrawStageIntro(Stage* stage, int screenWidth, int screenHeight);
void DrawStageProgress(Stage* stage, int screenWidth);
void DrawBossWarning(int screenWidth, int screenHeight);
void DrawStageComplete(Stage* stage, int screenWidth, int screenHeight);

// Wave management
void StartNextWave(Stage* stage);
bool IsWaveComplete(Stage* stage);

#endif // STAGE_MANAGER_H 