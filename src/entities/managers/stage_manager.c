#include "stage_manager.h"
#include <string.h>
#include <stdio.h>

// Stage 1: Blackhole Enemy Introduction
Stage CreateStage1(void) {
    Stage stage = {0};
    stage.stageNumber = 1;
    strcpy(stage.name, "Gravitational Anomaly");
    strcpy(stage.description, "Use gravity against enemies - Blackhole awakens when alone!");
    
    // Introduce blackhole enemy with some basic enemies
    stage.waveCount = 3;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC},
        .spawnDelay = 1.0f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 8.0f,
        .enemyCount = 1,
        .enemyTypes = {ENEMY_TYPE_BLACKHOLE},
        .spawnDelay = 0.0f,
        .useRandomSpawn = false,
        .spawnPattern = {{400, 200}}  // Center-top position
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 15.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_BASIC, ENEMY_TYPE_TRACKER, ENEMY_TYPE_BASIC},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    
    stage.targetKills = 9;
    stage.maxEnemiesAlive = 5;
    stage.enemyHealthMultiplier = 1.2f;  // Blackholes are slightly tougher
    stage.enemySpeedMultiplier = 0.8f;   // Blackholes move slower
    stage.enemySizeMultiplier = 1.3f;    // Blackholes are larger
    stage.backgroundColor = (Color){20, 20, 30, 255};  // Dark space background
    stage.particleColor = (Color){200, 200, 255, 255}; // Light blue particles
    stage.particleAttractionMultiplier = 1.5f;  // Stronger particle attraction
    
    return stage;
}

// Stage 2: Tracker enemies introduced
Stage CreateStage2(void) {
    Stage stage = {0};
    stage.stageNumber = 2;
    strcpy(stage.name, "The Hunt Begins");
    strcpy(stage.description, "Beware! Enemies now track your position");
    
    stage.waveCount = 3;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 2,
        .enemyTypes = {ENEMY_TYPE_TRACKER, ENEMY_TYPE_BASIC},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 8.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_TRACKER, ENEMY_TYPE_TRACKER, ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 18.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_TRACKER, ENEMY_TYPE_TRACKER, ENEMY_TYPE_TRACKER},
        .spawnDelay = 1.0f,
        .useRandomSpawn = true
    };
    
    stage.targetKills = 9;
    stage.maxEnemiesAlive = 5;
    stage.enemyHealthMultiplier = 1.2f;
    stage.enemySpeedMultiplier = 1.1f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){240, 240, 240, 255};
    stage.particleColor = DARKGRAY;
    
    return stage;
}

// Stage 3: Speed challenge
Stage CreateStage3(void) {
    Stage stage = {0};
    stage.stageNumber = 3;
    strcpy(stage.name, "Speed Demons");
    strcpy(stage.description, "Fast enemies incoming! Use boost wisely");
    
    stage.waveCount = 3;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_SPEEDY, ENEMY_TYPE_BASIC, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 7.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_SPEEDY, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_TRACKER, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_BASIC},
        .spawnDelay = 1.0f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 15.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_SPEEDY, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 0.8f,
        .useRandomSpawn = true
    };
    
    stage.targetKills = 12;
    stage.maxEnemiesAlive = 6;
    stage.enemyHealthMultiplier = 1.0f;  // Speedy enemies have less health
    stage.enemySpeedMultiplier = 1.5f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){220, 220, 255, 255};
    stage.particleColor = DARKBLUE;
    
    return stage;
}

// Stage 4: Splitter madness
Stage CreateStage4(void) {
    Stage stage = {0};
    stage.stageNumber = 4;
    strcpy(stage.name, "Divide and Conquer");
    strcpy(stage.description, "Enemies split when destroyed - Control the chaos!");
    
    stage.waveCount = 3;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 2,
        .enemyTypes = {ENEMY_TYPE_SPLITTER, ENEMY_TYPE_BASIC},
        .spawnDelay = 3.0f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 10.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_SPLITTER, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_TRACKER},
        .spawnDelay = 2.5f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 20.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_SPLITTER, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_SPLITTER},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    
    stage.targetKills = 20;  // More kills needed due to splitting
    stage.maxEnemiesAlive = 8;
    stage.enemyHealthMultiplier = 1.3f;
    stage.enemySpeedMultiplier = 1.0f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){255, 220, 220, 255};
    stage.particleColor = MAROON;
    
    return stage;
}

