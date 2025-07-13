#include "../../src/minunit/minunit.h"
#include "../../src/entities/managers/item_manager.h"
#include "../../src/entities/player.h"
#include "../../src/core/event/event_system.h"

static Player testPlayer;

void test_setup(void) {
    InitEventSystem();
    InitItemManager();
    testPlayer = InitPlayer(800, 600);
}

void test_teardown(void) {
    CleanupItemManager();
    CleanupEventSystem();
}

MU_TEST(test_item_manager_init) {
    mu_assert(g_itemManager.initialized, "Item manager should be initialized");
    mu_assert(!g_itemManager.hpPotion.isActive, "HP potion should start inactive");
    mu_assert_double_eq(0.0f, g_itemManager.potionSpawnTimer);
}

MU_TEST(test_item_manager_spawn_timer) {
    // Update for less than spawn interval
    UpdateItemManager(10.0f, 800, 600);
    mu_assert_double_eq(10.0f, g_itemManager.potionSpawnTimer);
    mu_assert(!g_itemManager.hpPotion.isActive, "Potion should not spawn yet");
    
    // Update to reach spawn interval
    UpdateItemManager(15.0f, 800, 600);
    mu_assert(g_itemManager.hpPotion.isActive, "Potion should spawn after 25 seconds");
    mu_assert_double_eq(0.0f, g_itemManager.potionSpawnTimer);
}

MU_TEST(test_item_collision_with_low_health) {
    // Spawn a potion manually
    SpawnHPPotion(&g_itemManager.hpPotion, 800, 600);
    
    // Set player position to potion position and reduce health
    testPlayer.position = g_itemManager.hpPotion.position;
    testPlayer.health = 1;
    
    // Check collision
    CheckItemCollisions(&testPlayer);
    
    mu_assert_int_eq(3, testPlayer.health);
    mu_assert(!g_itemManager.hpPotion.isActive, "Potion should be collected");
}

MU_TEST(test_item_collision_with_full_health) {
    // Spawn a potion manually
    SpawnHPPotion(&g_itemManager.hpPotion, 800, 600);
    
    // Set player position to potion position with full health
    testPlayer.position = g_itemManager.hpPotion.position;
    testPlayer.health = 3;
    
    // Check collision
    CheckItemCollisions(&testPlayer);
    
    mu_assert_int_eq(3, testPlayer.health);
    mu_assert(g_itemManager.hpPotion.isActive, "Potion should not be collected at full health");
}

MU_TEST(test_only_one_potion_at_time) {
    // Update to spawn first potion
    UpdateItemManager(25.0f, 800, 600);
    mu_assert(g_itemManager.hpPotion.isActive, "First potion should spawn");
    
    // Reset timer and update again
    g_itemManager.potionSpawnTimer = 0.0f;
    UpdateItemManager(25.0f, 800, 600);
    
    // Should still only have one potion
    mu_assert(g_itemManager.hpPotion.isActive, "Potion still active");
    mu_assert_double_eq(0.0f, g_itemManager.potionSpawnTimer);
}

MU_TEST_SUITE(item_manager_suite) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    
    MU_RUN_TEST(test_item_manager_init);
    MU_RUN_TEST(test_item_manager_spawn_timer);
    MU_RUN_TEST(test_item_collision_with_low_health);
    MU_RUN_TEST(test_item_collision_with_full_health);
    MU_RUN_TEST(test_only_one_potion_at_time);
}

int main() {
    MU_RUN_SUITE(item_manager_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}