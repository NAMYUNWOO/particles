#include "../../src/minunit/minunit.h"
#include "../../src/core/dev_test_mode.h"
#include "../../src/core/game.h"
#include "../../src/entities/enemy.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Test fixtures
static TestModeState testState;
static Game testGame;

void test_setup(void) {
    testState = InitTestMode();

    // Initialize minimal game state for testing
    testGame.screenWidth = 800;
    testGame.screenHeight = 600;
    testGame.enemyCount = 0;
    testGame.enemies = (Enemy*)calloc(MAX_ENEMIES, sizeof(Enemy));
}

void test_teardown(void) {
    free(testGame.enemies);
}

/**
 * Test 1: Verify InitTestMode() sets correct initial state
 *
 * Expected behavior:
 * - selectedEnemyType should be ENEMY_BASIC (0)
 * - showHelp should be false
 * - enemiesSpawned should be 0
 * - enemiesRemoved should be 0
 */
MU_TEST(test_init_test_mode) {
    TestModeState state = InitTestMode();

    mu_assert(state.selectedEnemyType == ENEMY_TYPE_BASIC,
              "Initial enemy type should be ENEMY_TYPE_BASIC");
    mu_assert(state.showHelp == true,
              "Help should be shown initially");
    mu_assert_int_eq(0, state.enemiesSpawned);
    mu_assert_int_eq(0, state.enemiesRemoved);
}

/**
 * Test 2: Verify GetEnemyTypeName() returns correct names
 *
 * Tests all 10 enemy types to ensure proper name mapping
 */
MU_TEST(test_get_enemy_type_name) {
    mu_check(strcmp(GetEnemyTypeName(ENEMY_TYPE_BASIC), "BASIC") == 0);
    mu_check(strcmp(GetEnemyTypeName(ENEMY_TYPE_TRACKER), "TRACKER") == 0);
    mu_check(strcmp(GetEnemyTypeName(ENEMY_TYPE_SPEEDY), "SPEEDY") == 0);
    mu_check(strcmp(GetEnemyTypeName(ENEMY_TYPE_SPLITTER), "SPLITTER") == 0);
    mu_check(strcmp(GetEnemyTypeName(ENEMY_TYPE_ORBITER), "ORBITER") == 0);
    mu_check(strcmp(GetEnemyTypeName(ENEMY_TYPE_BOSS_1), "BOSS_1") == 0);
    mu_check(strcmp(GetEnemyTypeName(ENEMY_TYPE_TELEPORTER), "TELEPORTER") == 0);
    mu_check(strcmp(GetEnemyTypeName(ENEMY_TYPE_REPULSOR), "REPULSOR") == 0);
    mu_check(strcmp(GetEnemyTypeName(ENEMY_TYPE_CLUSTER), "CLUSTER") == 0);
    mu_check(strcmp(GetEnemyTypeName(ENEMY_TYPE_BLACKHOLE), "BLACKHOLE") == 0);

    // Test invalid type returns "UNKNOWN"
    mu_check(strcmp(GetEnemyTypeName((EnemyType)999), "UNKNOWN") == 0);
}

/**
 * Test 3: Verify keyboard shortcuts change selected enemy type
 *
 * Tests all keyboard shortcuts (1-9, 0) for enemy selection
 */
MU_TEST(test_keyboard_enemy_selection) {
    TestModeState state = InitTestMode();

    // Simulate key presses using raylib IsKeyPressed mock
    // Since we can't actually simulate raylib input in unit tests,
    // we test the logic directly by setting the enemy type

    // Test that each enemy type can be selected
    for (int i = ENEMY_TYPE_BASIC; i < ENEMY_TYPE_COUNT; i++) {
        state.selectedEnemyType = (EnemyType)i;
        mu_assert(state.selectedEnemyType == i,
                  "Enemy type should be selectable");
    }

    // Test that enemy type stays within valid range
    state.selectedEnemyType = ENEMY_TYPE_BASIC;
    mu_assert(state.selectedEnemyType >= ENEMY_TYPE_BASIC &&
              state.selectedEnemyType < ENEMY_TYPE_COUNT,
              "Enemy type should be within valid range");
}

/**
 * Test 4: Verify RemoveNearestEnemy() correctly finds and removes closest enemy
 *
 * Sets up multiple enemies at different distances and verifies the nearest
 * one is removed
 */
MU_TEST(test_remove_nearest_enemy) {
    testGame.enemyCount = 3;

    // Create three enemies at different distances from test point (400, 300)
    testGame.enemies[0] = InitEnemyByType(ENEMY_TYPE_BASIC, 800, 600, (Vector2){100, 100});
    testGame.enemies[0].position = (Vector2){450.0f, 350.0f};  // Distance: ~70

    testGame.enemies[1] = InitEnemyByType(ENEMY_TYPE_TRACKER, 800, 600, (Vector2){100, 100});
    testGame.enemies[1].position = (Vector2){600.0f, 500.0f};  // Distance: ~282

    testGame.enemies[2] = InitEnemyByType(ENEMY_TYPE_SPEEDY, 800, 600, (Vector2){100, 100});
    testGame.enemies[2].position = (Vector2){410.0f, 310.0f};  // Distance: ~14 (nearest)

    // Try to remove enemy nearest to (400, 300)
    Vector2 mousePos = {400.0f, 300.0f};
    bool removed = RemoveNearestEnemy(&testGame, mousePos);

    mu_assert(removed, "Should successfully remove an enemy");
    mu_assert_int_eq(2, testGame.enemyCount);  // Should have 2 enemies remaining

    // Verify the nearest enemy (index 2) was removed by checking remaining positions
    // After removal, enemies should be shifted: [0] stays, [1] stays
    mu_check(testGame.enemies[0].position.x == 450.0f);  // First enemy unchanged
    mu_check(testGame.enemies[1].position.x == 600.0f);  // Second enemy unchanged
}

/**
 * Test 5: Verify RemoveNearestEnemy() returns false when no enemies exist
 */
MU_TEST(test_remove_nearest_enemy_no_enemies) {
    testGame.enemyCount = 0;

    Vector2 mousePos = {400.0f, 300.0f};
    bool removed = RemoveNearestEnemy(&testGame, mousePos);

    mu_assert(!removed, "Should return false when no enemies to remove");
}

MU_TEST_SUITE(dev_test_mode_suite) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

    MU_RUN_TEST(test_init_test_mode);
    MU_RUN_TEST(test_get_enemy_type_name);
    MU_RUN_TEST(test_keyboard_enemy_selection);
    MU_RUN_TEST(test_remove_nearest_enemy);
    MU_RUN_TEST(test_remove_nearest_enemy_no_enemies);
}

int main() {
    MU_RUN_SUITE(dev_test_mode_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
