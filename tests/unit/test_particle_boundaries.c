#include "../../src/minunit/minunit.h"
#include "../../src/entities/particle.h"
#include <stdlib.h>

void test_setup(void) {
    // Initialize test environment
}

void test_teardown(void) {
    // Cleanup
}

MU_TEST(test_particle_bounce_left_boundary) {
    Particle p = {
        .position = {0, 400},
        .velocity = {-5.0f, 0},
        .color = BLACK
    };
    
    MoveParticle(&p, 800, 600);
    
    mu_assert_double_eq(0.0, p.position.x);
    mu_assert(p.velocity.x > 0, "Velocity should reverse to positive");
}

MU_TEST(test_particle_bounce_right_boundary) {
    Particle p = {
        .position = {799, 400},
        .velocity = {5.0f, 0},
        .color = BLACK
    };
    
    MoveParticle(&p, 800, 600);
    
    mu_assert_double_eq(799.0, p.position.x);
    mu_assert(p.velocity.x < 0, "Velocity should reverse to negative");
}

MU_TEST(test_particle_bounce_top_boundary) {
    Particle p = {
        .position = {400, 0},
        .velocity = {0, -5.0f},
        .color = BLACK
    };
    
    MoveParticle(&p, 800, 600);
    
    mu_assert_double_eq(0.0, p.position.y);
    mu_assert(p.velocity.y > 0, "Velocity should reverse to positive");
}

MU_TEST(test_particle_bounce_bottom_boundary) {
    Particle p = {
        .position = {400, 599},
        .velocity = {0, 5.0f},
        .color = BLACK
    };
    
    MoveParticle(&p, 800, 600);
    
    mu_assert_double_eq(599.0, p.position.y);
    mu_assert(p.velocity.y < 0, "Velocity should reverse to negative");
}

MU_TEST(test_particle_bounce_corner) {
    Particle p = {
        .position = {0, 0},
        .velocity = {-5.0f, -5.0f},
        .color = BLACK
    };
    
    MoveParticle(&p, 800, 600);
    
    mu_assert_double_eq(0.0, p.position.x);
    mu_assert_double_eq(0.0, p.position.y);
    mu_assert(p.velocity.x > 0, "X velocity should be positive");
    mu_assert(p.velocity.y > 0, "Y velocity should be positive");
}

MU_TEST(test_particle_stays_in_bounds) {
    // Test 100 random movements
    for (int i = 0; i < 100; i++) {
        // Create a particle at random position with random velocity
        Particle p = {
            .position = {(float)(rand() % 800), (float)(rand() % 600)},
            .velocity = {(rand() % 200 - 100) / 10.0f, (rand() % 200 - 100) / 10.0f},
            .color = BLACK
        };
        
        // Move particle 100 times
        for (int j = 0; j < 100; j++) {
            MoveParticle(&p, 800, 600);
            
            // Assert particle stays in bounds
            mu_assert(p.position.x >= 0 && p.position.x < 800, 
                     "X position out of bounds");
            mu_assert(p.position.y >= 0 && p.position.y < 600, 
                     "Y position out of bounds");
        }
    }
}

MU_TEST(test_particle_no_wrapping) {
    // Test that particles don't wrap around
    Particle p1 = {
        .position = {-10, 300},  // Left of screen
        .velocity = {0, 0},
        .color = BLACK
    };
    
    MoveParticle(&p1, 800, 600);
    mu_assert_double_eq(0.0, p1.position.x);  // Should be clamped at 0, not wrapped
    
    Particle p2 = {
        .position = {810, 300},  // Right of screen
        .velocity = {0, 0},
        .color = BLACK
    };
    
    MoveParticle(&p2, 800, 600);
    mu_assert_double_eq(799.0, p2.position.x);  // Should be clamped at 799, not wrapped
    
    Particle p3 = {
        .position = {400, -10},  // Above screen
        .velocity = {0, 0},
        .color = BLACK
    };
    
    MoveParticle(&p3, 800, 600);
    mu_assert_double_eq(0.0, p3.position.y);  // Should be clamped at 0, not wrapped
    
    Particle p4 = {
        .position = {400, 610},  // Below screen
        .velocity = {0, 0},
        .color = BLACK
    };
    
    MoveParticle(&p4, 800, 600);
    mu_assert_double_eq(599.0, p4.position.y);  // Should be clamped at 599, not wrapped
}

MU_TEST_SUITE(particle_boundary_suite) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    
    MU_RUN_TEST(test_particle_bounce_left_boundary);
    MU_RUN_TEST(test_particle_bounce_right_boundary);
    MU_RUN_TEST(test_particle_bounce_top_boundary);
    MU_RUN_TEST(test_particle_bounce_bottom_boundary);
    MU_RUN_TEST(test_particle_bounce_corner);
    MU_RUN_TEST(test_particle_stays_in_bounds);
    MU_RUN_TEST(test_particle_no_wrapping);
}

int main(int argc, char *argv[]) {
    MU_RUN_SUITE(particle_boundary_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}