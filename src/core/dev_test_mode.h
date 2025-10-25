#ifndef DEV_TEST_MODE_H
#define DEV_TEST_MODE_H

#include "raylib.h"
#include "../entities/enemy.h"

/**
 * @brief Test mode state
 */
typedef struct {
    EnemyType selectedEnemyType;  // Currently selected enemy type for spawning
    bool showHelp;                // Whether to show help overlay
    int enemiesSpawned;           // Total enemies spawned this session
    int enemiesRemoved;           // Total enemies removed this session
} TestModeState;

/**
 * @brief Initialize test mode
 * @return TestModeState Initial test mode state
 */
TestModeState InitTestMode(void);

/**
 * @brief Update test mode logic
 * @param state Test mode state
 * @param game Game instance
 */
void UpdateTestMode(TestModeState* state, void* game);  // void* to avoid circular dependency

/**
 * @brief Draw test mode UI
 * @param state Test mode state
 * @param screenWidth Screen width
 * @param screenHeight Screen height
 */
void DrawTestModeUI(TestModeState* state, int screenWidth, int screenHeight);

/**
 * @brief Handle mouse input for enemy spawning
 * @param state Test mode state
 * @param game Game instance
 */
void HandleTestModeMouseInput(TestModeState* state, void* game);

/**
 * @brief Handle keyboard shortcuts for test mode
 * @param state Test mode state
 * @param game Game instance
 */
void HandleTestModeKeyboard(TestModeState* state, void* game);

/**
 * @brief Remove nearest enemy to mouse cursor
 * @param game Game instance
 * @param mousePos Mouse position
 * @return true if enemy was removed
 */
bool RemoveNearestEnemy(void* game, Vector2 mousePos);

/**
 * @brief Get enemy type name as string
 * @param type Enemy type
 * @return const char* Enemy type name
 */
const char* GetEnemyTypeName(EnemyType type);

#endif // DEV_TEST_MODE_H