// Stage 5: Orbital patterns
Stage CreateStage5(void) {
    Stage stage = {0};
    stage.stageNumber = 5;
    strcpy(stage.name, "Orbital Mechanics");
    strcpy(stage.description, "Enemies with complex movement patterns");
    
    stage.waveCount = 4;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_ORBITER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_BASIC},
        .spawnDelay = 2.0f,
        .useRandomSpawn = false,
        .spawnPattern = {{400, 200}, {200, 400}, {600, 400}}
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 8.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_ORBITER, ENEMY_TYPE_TRACKER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 16.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_ORBITER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_ORBITER},
        .spawnDelay = 1.2f,
        .useRandomSpawn = true
    };
    stage.waves[3] = (EnemyWave){
        .spawnTime = 25.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_ORBITER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_ORBITER},
        .spawnDelay = 0.5f,
        .useRandomSpawn = false,
        .spawnPattern = {{400, 400}, {300, 400}, {500, 400}}
    };
    
    stage.targetKills = 15;
    stage.maxEnemiesAlive = 7;
    stage.enemyHealthMultiplier = 1.4f;
    stage.enemySpeedMultiplier = 1.2f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){220, 255, 220, 255};
    stage.particleColor = DARKGREEN;
    
    return stage;
}

// Stage 6: First boss
Stage CreateStage6(void) {
    Stage stage = {0};
    stage.stageNumber = 6;
    strcpy(stage.name, "Guardian of the Void");
    strcpy(stage.description, "BOSS BATTLE - Defeat the Guardian!");
    
    stage.waveCount = 2;
    // Boss spawn
    stage.waves[0] = (EnemyWave){
        .spawnTime = 2.0f,
        .enemyCount = 1,
        .enemyTypes = {ENEMY_TYPE_BOSS_1},
        .spawnDelay = 0.0f,
        .useRandomSpawn = false,
        .spawnPattern = {{400, 300}}
    };
    // Minions during boss fight
    stage.waves[1] = (EnemyWave){
        .spawnTime = 10.0f,
        .enemyCount = 6,
        .enemyTypes = {ENEMY_TYPE_BASIC, ENEMY_TYPE_TRACKER, ENEMY_TYPE_BASIC, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_BASIC, ENEMY_TYPE_TRACKER},
        .spawnDelay = 3.0f,
        .useRandomSpawn = true
    };
    
    stage.targetKills = 7;  // Boss + minions
    stage.maxEnemiesAlive = 5;
    stage.enemyHealthMultiplier = 2.0f;  // Boss has much more health
    stage.enemySpeedMultiplier = 0.8f;
    stage.enemySizeMultiplier = 1.5f;  // Boss is larger
    stage.backgroundColor = (Color){40, 40, 40, 255};
    stage.particleColor = RED;
    stage.hasSpecialEffect = true;
    
    return stage;
}

// Stage 7: Teleporter chaos
Stage CreateStage7(void) {
    Stage stage = {0};
    stage.stageNumber = 7;
    strcpy(stage.name, "Quantum Flux");
    strcpy(stage.description, "Enemies can teleport - Stay alert!");
    
    stage.waveCount = 4;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 2,
        .enemyTypes = {ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_BASIC},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 6.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_TRACKER, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 14.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_TELEPORTER},
        .spawnDelay = 1.2f,
        .useRandomSpawn = true
    };
    stage.waves[3] = (EnemyWave){
        .spawnTime = 22.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_TELEPORTER},
        .spawnDelay = 0.8f,
        .useRandomSpawn = true
    };
    
    stage.targetKills = 14;
    stage.maxEnemiesAlive = 6;
    stage.enemyHealthMultiplier = 1.5f;
    stage.enemySpeedMultiplier = 1.3f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){200, 200, 255, 255};
    stage.particleColor = PURPLE;
    
    return stage;
}

