#include "dev_test_mode.h"
#include "game.h"
#include <raymath.h>
#include <string.h>
#include <stdio.h>

// Enemy type names for UI display
static const char* ENEMY_TYPE_NAMES[] = {
    "BASIC",        // ENEMY_TYPE_BASIC
    "TRACKER",      // ENEMY_TYPE_TRACKER
    "SPEEDY",       // ENEMY_TYPE_SPEEDY
    "SPLITTER",     // ENEMY_TYPE_SPLITTER
    "ORBITER",      // ENEMY_TYPE_ORBITER
    "BOSS_1",       // ENEMY_TYPE_BOSS_1
    "TELEPORTER",   // ENEMY_TYPE_TELEPORTER
    "REPULSOR",     // ENEMY_TYPE_REPULSOR
    "CLUSTER",      // ENEMY_TYPE_CLUSTER
    "BOSS_FINAL",   // ENEMY_TYPE_BOSS_FINAL
    "BLACKHOLE"     // ENEMY_TYPE_BLACKHOLE
};

// Keyboard shortcuts for enemy selection (1-9, 0 for BLACKHOLE)
static const int ENEMY_SELECTION_KEYS[] = {
    KEY_ONE,    // ENEMY_TYPE_BASIC
    KEY_TWO,    // ENEMY_TYPE_TRACKER
    KEY_THREE,  // ENEMY_TYPE_SPEEDY
    KEY_FOUR,   // ENEMY_TYPE_SPLITTER
    KEY_FIVE,   // ENEMY_TYPE_ORBITER
    KEY_SIX,    // ENEMY_TYPE_BOSS_1
    KEY_SEVEN,  // ENEMY_TYPE_TELEPORTER
    KEY_EIGHT,  // ENEMY_TYPE_REPULSOR
    KEY_NINE,   // ENEMY_TYPE_CLUSTER
    KEY_ZERO,   // ENEMY_TYPE_BLACKHOLE (0 = 10th)
};

/**
 * Initialize test mode
 */
TestModeState InitTestMode(void) {
    TestModeState state = {
        .selectedEnemyType = ENEMY_TYPE_BASIC,
        .showHelp = true,  // Show help on startup
        .enemiesSpawned = 0,
        .enemiesRemoved = 0
    };
    return state;
}

/**
 * Get enemy type name as string
 */
const char* GetEnemyTypeName(EnemyType type) {
    if (type >= 0 && type < ENEMY_TYPE_COUNT) {
        return ENEMY_TYPE_NAMES[type];
    }
    return "UNKNOWN";
}

/**
 * Handle keyboard shortcuts for test mode
 */
void HandleTestModeKeyboard(TestModeState* state, void* gamePtr) {
    Game* game = (Game*)gamePtr;

    // Toggle help overlay with F1
    if (IsKeyPressed(KEY_F1)) {
        state->showHelp = !state->showHelp;
    }

    // Clear all enemies with C
    if (IsKeyPressed(KEY_C)) {
        game->enemyCount = 0;
        state->enemiesRemoved = state->enemiesSpawned;
    }

    // Remove nearest enemy with R
    if (IsKeyPressed(KEY_R)) {
        Vector2 mousePos = GetMousePosition();
        if (RemoveNearestEnemy(game, mousePos)) {
            state->enemiesRemoved++;
        }
    }

    // Cycle through enemy types with TAB/Shift+TAB
    if (IsKeyPressed(KEY_TAB)) {
        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            // Shift+TAB: Previous enemy type (with wrapping)
            state->selectedEnemyType = (EnemyType)((state->selectedEnemyType - 1 + ENEMY_TYPE_COUNT) % ENEMY_TYPE_COUNT);
        } else {
            // TAB: Next enemy type (with wrapping)
            state->selectedEnemyType = (EnemyType)((state->selectedEnemyType + 1) % ENEMY_TYPE_COUNT);
        }
    }

    // Enemy type selection (1-9, 0) - Quick access to first 10
    for (int i = 0; i < ENEMY_TYPE_COUNT && i < 10; i++) {
        if (IsKeyPressed(ENEMY_SELECTION_KEYS[i])) {
            state->selectedEnemyType = (EnemyType)i;
        }
    }

    // Special case: 0 key selects enemy at index 9 (10th enemy) if it exists
    if (IsKeyPressed(KEY_ZERO) && ENEMY_TYPE_COUNT > 9) {
        state->selectedEnemyType = (EnemyType)9;
    }
}

/**
 * Handle mouse input for enemy spawning
 */
