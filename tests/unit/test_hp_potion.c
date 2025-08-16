#include "../../src/minunit/minunit.h"
#include "../../src/entities/items/hp_potion.h"
#include "../../src/core/memory_pool.h"

// Stub for testing - define uninitialized pool
MemoryPool g_itemEventPool = {0};

static HPPotion testPotion;

void test_setup(void) {
    testPotion = InitHPPotion();
}

void test_teardown(void) {
    // Nothing to clean up
}

MU_TEST(test_hp_potion_init) {
    mu_assert(!testPotion.isActive, "Potion should start inactive");
    mu_assert_double_eq(HPPOTION_BASE_SIZE, testPotion.radius);
    mu_assert_double_eq(0.0f, testPotion.lifetime);
}

MU_TEST(test_hp_potion_spawn) {
    SpawnHPPotion(&testPotion, 800, 600);
    
    mu_assert(testPotion.isActive, "Potion should be active after spawn");
    mu_assert_double_eq(POTION_LIFETIME, testPotion.lifetime);
    mu_assert(testPotion.position.x >= HPPOTION_BASE_SIZE * 2, "X position in bounds");
    mu_assert(testPotion.position.x <= 800 - HPPOTION_BASE_SIZE * 2, "X position in bounds");
    mu_assert(testPotion.position.y >= HPPOTION_BASE_SIZE * 2, "Y position in bounds");
    mu_assert(testPotion.position.y <= 600 - HPPOTION_BASE_SIZE * 2, "Y position in bounds");
}

MU_TEST(test_hp_potion_update_lifetime) {
    SpawnHPPotion(&testPotion, 800, 600);
    float initialLifetime = testPotion.lifetime;
    
    UpdateHPPotion(&testPotion, 1.0f);
    
    mu_assert_double_eq(initialLifetime - 1.0f, testPotion.lifetime);
    mu_assert(testPotion.isActive, "Potion still active");
}

MU_TEST(test_hp_potion_blink_phase) {
    SpawnHPPotion(&testPotion, 800, 600);
    
    // Fast forward to blink phase
    UpdateHPPotion(&testPotion, POTION_LIFETIME - POTION_BLINK_TIME + 0.1f);
    
    mu_assert(testPotion.isBlinking, "Potion should be blinking");
    mu_assert(testPotion.isActive, "Potion still active while blinking");
}

MU_TEST(test_hp_potion_expire) {
    SpawnHPPotion(&testPotion, 800, 600);
    
    // Fast forward past lifetime
    UpdateHPPotion(&testPotion, POTION_LIFETIME + 0.1f);
    
    mu_assert(!testPotion.isActive, "Potion should be inactive after expiring");
}

MU_TEST(test_hp_potion_collision) {
    // Manually set potion position for testing
    testPotion.position = (Vector2){400.0f, 300.0f};
    testPotion.radius = HPPOTION_BASE_SIZE;
    testPotion.isActive = true;
    
    // Test collision with player at same position
    Vector2 playerPos = {400.0f, 300.0f};
    float playerRadius = 10.0f;
    
    bool collides = CheckPotionPlayerCollision(testPotion, playerPos, playerRadius);
    mu_assert(collides, "Should detect collision at same position");
    
    // Test no collision when far away
    playerPos.x = 500.0f;
    playerPos.y = 400.0f;
    collides = CheckPotionPlayerCollision(testPotion, playerPos, playerRadius);
    mu_assert(!collides, "Should not detect collision when far away");
}

MU_TEST_SUITE(hp_potion_suite) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    
    MU_RUN_TEST(test_hp_potion_init);
    MU_RUN_TEST(test_hp_potion_spawn);
    MU_RUN_TEST(test_hp_potion_update_lifetime);
    MU_RUN_TEST(test_hp_potion_blink_phase);
    MU_RUN_TEST(test_hp_potion_expire);
    MU_RUN_TEST(test_hp_potion_collision);
}

int main() {
    MU_RUN_SUITE(hp_potion_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}