// Stage 8: Repulsor fields
Stage CreateStage8(void) {
    Stage stage = {0};
    stage.stageNumber = 8;
    strcpy(stage.name, "Magnetic Storm");
    strcpy(stage.description, "Enemies repel particles - Find new strategies!");
    
    stage.waveCount = 4;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 2,
        .enemyTypes = {ENEMY_TYPE_REPULSOR, ENEMY_TYPE_BASIC},
        .spawnDelay = 3.0f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 8.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_REPULSOR, ENEMY_TYPE_TRACKER, ENEMY_TYPE_REPULSOR},
        .spawnDelay = 2.5f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 16.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_REPULSOR, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_REPULSOR, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_REPULSOR},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    stage.waves[3] = (EnemyWave){
        .spawnTime = 25.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_REPULSOR, ENEMY_TYPE_REPULSOR, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_REPULSOR},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    
    stage.targetKills = 14;
    stage.maxEnemiesAlive = 7;
    stage.enemyHealthMultiplier = 1.6f;
    stage.enemySpeedMultiplier = 1.1f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){255, 240, 200, 255};
    stage.particleColor = ORANGE;
    stage.particleAttractionMultiplier = 0.7f;  // Particles are harder to control
    
    return stage;
}

// Stage 9: Cluster explosions
Stage CreateStage9(void) {
    Stage stage = {0};
    stage.stageNumber = 9;
    strcpy(stage.name, "Chain Reaction");
    strcpy(stage.description, "Explosive enemies - One explosion triggers another!");
    
    stage.waveCount = 5;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_BASIC, ENEMY_TYPE_CLUSTER},
        .spawnDelay = 2.5f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 8.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_TRACKER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 15.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_ORBITER},
        .spawnDelay = 1.8f,
        .useRandomSpawn = true
    };
    stage.waves[3] = (EnemyWave){
        .spawnTime = 23.0f,
        .enemyCount = 6,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_REPULSOR, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_CLUSTER},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    stage.waves[4] = (EnemyWave){
        .spawnTime = 32.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_CLUSTER},
        .spawnDelay = 0.5f,
        .useRandomSpawn = false,
        .spawnPattern = {{200, 200}, {600, 200}, {200, 600}, {600, 600}}
    };
    
    stage.targetKills = 22;
    stage.maxEnemiesAlive = 8;
    stage.enemyHealthMultiplier = 1.7f;
    stage.enemySpeedMultiplier = 1.4f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){255, 200, 200, 255};
    stage.particleColor = DARKPURPLE;
    
    return stage;
}

