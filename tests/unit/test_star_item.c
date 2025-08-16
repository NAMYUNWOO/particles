#include "../../src/minunit/minunit.h"
#include "../../src/entities/items/star_item.h"
#include "../../src/entities/managers/item_manager.h"
#include "../../src/entities/player.h"
#include "../../src/core/event/event_system.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

static Player testPlayer;

void test_setup(void) {
    srand(42); // Fixed seed for reproducible tests
    InitEventSystem();
    InitItemManager();
    testPlayer = InitPlayer(400, 300);
}

void test_teardown(void) {
    CleanupItemManager();
    CleanupEventSystem();
}

MU_TEST(test_star_init) {
    StarItem star = InitStarItem();
    mu_assert(!star.isActive, "Star should start inactive");
    mu_assert_double_eq(STAR_BASE_SIZE, star.radius);
    mu_assert_double_eq(0.0f, star.colorTimer);
}

MU_TEST(test_star_spawn_position) {
    StarItem star = InitStarItem();
    SpawnStarItem(&star, 800, 600);
    
    mu_assert(star.isActive, "Star should be active after spawn");
    
    // Check spawn is off-screen (stars spawn AT the margin, not beyond it)
    float margin = STAR_BASE_SIZE * 2;
    bool offScreen = (star.position.x <= -margin || star.position.x >= 800 + margin ||
                      star.position.y <= -margin || star.position.y >= 600 + margin);
    mu_assert(offScreen, "Star should spawn off-screen");
    
    // Check velocity is non-zero
    mu_assert(star.velocity.x != 0 || star.velocity.y != 0, "Star should have velocity");
    
    // Check velocity magnitude (with small tolerance for floating point)
    float speed = sqrtf(star.velocity.x * star.velocity.x + star.velocity.y * star.velocity.y);
    float tolerance = 0.001f;
    mu_assert(fabs(speed - STAR_SPEED) < tolerance, "Star speed should be approximately STAR_SPEED");
}

MU_TEST(test_star_movement) {
    StarItem star = InitStarItem();
    star.position = (Vector2){100, 300};  // Start in bounds
    star.velocity = (Vector2){100, 0};
    star.isActive = true;
    
    float oldX = star.position.x;
    UpdateStarItem(&star, 0.1f, 800, 600);
    
    mu_assert_double_eq(oldX + 10.0f, star.position.x); // 100 * 0.1
    mu_assert(star.isActive, "Star should still be active");
}

MU_TEST(test_star_out_of_bounds) {
    StarItem star = InitStarItem();
    star.position = (Vector2){850, 300};
    star.velocity = (Vector2){100, 0};
    star.isActive = true;
    
    UpdateStarItem(&star, 1.0f, 800, 600); // Move far out
    mu_assert(!star.isActive, "Star should deactivate when out of bounds");
}

MU_TEST(test_star_collision) {
    StarItem star = InitStarItem();
    star.position = (Vector2){100, 100};
    star.isActive = true;
    
    // Test collision
    testPlayer.position = (Vector2){100, 100};
    bool collided = CheckStarPlayerCollision(&star, testPlayer.position, testPlayer.size);
    mu_assert(collided, "Should detect collision at same position");
    
    // Test no collision
    testPlayer.position = (Vector2){200, 200};
    collided = CheckStarPlayerCollision(&star, testPlayer.position, testPlayer.size);
    mu_assert(!collided, "Should not detect collision at distance");
}

MU_TEST(test_star_invincibility_trigger) {
    // Manually spawn star
    g_itemManager.starItem.position = (Vector2){100, 100};
    g_itemManager.starItem.isActive = true;
    
    testPlayer.position = (Vector2){100, 100};
    testPlayer.isInvincible = false;
    testPlayer.invincibleTimer = 0.0f;
    
    CheckItemCollisions(&testPlayer);
    
    mu_assert(testPlayer.isInvincible, "Player should become invincible");
    mu_assert_double_eq(STAR_INVINCIBILITY_DURATION, testPlayer.invincibleTimer);
    mu_assert(!g_itemManager.starItem.isActive, "Star should be collected");
}

MU_TEST(test_rainbow_color) {
    // Test color cycling
    Color color1 = GetRainbowColor(0.0f);
    Color color2 = GetRainbowColor(0.5f);
    
    // Colors should be different
    mu_assert(color1.r != color2.r || color1.g != color2.g || color1.b != color2.b,
              "Colors should change over time");
    
    // Should cycle back
    Color color4 = GetRainbowColor(1.0f);
    Color color5 = GetRainbowColor(0.0f);
    mu_assert(color4.r == color5.r && color4.g == color5.g && color4.b == color5.b,
              "Colors should cycle");
}

MU_TEST(test_star_spawn_interval) {
    InitItemManager();
    
    // Test minimum interval
    g_itemManager.starSpawnTimer = 0.0f;
    mu_assert(g_itemManager.nextStarSpawnInterval >= STAR_SPAWN_MIN_INTERVAL,
              "Spawn interval should be at least minimum");
    mu_assert(g_itemManager.nextStarSpawnInterval <= STAR_SPAWN_MAX_INTERVAL,
              "Spawn interval should not exceed maximum");
}

MU_TEST_SUITE(test_suite_star_item) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    
    MU_RUN_TEST(test_star_init);
    MU_RUN_TEST(test_star_spawn_position);
    MU_RUN_TEST(test_star_movement);
    MU_RUN_TEST(test_star_out_of_bounds);
    MU_RUN_TEST(test_star_collision);
    MU_RUN_TEST(test_star_invincibility_trigger);
    MU_RUN_TEST(test_rainbow_color);
    MU_RUN_TEST(test_star_spawn_interval);
}

int main(int argc, char *argv[]) {
    MU_RUN_SUITE(test_suite_star_item);
    MU_REPORT();
    return MU_EXIT_CODE;
}