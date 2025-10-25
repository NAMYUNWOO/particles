#include "gravity_system.h"
#include "game.h"
#include "../entities/particle.h"
#include "../entities/enemy.h"
#include <string.h>
#include <stdio.h>

// Particle count (defined in game.h)
#ifndef PARTICLE_COUNT
#define PARTICLE_COUNT 100000
#endif

// Internal state
static GravitySource g_gravitySources[MAX_GRAVITY_SOURCES];
static int g_nextSourceId = 1; // Start at 1 (0 = invalid)
static int g_activeSourceCount = 0;

void InitGravitySystem(void) {
    memset(g_gravitySources, 0, sizeof(g_gravitySources));
    g_nextSourceId = 1;
    g_activeSourceCount = 0;
}

void CleanupGravitySystem(void) {
    // Nothing to free (static arrays)
    g_activeSourceCount = 0;
}

int RegisterGravitySource(GravitySource source) {
    // Find empty slot
    for (int i = 0; i < MAX_GRAVITY_SOURCES; i++) {
        if (g_gravitySources[i].sourceId == 0) { // Empty slot
            source.sourceId = g_nextSourceId++;
            g_gravitySources[i] = source;
            g_activeSourceCount++;
            return source.sourceId;
        }
    }

    // No slots available
    fprintf(stderr, "WARNING: Max gravity sources (%d) reached!\n", MAX_GRAVITY_SOURCES);
    return 0; // Invalid ID
}

void UnregisterGravitySource(int sourceId) {
    for (int i = 0; i < MAX_GRAVITY_SOURCES; i++) {
        if (g_gravitySources[i].sourceId == sourceId) {
            memset(&g_gravitySources[i], 0, sizeof(GravitySource));
            g_activeSourceCount--;
            return;
        }
    }
}

void UpdateGravitySource(int sourceId, Vector2 newPosition) {
    for (int i = 0; i < MAX_GRAVITY_SOURCES; i++) {
        if (g_gravitySources[i].sourceId == sourceId) {
            g_gravitySources[i].position = newPosition;
            return;
        }
    }
}

void SetGravitySourceActive(int sourceId, bool active) {
    for (int i = 0; i < MAX_GRAVITY_SOURCES; i++) {
        if (g_gravitySources[i].sourceId == sourceId) {
            g_gravitySources[i].active = active;
            return;
        }
    }
}

void ApplyAllGravitySources(void* gamePtr, float deltaTime) {
    Game* game = (Game*)gamePtr;

    // Early exit if no gravity sources
    if (g_activeSourceCount == 0) return;

    // Apply gravity to all particles
    for (int p = 0; p < PARTICLE_COUNT; p++) {
        // Skip if particle shouldn't be affected (future feature)
        // if (!game->particles[p].affectedByGravity) continue;

        Vector2 totalForce = {0, 0};

        // Accumulate forces from all active sources
        for (int s = 0; s < MAX_GRAVITY_SOURCES; s++) {
            if (g_gravitySources[s].sourceId == 0) continue; // Empty slot
            if (!g_gravitySources[s].active) continue;

            // Quick range check
            if (!IsInGravityRange(game->particles[p].position, g_gravitySources[s])) {
                continue;
            }

            // Calculate and accumulate force
            Vector2 force = CalculateGravityForce(game->particles[p].position, g_gravitySources[s]);
            totalForce.x += force.x;
            totalForce.y += force.y;
        }

        // Apply accumulated force to velocity
        game->particles[p].velocity.x += totalForce.x;
        game->particles[p].velocity.y += totalForce.y;
    }

    // TODO Phase 4: Apply gravity to enemies
    // TODO Phase 5: Apply gravity to player
    // TODO Phase 5: Apply gravity to items
}

void DrawGravityFields(bool showLabels) {
    for (int i = 0; i < MAX_GRAVITY_SOURCES; i++) {
        if (g_gravitySources[i].sourceId == 0) continue;
        if (!g_gravitySources[i].active) continue;

        GravitySource* src = &g_gravitySources[i];

        // Choose color based on type
        Color fieldColor = BLUE;
        if (src->type & GRAVITY_TYPE_ATTRACTION) {
            fieldColor = Fade(PURPLE, 0.3f); // Purple for attraction
        } else if (src->type & GRAVITY_TYPE_REPULSION) {
            fieldColor = Fade(ORANGE, 0.3f); // Orange for repulsion
        }

        // Draw influence radius
        DrawCircleLines((int)src->position.x, (int)src->position.y, src->radius, fieldColor);
        DrawCircleV(src->position, 5.0f, fieldColor);

        // Draw label if requested
        if (showLabels) {
            const char* typeStr = (src->type & GRAVITY_TYPE_ATTRACTION) ? "ATT" : "REP";
            DrawText(TextFormat("%s %.0f", typeStr, src->strength),
                    (int)src->position.x + 10, (int)src->position.y - 10, 12, WHITE);
        }
    }
}

int GetActiveGravitySourceCount(void) {
    return g_activeSourceCount;
}