// Stage 10: Final boss
Stage CreateStage10(void) {
    Stage stage = {0};
    stage.stageNumber = 10;
    strcpy(stage.name, "The Particle Overlord");
    strcpy(stage.description, "FINAL BOSS - The ultimate challenge awaits!");
    
    stage.waveCount = 3;
    // Final boss
    stage.waves[0] = (EnemyWave){
        .spawnTime = 3.0f,
        .enemyCount = 1,
        .enemyTypes = {ENEMY_TYPE_BOSS_FINAL},
        .spawnDelay = 0.0f,
        .useRandomSpawn = false,
        .spawnPattern = {{400, 400}}
    };
    // First minion wave
    stage.waves[1] = (EnemyWave){
        .spawnTime = 15.0f,
        .enemyCount = 8,
        .enemyTypes = {ENEMY_TYPE_SPEEDY, ENEMY_TYPE_TRACKER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_SPLITTER, 
                      ENEMY_TYPE_ORBITER, ENEMY_TYPE_REPULSOR, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_BASIC},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    // Second minion wave (during boss phase 2)
    stage.waves[2] = (EnemyWave){
        .spawnTime = 35.0f,
        .enemyCount = 6,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_REPULSOR, 
                      ENEMY_TYPE_CLUSTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    
    stage.targetKills = 15;  // Boss + all minions
    stage.maxEnemiesAlive = 10;
    stage.enemyHealthMultiplier = 3.0f;  // Final boss has massive health
    stage.enemySpeedMultiplier = 1.5f;
    stage.enemySizeMultiplier = 2.0f;  // Final boss is much larger
    stage.backgroundColor = BLACK;
    stage.particleColor = GOLD;
    stage.hasSpecialEffect = true;
    
    return stage;
}

// Update stage state
void UpdateStage(Stage* stage, float deltaTime) {
    stage->stateTimer += deltaTime;
    stage->waveTimer += deltaTime;
    
    switch (stage->state) {
        case STAGE_STATE_INTRO:
            if (stage->stateTimer > 3.0f) {
                stage->state = STAGE_STATE_ACTIVE;
                stage->stateTimer = 0.0f;
            }
            break;
            
        case STAGE_STATE_ACTIVE:
            // Check for wave spawning
            if (stage->currentWave < stage->waveCount) {
                EnemyWave* wave = &stage->waves[stage->currentWave];
                if (stage->waveTimer >= wave->spawnTime) {
                    // Wave is ready to spawn
                    if (IsWaveComplete(stage)) {
                        StartNextWave(stage);
                    }
                }
            }
            
            // Check stage completion
            if (IsStageComplete(stage)) {
                stage->state = STAGE_STATE_COMPLETE;
                stage->stateTimer = 0.0f;
            }
            break;
            
        case STAGE_STATE_COMPLETE:
            if (stage->stateTimer > 3.0f) {
                stage->state = STAGE_STATE_TRANSITION;
                stage->stateTimer = 0.0f;
            }
            break;
            
        case STAGE_STATE_BOSS_WARNING:
            if (stage->stateTimer > 2.0f) {
                stage->state = STAGE_STATE_ACTIVE;
                stage->stateTimer = 0.0f;
            }
            break;
            
        case STAGE_STATE_TRANSITION:
            // No action needed, handled by game logic
            break;
            
        default:
            break;
    }
}

// Check if enemy should spawn
// Static variables for spawn timing
static float g_lastSpawnTime = -999.0f;
static int g_lastStageNumber = -1;

// Reset spawn timing (call this when starting a new game)
void ResetSpawnTiming(void) {
    g_lastSpawnTime = -999.0f;
    g_lastStageNumber = -1;
}

bool ShouldSpawnEnemy(Stage* stage, float currentTime) {
    if (stage->state != STAGE_STATE_ACTIVE) return false;
    if (stage->currentWave >= stage->waveCount) return false;
    
    EnemyWave* wave = &stage->waves[stage->currentWave];
    
    // Check if it's time for this wave
    if (currentTime < wave->spawnTime) return false;
    
    // Check if we've spawned all enemies in this wave
    int spawnedInWave = 0;
    for (int i = 0; i <= stage->currentWave; i++) {
        if (i < stage->currentWave) {
            spawnedInWave += stage->waves[i].enemyCount;
        }
    }
    
    int enemiesInCurrentWave = stage->totalEnemiesSpawned - spawnedInWave;
    if (enemiesInCurrentWave >= wave->enemyCount) return false;
    
    // Reset spawn time when stage changes or when it's the first spawn of a new stage
    if (g_lastStageNumber != stage->stageNumber || stage->totalEnemiesSpawned == 0) {
        g_lastSpawnTime = currentTime - wave->spawnDelay - 1.0f;  // Ensure immediate spawn
        g_lastStageNumber = stage->stageNumber;
    }
    
    if (currentTime - g_lastSpawnTime >= wave->spawnDelay) {
        g_lastSpawnTime = currentTime;
        return true;
    }
    
    return false;
}

// Get next enemy type to spawn
EnemyType GetNextEnemyType(Stage* stage) {
    if (stage->currentWave >= stage->waveCount) return ENEMY_TYPE_BASIC;
    
    EnemyWave* wave = &stage->waves[stage->currentWave];
    
    // Calculate which enemy in the wave we're spawning
    int spawnedBefore = 0;
    for (int i = 0; i < stage->currentWave; i++) {
        spawnedBefore += stage->waves[i].enemyCount;
    }
    
    int indexInWave = stage->totalEnemiesSpawned - spawnedBefore;
    if (indexInWave >= 0 && indexInWave < wave->enemyCount) {
        return wave->enemyTypes[indexInWave];
    }
    
    return ENEMY_TYPE_BASIC;
}

// Get spawn position
Vector2 GetEnemySpawnPosition(Stage* stage, int screenWidth, int screenHeight) {
    if (stage->currentWave >= stage->waveCount) {
        return (Vector2){GetRandomValue(50, screenWidth-50), GetRandomValue(50, screenHeight-50)};
    }
    
    EnemyWave* wave = &stage->waves[stage->currentWave];
    
    if (!wave->useRandomSpawn) {
        // Use predefined spawn pattern
        int spawnedBefore = 0;
        for (int i = 0; i < stage->currentWave; i++) {
            spawnedBefore += stage->waves[i].enemyCount;
        }
        
        int indexInWave = stage->totalEnemiesSpawned - spawnedBefore;
        if (indexInWave >= 0 && indexInWave < wave->enemyCount) {
            return wave->spawnPattern[indexInWave];
        }
    }
    
    // Random spawn position
    return (Vector2){GetRandomValue(50, screenWidth-50), GetRandomValue(50, screenHeight-50)};
}

// Check if stage is complete
bool IsStageComplete(Stage* stage) {
    return stage->enemiesKilled >= stage->targetKills;
}

// Check if current wave is complete
bool IsWaveComplete(Stage* stage) {
    if (stage->currentWave >= stage->waveCount) return true;
    
    int totalInPreviousWaves = 0;
    for (int i = 0; i < stage->currentWave; i++) {
        totalInPreviousWaves += stage->waves[i].enemyCount;
    }
    
    int spawnedInCurrentWave = stage->totalEnemiesSpawned - totalInPreviousWaves;
    return spawnedInCurrentWave >= stage->waves[stage->currentWave].enemyCount;
}

// Start next wave
void StartNextWave(Stage* stage) {
    if (stage->currentWave < stage->waveCount - 1) {
        stage->currentWave++;
    }
}

// Draw stage intro
void DrawStageIntro(Stage* stage, int screenWidth, int screenHeight) {
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
    
    char stageText[32];
    sprintf(stageText, "STAGE %d", stage->stageNumber);
    int fontSize = 48;
    int textWidth = MeasureText(stageText, fontSize);
    DrawText(stageText, screenWidth/2 - textWidth/2, screenHeight/2 - 100, fontSize, WHITE);
    
    fontSize = 32;
    textWidth = MeasureText(stage->name, fontSize);
    DrawText(stage->name, screenWidth/2 - textWidth/2, screenHeight/2 - 40, fontSize, YELLOW);
    
    fontSize = 20;
    textWidth = MeasureText(stage->description, fontSize);
    DrawText(stage->description, screenWidth/2 - textWidth/2, screenHeight/2 + 20, fontSize, LIGHTGRAY);
}

// Draw stage progress
void DrawStageProgress(Stage* stage, int screenWidth) {
    char progressText[64];
    sprintf(progressText, "Stage %d - Enemies: %d/%d", stage->stageNumber, stage->enemiesKilled, stage->targetKills);
    DrawText(progressText, screenWidth - 250, 10, 18, BLACK);
    
    // Draw progress bar
    int barWidth = 200;
    int barHeight = 10;
    int barX = screenWidth - 220;
    int barY = 35;
    
    DrawRectangle(barX, barY, barWidth, barHeight, LIGHTGRAY);
    float progress = (float)stage->enemiesKilled / stage->targetKills;
    DrawRectangle(barX, barY, (int)(barWidth * progress), barHeight, GREEN);
    DrawRectangleLines(barX, barY, barWidth, barHeight, DARKGRAY);
}

// Draw boss warning
void DrawBossWarning(int screenWidth, int screenHeight) {
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, 0.3f));
    
    const char* warningText = "! BOSS INCOMING !";
    int fontSize = 64;
    int textWidth = MeasureText(warningText, fontSize);
    
    // Flashing effect
    Color textColor = ((int)(GetTime() * 4) % 2 == 0) ? WHITE : RED;
    DrawText(warningText, screenWidth/2 - textWidth/2, screenHeight/2 - 32, fontSize, textColor);
}

