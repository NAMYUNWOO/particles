#ifndef GRAVITY_SYSTEM_H
#define GRAVITY_SYSTEM_H

#include "raylib.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// Maximum concurrent gravity sources (performance vs feature tradeoff)
#define MAX_GRAVITY_SOURCES 32

// Gravity types (bitflags for future combinations)
typedef enum {
    GRAVITY_TYPE_NONE       = 0,
    GRAVITY_TYPE_ATTRACTION = 1 << 0,  // Pulls targets towards source (BLACKHOLE)
    GRAVITY_TYPE_REPULSION  = 1 << 1,  // Pushes targets away (REPULSOR)
    GRAVITY_TYPE_ORBITAL    = 1 << 2,  // Future: Orbital mechanics
    GRAVITY_TYPE_DIRECTIONAL= 1 << 3,  // Future: Wind, conveyor belts
} GravityType;

// Gravity source (what creates gravity)
typedef struct {
    Vector2 position;      // Gravity center point
    float radius;          // Effective range (pixels)
    float strength;        // Force multiplier (base force * strength)
    GravityType type;      // Type of gravity
    bool active;           // Is this source currently active?
    void* sourcePtr;       // Pointer to source object (Enemy*, etc) - for reference only
    int sourceType;        // Source type: 0=enemy, 1=environment, 2=player_skill, 3=item
    int sourceId;          // Unique ID for source (for removal)
} GravitySource;

// Gravity target (what receives gravity)
typedef struct {
    Vector2* position;     // Pointer to target's position
    Vector2* velocity;     // Pointer to target's velocity (modified by gravity)
    float mass;            // Mass factor (1.0 = normal, 0.5 = half effect, 2.0 = double)
    bool affectedByGravity; // Can be disabled per-object
} GravityTarget;

// System lifecycle
void InitGravitySystem(void);
void CleanupGravitySystem(void);

// Source management
int RegisterGravitySource(GravitySource source);    // Returns unique ID
void UnregisterGravitySource(int sourceId);
void UpdateGravitySource(int sourceId, Vector2 newPosition); // For moving sources
void SetGravitySourceActive(int sourceId, bool active);

// Main update function (call once per frame)
void ApplyAllGravitySources(void* gamePtr, float deltaTime);

// Helper functions (inline for performance)
static inline Vector2 CalculateGravityForce(Vector2 targetPos, GravitySource source) {
    float dx = source.position.x - targetPos.x;
    float dy = source.position.y - targetPos.y;
    float dist = sqrtf(dx*dx + dy*dy);

    // Avoid division by zero
    if (dist < 1.0f) dist = 1.0f;

    // Normalize direction
    Vector2 direction = {dx / dist, dy / dist};

    // Calculate force based on type
    float force = 0.0f;

    if (source.type & GRAVITY_TYPE_ATTRACTION) {
        // Linear falloff (matches current BLACKHOLE implementation)
        force = source.strength * (1.0f - dist / source.radius);
        // Keep direction (pulls towards source)
    } else if (source.type & GRAVITY_TYPE_REPULSION) {
        // Same falloff but inverted direction
        force = source.strength * (1.0f - dist / source.radius);
        direction.x = -direction.x;  // Flip direction (pushes away)
        direction.y = -direction.y;
    }

    // Clamp force to prevent extreme values
    if (force < 0.0f) force = 0.0f;
    if (force > source.strength * 2.0f) force = source.strength * 2.0f;

    return (Vector2){direction.x * force, direction.y * force};
}

static inline bool IsInGravityRange(Vector2 targetPos, GravitySource source) {
    float dx = source.position.x - targetPos.x;
    float dy = source.position.y - targetPos.y;
    float distSquared = dx*dx + dy*dy;
    float radiusSquared = source.radius * source.radius;
    return distSquared < radiusSquared && distSquared > 1.0f; // Exclude center point
}

// Debug/visualization
void DrawGravityFields(bool showLabels); // For test mode (G key)
int GetActiveGravitySourceCount(void);

#endif // GRAVITY_SYSTEM_H
