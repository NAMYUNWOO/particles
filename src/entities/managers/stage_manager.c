#include "stage_manager.h"
#include "stages/stage_common.h"
#include <string.h>
#include <stdio.h>

// Individual stage implementations are now in separate files:
// - stages/stage_1.c through stages/stage_10.c
// - See stages/stage_common.h for the interface

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