// Draw stage complete
void DrawStageComplete(Stage* stage, int screenWidth, int screenHeight) {
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
    
    const char* completeText = "STAGE COMPLETE!";
    int fontSize = 48;
    int textWidth = MeasureText(completeText, fontSize);
    DrawText(completeText, screenWidth/2 - textWidth/2, screenHeight/2 - 50, fontSize, GOLD);
    
    char scoreText[64];
    sprintf(scoreText, "Enemies Defeated: %d", stage->enemiesKilled);
    fontSize = 24;
    textWidth = MeasureText(scoreText, fontSize);
    DrawText(scoreText, screenWidth/2 - textWidth/2, screenHeight/2 + 20, fontSize, WHITE);
}

// Initialize stage manager
void InitStageManager(void) {
    // Any global stage manager initialization can go here
}

// Transition to next stage
void TransitionStageData(Stage* currentStage, Stage* nextStage) {
    // Special handling for boss stages
    if (nextStage->stageNumber == 6 || nextStage->stageNumber == 10) {
        nextStage->state = STAGE_STATE_BOSS_WARNING;
    } else {
        nextStage->state = STAGE_STATE_INTRO;
    }
    nextStage->stateTimer = 0.0f;
    nextStage->waveTimer = 0.0f;
    nextStage->currentWave = 0;
    nextStage->enemiesKilled = 0;
    nextStage->totalEnemiesSpawned = 0;
} 