void HandleTestModeMouseInput(TestModeState* state, void* gamePtr) {
    Game* game = (Game*)gamePtr;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();

        // Check if we can spawn more enemies
        if (game->enemyCount >= MAX_ENEMIES) {
            return;  // Max enemies reached
        }

        // Spawn enemy at mouse position
        Enemy newEnemy = InitEnemyByType(
            state->selectedEnemyType,
            game->screenWidth,
            game->screenHeight,
            game->player.position
        );

        // Override spawn position with mouse position
        newEnemy.position = mousePos;

        // Add to game's enemy array
        game->enemies[game->enemyCount] = newEnemy;
        game->enemyCount++;
        state->enemiesSpawned++;
    }
}

/**
 * Remove nearest enemy to mouse cursor
 */
bool RemoveNearestEnemy(void* gamePtr, Vector2 mousePos) {
    Game* game = (Game*)gamePtr;

    if (game->enemyCount == 0) return false;

    // Find nearest enemy
    int nearestIndex = -1;
    float nearestDistance = 999999.0f;

    for (int i = 0; i < game->enemyCount; i++) {
        float distance = Vector2Distance(game->enemies[i].position, mousePos);
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestIndex = i;
        }
    }

    if (nearestIndex >= 0) {
        // Remove enemy by shifting array
        for (int i = nearestIndex; i < game->enemyCount - 1; i++) {
            game->enemies[i] = game->enemies[i + 1];
        }
        game->enemyCount--;
        return true;
    }

    return false;
}

/**
 * Update test mode logic
 */
void UpdateTestMode(TestModeState* state, void* gamePtr) {
    HandleTestModeKeyboard(state, gamePtr);
    HandleTestModeMouseInput(state, gamePtr);
}

/**
 * Draw test mode UI
 */
void DrawTestModeUI(TestModeState* state, int screenWidth, int screenHeight) {
    // Draw enemy selector panel (top-left)
    int panelX = 10;
    int panelY = 100;
    int panelWidth = 250;
    int panelHeight = 80;

    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Fade(BLACK, 0.7f));
    DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, YELLOW);

    // Selected enemy type
    DrawText("SELECTED ENEMY:", panelX + 10, panelY + 10, 16, WHITE);
    const char* typeName = GetEnemyTypeName(state->selectedEnemyType);
    DrawText(typeName, panelX + 10, panelY + 30, 20, YELLOW);

    // Stats
    char statsText[128];
    sprintf(statsText, "Spawned: %d  Removed: %d", state->enemiesSpawned, state->enemiesRemoved);
    DrawText(statsText, panelX + 10, panelY + 55, 14, LIGHTGRAY);

    // Draw help overlay if enabled
    if (state->showHelp) {
        int helpX = screenWidth - 330;
        int helpY = 10;
        int helpWidth = 320;
        int helpHeight = 280;

        DrawRectangle(helpX, helpY, helpWidth, helpHeight, Fade(BLACK, 0.8f));
        DrawRectangleLines(helpX, helpY, helpWidth, helpHeight, GREEN);

        DrawText("TEST MODE CONTROLS", helpX + 10, helpY + 10, 16, GREEN);
        DrawText("F1: Toggle Help", helpX + 10, helpY + 35, 14, WHITE);
        DrawText("TAB: Next Enemy Type", helpX + 10, helpY + 55, 14, YELLOW);
        DrawText("Shift+TAB: Previous Enemy", helpX + 10, helpY + 75, 14, YELLOW);
        DrawText("1-9,0: Quick Select (1st-10th)", helpX + 10, helpY + 95, 14, WHITE);
        DrawText("Left Click: Spawn Enemy", helpX + 10, helpY + 115, 14, WHITE);
        DrawText("R: Remove Nearest Enemy", helpX + 10, helpY + 135, 14, WHITE);
        DrawText("C: Clear All Enemies", helpX + 10, helpY + 155, 14, WHITE);
        DrawText("ESC: Exit Test Mode", helpX + 10, helpY + 175, 14, WHITE);
        DrawText("", helpX + 10, helpY + 195, 14, WHITE);
        DrawText("QUICK SELECT:", helpX + 10, helpY + 210, 14, YELLOW);
        DrawText("1=BASIC  2=TRACKER  3=SPEEDY", helpX + 10, helpY + 230, 12, LIGHTGRAY);
        DrawText("4=SPLIT  5=ORBIT   6=BOSS", helpX + 10, helpY + 245, 12, LIGHTGRAY);
        DrawText("7=TELE   8=REPULSE 9=CLUSTER", helpX + 10, helpY + 260, 12, LIGHTGRAY);
    }
}
