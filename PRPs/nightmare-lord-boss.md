# PRP: Nightmare Lord Boss Implementation

## Goal
Implement the Nightmare Lord ultimate boss enemy with three-phase combat system, adaptive mechanics, and unique abilities that create the most challenging encounter in the game.

## Why
- **Ultimate Challenge**: Provides end-game content for players who have mastered all 10 stages
- **Player Retention**: Complex boss mechanics encourage multiple attempts and skill improvement
- **Showcase Feature**: Demonstrates all game systems working together at maximum complexity
- **Achievement Goal**: Creates a memorable finale for dedicated players

## What
The Nightmare Lord is an 8x-sized boss with 2000 HP that evolves through three distinct phases, each with unique abilities and increasing difficulty. Features health regeneration, adaptive defense, fear aura, and a dramatic "second wind" resurrection mechanic.

### Success Criteria
- [ ] Boss spawns with correct size (8x normal enemy) and 2000 HP
- [ ] Three distinct phases trigger at 66% and 33% HP thresholds
- [ ] All 9 unique abilities function correctly across phases
- [ ] Adaptive defense system responds to player attack patterns
- [ ] Fear aura affects particles within 150-pixel radius
- [ ] Second wind resurrection triggers on first death
- [ ] Visual effects clearly communicate boss state and abilities
- [ ] Performance maintains 60 FPS with 100,000 particles

## All Needed Context

### Documentation & References
```yaml
# MUST READ - Include these in your context window
- file: src/entities/enemy.h
  why: Enemy structure, types, movement patterns, AI states
  
- file: src/entities/enemy.c
  why: Boss implementation patterns (BOSS_1, BOSS_FINAL), phase system
  
- file: src/entities/managers/enemy_manager.c
  why: Enemy spawning and management patterns
  
- file: src/core/event/event_types.h
  why: Event system for ability triggers and phase transitions
  
- file: src/entities/managers/stage_manager.h
  why: Integration with stage system for boss spawn
  
- file: boss_nightmare_design.md
  why: Complete design specification with all abilities
  
- url: https://www.raylib.com/cheatsheet/cheatsheet.html#shapes
  why: Drawing functions for visual effects (circles, gradients)
  
- url: https://github.com/raysan5/raylib/wiki/Working-with-Colors
  why: Color manipulation for phase transitions and effects
```

### Current Codebase Tree
```bash
src/
├── entities/
│   ├── enemy.h           # Enemy types and structures
│   ├── enemy.c           # Enemy behaviors
│   ├── player.h          # Player structure
│   └── managers/
│       ├── enemy_manager.h
│       └── enemy_manager.c
├── core/
│   ├── event/
│   │   ├── event_system.h
│   │   └── event_types.h
│   ├── physics.h         # Collision detection
│   └── memory_pool.h     # Memory management
└── minunit/
    └── minunit.h         # Unit testing framework
```

### Desired Codebase Tree with Files to be Added
```bash
src/
├── entities/
│   ├── enemy.h           # Update with ENEMY_TYPE_NIGHTMARE
│   ├── enemy.c           # Add Nightmare Lord behaviors
│   ├── bosses/           # NEW directory
│   │   ├── nightmare_lord.h    # Boss-specific data structures
│   │   └── nightmare_lord.c    # Boss ability implementations
│   └── managers/
│       └── enemy_manager.c     # Update spawn logic
tests/
└── unit/
    └── test_nightmare_lord.c   # Boss unit tests
```

## Common Integration Challenges

### External Dependencies
- **Memory Pools**: Boss abilities need event pools for particle manipulation
- **Global Variables**: Access to particle manager for theft/manipulation abilities
- **Cross-Module Dependencies**: Physics system for black hole effects

### Making Code Testable
- Separate ability logic into testable functions
- Use dependency injection for particle/player references
- Create mock event system for unit tests
- Example test stub:
```c
// In test file
typedef struct {
    Vector2 position;
    float radius;
    Vector2 velocity;
} MockParticle;

MockParticle g_testParticles[1000];
int g_testParticleCount = 0;
```

### Testing Considerations
- Phase transitions must be deterministic for testing
- Ability cooldowns need time injection for tests
- Visual effects should be optional (test flag)
- Performance tests with full particle count

# Implementation Blueprint

## Phase 1: Core Boss Structure

### Step 1: Update Enemy Types
**File: `src/entities/enemy.h`**
```c
// Add to EnemyType enum (before ENEMY_TYPE_COUNT)
ENEMY_TYPE_NIGHTMARE,      // Ultimate boss with 3 phases

// Add nightmare-specific constants
#define NIGHTMARE_BASE_SIZE (ENEMY_MAX_SIZE * 8.0f)
#define NIGHTMARE_MAX_HEALTH 2000.0f
#define NIGHTMARE_REGEN_RATE 5.0f
#define NIGHTMARE_REGEN_DELAY 3.0f
#define FEAR_AURA_RADIUS 150.0f
#define ADAPTIVE_DEFENSE_THRESHOLD 100.0f
```

### Step 2: Create Nightmare Lord Header
**File: `src/entities/bosses/nightmare_lord.h`**
```c
#ifndef NIGHTMARE_LORD_H
#define NIGHTMARE_LORD_H

#include "raylib.h"
#include "../enemy.h"
#include <stdbool.h>

// Phase definitions
typedef enum {
    NIGHTMARE_PHASE_1_SHADOW = 0,    // 100% - 66% HP
    NIGHTMARE_PHASE_2_CHAOS = 1,     // 66% - 33% HP
    NIGHTMARE_PHASE_3_DESPERATION = 2 // 33% - 0% HP
} NightmarePhase;

// Ability types
typedef enum {
    ABILITY_SHADOW_CLONES,
    ABILITY_PHASE_SHIFT,
    ABILITY_DARK_WAVE,
    ABILITY_CHAOS_VORTEXES,
    ABILITY_PARTICLE_THEFT,
    ABILITY_TELEPORT_BARRAGE,
    ABILITY_BERSERK_MODE,
    ABILITY_DEATH_SPIRAL,
    ABILITY_FINAL_GAMBIT
} NightmareAbility;

// Vortex structure for Chaos phase
typedef struct {
    Vector2 position;
    float radius;
    float strength;
    float lifetime;
    bool isPulling;  // Alternates between pull/push
} ChaosVortex;

// Clone structure for Shadow phase
typedef struct {
    Enemy enemy;
    bool isActive;
    int parentId;
} ShadowClone;

// Nightmare Lord specific data
typedef struct {
    // Phase management
    NightmarePhase currentPhase;
    float phaseTransitionTimer;
    bool isTransitioning;
    
    // Health regeneration
    float regenCooldown;
    float lastDamageTime;
    
    // Adaptive defense
    float damageFromDirection[8];  // 8 directional segments
    float defenseMultiplier[8];
    
    // Shadow Form abilities (Phase 1)
    ShadowClone clones[3];
    int activeCloneCount;
    float phaseShiftTimer;
    bool isPhased;
    float darkWaveTimer;
    float darkWaveAngle;
    
    // Chaos Storm abilities (Phase 2)
    ChaosVortex vortexes[5];
    int activeVortexCount;
    float vortexSpawnTimer;
    float particleTheftTimer;
    float stolenParticleRadius;
    Vector2* stolenParticles;
    int stolenParticleCount;
    float teleportBarrageTimer;
    int teleportCount;
    Vector2 teleportPattern[6];
    
    // Final Desperation abilities (Phase 3)
    bool isBerserk;
    float berserkTimer;
    Vector2 deathSpiralCenter;
    float deathSpiralStrength;
    float deathSpiralTimer;
    bool finalGambitActive;
    float finalGambitTimer;
    float explosionRadius;
    
    // Visual effects
    float rainbowTimer;
    float shadowPulseTimer;
    float screenShakeIntensity;
    Color phaseColor;
    
    // Second Wind
    bool hasUsedSecondWind;
    bool isResurrecting;
    float resurrectionTimer;
    
    // Fear Aura
    float fearAuraPulseTimer;
    
} NightmareLordData;

// Initialization
Enemy InitNightmareLord(Vector2 position);
NightmareLordData* InitNightmareLordData(void);

// Update functions
void UpdateNightmareLord(Enemy* enemy, Vector2 playerPos, float deltaTime);
void UpdateNightmarePhase(Enemy* enemy, NightmareLordData* data);
void UpdateNightmareAbilities(Enemy* enemy, NightmareLordData* data, Vector2 playerPos, float deltaTime);

// Ability implementations
void ExecuteShadowClones(Enemy* enemy, NightmareLordData* data);
void ExecutePhaseShift(Enemy* enemy, NightmareLordData* data, float deltaTime);
void ExecuteDarkWave(Enemy* enemy, NightmareLordData* data, float deltaTime);
void ExecuteChaosVortexes(Enemy* enemy, NightmareLordData* data, float deltaTime);
void ExecuteParticleTheft(Enemy* enemy, NightmareLordData* data, Vector2 playerPos);
void ExecuteTeleportBarrage(Enemy* enemy, NightmareLordData* data, float deltaTime);
void ExecuteBerserkMode(Enemy* enemy, NightmareLordData* data);
void ExecuteDeathSpiral(Enemy* enemy, NightmareLordData* data, Vector2 playerPos, float deltaTime);
void ExecuteFinalGambit(Enemy* enemy, NightmareLordData* data, float deltaTime);

// Special mechanics
void UpdateAdaptiveDefense(Enemy* enemy, NightmareLordData* data, Vector2 damageSource, float damage);
void ApplyFearAura(Enemy* enemy, NightmareLordData* data, void* particles, int particleCount);
void TriggerSecondWind(Enemy* enemy, NightmareLordData* data);

// Rendering
void DrawNightmareLord(Enemy enemy, NightmareLordData* data);
void DrawNightmareEffects(Enemy enemy, NightmareLordData* data);
void DrawShadowClones(NightmareLordData* data);
void DrawChaosVortexes(NightmareLordData* data);

// Utility functions
float CalculateDirectionSegment(Vector2 from, Vector2 to);
Color GetNightmarePhaseColor(NightmarePhase phase, float timer);
bool IsNightmareLordDefeated(Enemy* enemy, NightmareLordData* data);

#endif // NIGHTMARE_LORD_H
```

### Step 3: Implement Core Nightmare Lord
**File: `src/entities/bosses/nightmare_lord.c`**
```c
#include "nightmare_lord.h"
#include "../../core/event/event_system.h"
#include "../../core/event/event_types.h"
#include "../../core/memory_pool.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

// External dependencies
extern MemoryPool g_enemyEventPool;

Enemy InitNightmareLord(Vector2 position) {
    Enemy boss = {0};
    
    // Basic enemy properties
    boss.type = ENEMY_TYPE_NIGHTMARE;
    boss.position = position;
    boss.radius = NIGHTMARE_BASE_SIZE;
    boss.maxHealth = NIGHTMARE_MAX_HEALTH;
    boss.health = NIGHTMARE_MAX_HEALTH;
    boss.damage = 50.0f;  // High contact damage
    boss.movePattern = MOVE_PATTERN_AGGRESSIVE;
    boss.aiState = AI_STATE_IDLE;
    boss.color = BLACK;
    boss.originalColor = BLACK;
    boss.phase = 0;  // Start in phase 1
    boss.velocity = (Vector2){0, 0};
    
    // Allocate and initialize Nightmare-specific data
    NightmareLordData* data = InitNightmareLordData();
    boss.customData = data;
    
    return boss;
}

NightmareLordData* InitNightmareLordData(void) {
    NightmareLordData* data = (NightmareLordData*)calloc(1, sizeof(NightmareLordData));
    if (!data) return NULL;
    
    // Initialize phase data
    data->currentPhase = NIGHTMARE_PHASE_1_SHADOW;
    data->phaseTransitionTimer = 0.0f;
    data->isTransitioning = false;
    
    // Initialize adaptive defense (no resistance initially)
    for (int i = 0; i < 8; i++) {
        data->damageFromDirection[i] = 0.0f;
        data->defenseMultiplier[i] = 1.0f;
    }
    
    // Initialize visual effects
    data->phaseColor = BLACK;
    data->screenShakeIntensity = 0.0f;
    
    // Allocate stolen particles array
    data->stolenParticles = (Vector2*)calloc(1000, sizeof(Vector2));
    
    return data;
}

void UpdateNightmareLord(Enemy* enemy, Vector2 playerPos, float deltaTime) {
    if (!enemy || enemy->type != ENEMY_TYPE_NIGHTMARE) return;
    
    NightmareLordData* data = (NightmareLordData*)enemy->customData;
    if (!data) return;
    
    // Update phase transitions
    UpdateNightmarePhase(enemy, data);
    
    // Health regeneration
    if (GetTime() - data->lastDamageTime > NIGHTMARE_REGEN_DELAY && 
        enemy->health < enemy->maxHealth && !data->isTransitioning) {
        enemy->health += NIGHTMARE_REGEN_RATE * deltaTime;
        if (enemy->health > enemy->maxHealth) {
            enemy->health = enemy->maxHealth;
        }
    }
    
    // Update abilities based on current phase
    if (!data->isTransitioning) {
        UpdateNightmareAbilities(enemy, data, playerPos, deltaTime);
    }
    
    // Update visual timers
    data->rainbowTimer += deltaTime;
    data->shadowPulseTimer += deltaTime;
    data->fearAuraPulseTimer += deltaTime;
    
    // Update movement (base movement handled by enemy system)
    if (data->currentPhase == NIGHTMARE_PHASE_3_DESPERATION && data->isBerserk) {
        // Triple speed in berserk mode
        enemy->velocity.x *= 3.0f;
        enemy->velocity.y *= 3.0f;
    }
    
    // Check for death and second wind
    if (enemy->health <= 0 && !data->hasUsedSecondWind) {
        TriggerSecondWind(enemy, data);
    }
}

void UpdateNightmarePhase(Enemy* enemy, NightmareLordData* data) {
    float healthPercent = enemy->health / enemy->maxHealth;
    
    // Check for phase transitions
    if (healthPercent <= 0.66f && data->currentPhase == NIGHTMARE_PHASE_1_SHADOW) {
        data->currentPhase = NIGHTMARE_PHASE_2_CHAOS;
        data->isTransitioning = true;
        data->phaseTransitionTimer = 0.0f;
        enemy->isInvulnerable = true;
        
        // Publish phase change event
        if (g_enemyEventPool.blocks != NULL) {
            EnemyEventData* eventData = MemoryPool_Alloc(&g_enemyEventPool);
            if (eventData) {
                eventData->enemyId = enemy->id;
                eventData->enemyType = enemy->type;
                eventData->position = enemy->position;
                eventData->customValue = 2;  // Phase 2
                PublishEvent(EVENT_BOSS_PHASE_CHANGE, eventData);
            }
        }
    } else if (healthPercent <= 0.33f && data->currentPhase == NIGHTMARE_PHASE_2_CHAOS) {
        data->currentPhase = NIGHTMARE_PHASE_3_DESPERATION;
        data->isTransitioning = true;
        data->phaseTransitionTimer = 0.0f;
        enemy->isInvulnerable = true;
        
        // Activate berserk mode
        ExecuteBerserkMode(enemy, data);
    }
    
    // Handle transition period
    if (data->isTransitioning) {
        data->phaseTransitionTimer += GetFrameTime();
        if (data->phaseTransitionTimer >= 2.0f) {
            data->isTransitioning = false;
            enemy->isInvulnerable = false;
            
            // Update colors based on phase
            switch (data->currentPhase) {
                case NIGHTMARE_PHASE_1_SHADOW:
                    enemy->color = BLACK;
                    break;
                case NIGHTMARE_PHASE_2_CHAOS:
                    enemy->color = DARKPURPLE;
                    break;
                case NIGHTMARE_PHASE_3_DESPERATION:
                    enemy->color = MAROON;
                    break;
            }
        }
    }
}

void UpdateNightmareAbilities(Enemy* enemy, NightmareLordData* data, Vector2 playerPos, float deltaTime) {
    switch (data->currentPhase) {
        case NIGHTMARE_PHASE_1_SHADOW:
            // Shadow clones
            if (data->activeCloneCount == 0 && enemy->health < enemy->maxHealth * 0.9f) {
                ExecuteShadowClones(enemy, data);
            }
            
            // Phase shift
            ExecutePhaseShift(enemy, data, deltaTime);
            
            // Dark wave
            data->darkWaveTimer += deltaTime;
            if (data->darkWaveTimer >= 5.0f) {
                ExecuteDarkWave(enemy, data, deltaTime);
                data->darkWaveTimer = 0.0f;
            }
            break;
            
        case NIGHTMARE_PHASE_2_CHAOS:
            // Chaos vortexes
            ExecuteChaosVortexes(enemy, data, deltaTime);
            
            // Particle theft
            data->particleTheftTimer += deltaTime;
            if (data->particleTheftTimer >= 8.0f) {
                ExecuteParticleTheft(enemy, data, playerPos);
                data->particleTheftTimer = 0.0f;
            }
            
            // Teleport barrage
            data->teleportBarrageTimer += deltaTime;
            if (data->teleportBarrageTimer >= 10.0f) {
                ExecuteTeleportBarrage(enemy, data, deltaTime);
            }
            break;
            
        case NIGHTMARE_PHASE_3_DESPERATION:
            // Death spiral
            if (enemy->health < enemy->maxHealth * 0.15f && data->deathSpiralTimer == 0.0f) {
                data->deathSpiralCenter = enemy->position;
                data->deathSpiralStrength = 50.0f;
            }
            if (data->deathSpiralStrength > 0) {
                ExecuteDeathSpiral(enemy, data, playerPos, deltaTime);
            }
            
            // Final gambit
            if (enemy->health < enemy->maxHealth * 0.05f && !data->finalGambitActive) {
                data->finalGambitActive = true;
                data->finalGambitTimer = 0.0f;
                enemy->isInvulnerable = true;
            }
            if (data->finalGambitActive) {
                ExecuteFinalGambit(enemy, data, deltaTime);
            }
            break;
    }
}

// Ability implementations would follow...
// (Continuing with key abilities as examples)

void ExecuteShadowClones(Enemy* enemy, NightmareLordData* data) {
    // Spawn 3 clones around the boss
    float angleStep = (2 * PI) / 3.0f;
    
    for (int i = 0; i < 3; i++) {
        if (!data->clones[i].isActive) {
            float angle = i * angleStep;
            Vector2 clonePos = {
                enemy->position.x + cosf(angle) * 150.0f,
                enemy->position.y + sinf(angle) * 150.0f
            };
            
            // Initialize clone as basic enemy
            data->clones[i].enemy = InitEnemyByType(ENEMY_TYPE_BASIC, 800, 600, enemy->position);
            data->clones[i].enemy.position = clonePos;
            data->clones[i].enemy.radius = enemy->radius * 0.5f;
            data->clones[i].enemy.maxHealth = 200.0f;
            data->clones[i].enemy.health = 200.0f;
            data->clones[i].enemy.color = Fade(BLACK, 0.7f);
            data->clones[i].enemy.damage = enemy->damage * 0.5f;
            data->clones[i].isActive = true;
            data->clones[i].parentId = enemy->id;
            
            data->activeCloneCount++;
        }
    }
}

void ExecutePhaseShift(Enemy* enemy, NightmareLordData* data, float deltaTime) {
    data->phaseShiftTimer += deltaTime;
    
    if (data->phaseShiftTimer >= 2.0f) {
        data->isPhased = !data->isPhased;
        data->phaseShiftTimer = 0.0f;
        
        if (data->isPhased) {
            // Become semi-transparent and take reduced damage
            enemy->color = Fade(enemy->originalColor, 0.5f);
            // Damage reduction handled in damage function
        } else {
            enemy->color = enemy->originalColor;
        }
    }
}

void ExecuteFinalGambit(Enemy* enemy, NightmareLordData* data, float deltaTime) {
    data->finalGambitTimer += deltaTime;
    
    // Visual countdown effect
    float glowIntensity = data->finalGambitTimer / 5.0f;
    enemy->color = (Color){
        255,
        (unsigned char)(255 * glowIntensity),
        (unsigned char)(255 * glowIntensity),
        255
    };
    
    // Explosion at 5 seconds
    if (data->finalGambitTimer >= 5.0f) {
        // Publish explosion event
        if (g_enemyEventPool.blocks != NULL) {
            EnemyEventData* eventData = MemoryPool_Alloc(&g_enemyEventPool);
            if (eventData) {
                eventData->enemyId = enemy->id;
                eventData->position = enemy->position;
                eventData->customValue = 1000.0f;  // Explosion radius
                PublishEvent(EVENT_BOSS_FINAL_GAMBIT, eventData);
            }
        }
        
        // Boss dies after explosion
        enemy->health = 0;
        data->hasUsedSecondWind = true;  // Prevent resurrection
    }
}

void TriggerSecondWind(Enemy* enemy, NightmareLordData* data) {
    data->hasUsedSecondWind = true;
    data->isResurrecting = true;
    data->resurrectionTimer = 0.0f;
    enemy->health = 0.1f;  // Keep barely alive during resurrection
    enemy->isInvulnerable = true;
    
    // Dramatic resurrection after 3 seconds
    // This would be handled in the update loop
}

void UpdateAdaptiveDefense(Enemy* enemy, NightmareLordData* data, Vector2 damageSource, float damage) {
    // Calculate which direction the damage came from
    float segment = CalculateDirectionSegment(enemy->position, damageSource);
    int segmentIndex = (int)segment;
    
    // Track damage from this direction
    data->damageFromDirection[segmentIndex] += damage;
    
    // Update defense multiplier if threshold reached
    if (data->damageFromDirection[segmentIndex] >= ADAPTIVE_DEFENSE_THRESHOLD) {
        data->defenseMultiplier[segmentIndex] = 0.75f;  // 25% damage reduction
        data->damageFromDirection[segmentIndex] = 0.0f;  // Reset counter
    }
}

float CalculateDirectionSegment(Vector2 from, Vector2 to) {
    Vector2 direction = {to.x - from.x, to.y - from.y};
    float angle = atan2f(direction.y, direction.x);
    if (angle < 0) angle += 2 * PI;
    
    // Convert to 0-7 segment
    return (angle / (2 * PI)) * 8.0f;
}

// Cleanup function
void CleanupNightmareLordData(NightmareLordData* data) {
    if (data) {
        if (data->stolenParticles) {
            free(data->stolenParticles);
        }
        free(data);
    }
}
```

## Phase 2: Integration with Enemy System

### Step 4: Update Enemy System
**Modify `src/entities/enemy.c`**:
```c
// In InitEnemyByType(), add case for Nightmare Lord
case ENEMY_TYPE_NIGHTMARE:
    enemy = InitNightmareLord((Vector2){screenWidth/2, screenHeight/2});
    break;

// In UpdateEnemyAI(), add Nightmare Lord handling
case ENEMY_TYPE_NIGHTMARE:
    UpdateNightmareLord(enemy, playerPos, deltaTime);
    break;

// In DrawEnemy(), add special rendering
if (enemy.type == ENEMY_TYPE_NIGHTMARE) {
    NightmareLordData* data = (NightmareLordData*)enemy.customData;
    if (data) {
        DrawNightmareLord(enemy, data);
        DrawNightmareEffects(enemy, data);
        return;  // Skip normal enemy drawing
    }
}

// In DamageEnemy(), add adaptive defense
if (enemy->type == ENEMY_TYPE_NIGHTMARE) {
    NightmareLordData* data = (NightmareLordData*)enemy->customData;
    if (data) {
        // Apply phase shift reduction
        if (data->isPhased) {
            damage *= 0.5f;
        }
        
        // Apply adaptive defense
        float segment = CalculateDirectionSegment(enemy->position, damageSource);
        int segmentIndex = (int)segment;
        damage *= data->defenseMultiplier[segmentIndex];
        
        // Update adaptive defense
        UpdateAdaptiveDefense(enemy, data, damageSource, damage);
        
        // Update last damage time for regen
        data->lastDamageTime = GetTime();
    }
}
```

## Phase 3: Visual Effects Implementation

### Step 5: Implement Rendering Functions
**Add to `src/entities/bosses/nightmare_lord.c`**:
```c
void DrawNightmareLord(Enemy enemy, NightmareLordData* data) {
    // Base boss rendering with phase-specific effects
    Color baseColor = enemy.color;
    
    // Add pulsing shadow effect
    float shadowScale = 1.0f + sinf(data->shadowPulseTimer * 2.0f) * 0.1f;
    DrawCircle(enemy.position.x, enemy.position.y, 
               enemy.radius * shadowScale * 1.2f, 
               Fade(BLACK, 0.3f));
    
    // Draw main body with phase effects
    if (data->currentPhase == NIGHTMARE_PHASE_3_DESPERATION && data->isBerserk) {
        // Berserk trail effect
        for (int i = 0; i < 5; i++) {
            float alpha = 0.5f - (i * 0.1f);
            Vector2 trailPos = {
                enemy.position.x - enemy.velocity.x * i * 0.1f,
                enemy.position.y - enemy.velocity.y * i * 0.1f
            };
            DrawCircle(trailPos.x, trailPos.y, enemy.radius, Fade(RED, alpha));
        }
    }
    
    // Main body
    DrawCircle(enemy.position.x, enemy.position.y, enemy.radius, baseColor);
    
    // Red energy tendrils
    for (int i = 0; i < 8; i++) {
        float angle = (i * PI / 4) + data->rainbowTimer;
        float tendrilLength = enemy.radius * 1.5f + sinf(data->rainbowTimer * 3 + i) * 20;
        Vector2 tendrilEnd = {
            enemy.position.x + cosf(angle) * tendrilLength,
            enemy.position.y + sinf(angle) * tendrilLength
        };
        DrawLineEx(enemy.position, tendrilEnd, 3.0f, Fade(RED, 0.7f));
    }
    
    // Fear aura visualization
    float auraAlpha = 0.2f + sinf(data->fearAuraPulseTimer * 3.0f) * 0.1f;
    DrawCircleLines(enemy.position.x, enemy.position.y, FEAR_AURA_RADIUS, Fade(PURPLE, auraAlpha));
    
    // Adaptive defense shields
    for (int i = 0; i < 8; i++) {
        if (data->defenseMultiplier[i] < 1.0f) {
            float angle = (i * PI / 4);
            float shieldDist = enemy.radius + 20;
            Vector2 shieldPos = {
                enemy.position.x + cosf(angle) * shieldDist,
                enemy.position.y + sinf(angle) * shieldDist
            };
            DrawCircle(shieldPos.x, shieldPos.y, 15, Fade(SKYBLUE, 0.5f));
        }
    }
    
    // Boss name and health bar
    const char* bossName = "NIGHTMARE LORD";
    int fontSize = 24;
    int textWidth = MeasureText(bossName, fontSize);
    DrawText(bossName, enemy.position.x - textWidth/2, 
             enemy.position.y - enemy.radius - 40, fontSize, WHITE);
    
    // Health bar
    float healthPercent = enemy.health / enemy.maxHealth;
    Rectangle healthBarBg = {
        enemy.position.x - 100, 
        enemy.position.y - enemy.radius - 25,
        200, 10
    };
    DrawRectangleRec(healthBarBg, DARKGRAY);
    
    Rectangle healthBar = healthBarBg;
    healthBar.width *= healthPercent;
    Color healthColor = (healthPercent > 0.66f) ? GREEN : 
                       (healthPercent > 0.33f) ? YELLOW : RED;
    DrawRectangleRec(healthBar, healthColor);
}

void DrawNightmareEffects(Enemy enemy, NightmareLordData* data) {
    // Draw phase-specific effects
    switch (data->currentPhase) {
        case NIGHTMARE_PHASE_1_SHADOW:
            DrawShadowClones(data);
            break;
            
        case NIGHTMARE_PHASE_2_CHAOS:
            DrawChaosVortexes(data);
            // Draw stolen particles
            for (int i = 0; i < data->stolenParticleCount; i++) {
                DrawCircle(data->stolenParticles[i].x, 
                          data->stolenParticles[i].y, 
                          2, Fade(PURPLE, 0.8f));
            }
            break;
            
        case NIGHTMARE_PHASE_3_DESPERATION:
            // Death spiral visualization
            if (data->deathSpiralStrength > 0) {
                for (int i = 0; i < 5; i++) {
                    float radius = 100 + i * 50;
                    float alpha = 0.3f - (i * 0.05f);
                    DrawCircleLines(data->deathSpiralCenter.x, 
                                   data->deathSpiralCenter.y, 
                                   radius, Fade(BLACK, alpha));
                }
            }
            
            // Final gambit countdown
            if (data->finalGambitActive) {
                int countdown = 5 - (int)data->finalGambitTimer;
                if (countdown > 0) {
                    char countText[2];
                    sprintf(countText, "%d", countdown);
                    DrawText(countText, enemy.position.x - 20, 
                            enemy.position.y - 20, 48, RED);
                }
            }
            break;
    }
}
```

## Unit Tests

**File: `tests/unit/test_nightmare_lord.c`**
```c
#include "../../src/minunit/minunit.h"
#include "../../src/entities/bosses/nightmare_lord.h"
#include "../../src/entities/enemy.h"
#include "../../src/core/event/event_system.h"
#include <math.h>

static Enemy testBoss;
static NightmareLordData* testData;

void test_setup(void) {
    InitEventSystem();
    testBoss = InitNightmareLord((Vector2){400, 300});
    testData = (NightmareLordData*)testBoss.customData;
}

void test_teardown(void) {
    CleanupNightmareLordData(testData);
    CleanupEventSystem();
}

MU_TEST(test_nightmare_init) {
    mu_assert_int_eq(ENEMY_TYPE_NIGHTMARE, testBoss.type);
    mu_assert_double_eq(NIGHTMARE_BASE_SIZE, testBoss.radius);
    mu_assert_double_eq(NIGHTMARE_MAX_HEALTH, testBoss.maxHealth);
    mu_assert_double_eq(NIGHTMARE_MAX_HEALTH, testBoss.health);
    mu_assert(testData != NULL, "Nightmare data should be initialized");
    mu_assert_int_eq(NIGHTMARE_PHASE_1_SHADOW, testData->currentPhase);
}

MU_TEST(test_phase_transitions) {
    // Test phase 1 to 2 transition
    testBoss.health = NIGHTMARE_MAX_HEALTH * 0.65f;  // Below 66%
    UpdateNightmarePhase(&testBoss, testData);
    mu_assert_int_eq(NIGHTMARE_PHASE_2_CHAOS, testData->currentPhase);
    mu_assert(testData->isTransitioning, "Should be transitioning");
    mu_assert(testBoss.isInvulnerable, "Should be invulnerable during transition");
    
    // Complete transition
    testData->phaseTransitionTimer = 3.0f;
    UpdateNightmarePhase(&testBoss, testData);
    mu_assert(!testData->isTransitioning, "Transition should be complete");
    mu_assert(!testBoss.isInvulnerable, "Should no longer be invulnerable");
    
    // Test phase 2 to 3 transition
    testBoss.health = NIGHTMARE_MAX_HEALTH * 0.32f;  // Below 33%
    UpdateNightmarePhase(&testBoss, testData);
    mu_assert_int_eq(NIGHTMARE_PHASE_3_DESPERATION, testData->currentPhase);
}

MU_TEST(test_adaptive_defense) {
    Vector2 damageSource = {500, 300};  // From the right
    
    // Initial damage (no reduction)
    float segment = CalculateDirectionSegment(testBoss.position, damageSource);
    int segmentIndex = (int)segment;
    mu_assert_double_eq(1.0f, testData->defenseMultiplier[segmentIndex]);
    
    // Apply damage until threshold
    for (int i = 0; i < 5; i++) {
        UpdateAdaptiveDefense(&testBoss, testData, damageSource, 25.0f);
    }
    
    // Check defense activated
    mu_assert_double_eq(0.75f, testData->defenseMultiplier[segmentIndex]);
}

MU_TEST(test_health_regeneration) {
    testBoss.health = NIGHTMARE_MAX_HEALTH * 0.5f;
    testData->lastDamageTime = GetTime() - 4.0f;  // 4 seconds ago
    
    float oldHealth = testBoss.health;
    UpdateNightmareLord(&testBoss, (Vector2){0, 0}, 1.0f);
    
    mu_assert(testBoss.health > oldHealth, "Health should regenerate");
    mu_assert(testBoss.health <= oldHealth + NIGHTMARE_REGEN_RATE, 
              "Regen rate should be limited");
}

MU_TEST(test_shadow_clones) {
    testBoss.health = NIGHTMARE_MAX_HEALTH * 0.85f;  // Trigger clone spawn
    ExecuteShadowClones(&testBoss, testData);
    
    mu_assert_int_eq(3, testData->activeCloneCount);
    
    // Verify clone properties
    for (int i = 0; i < 3; i++) {
        mu_assert(testData->clones[i].isActive, "Clone should be active");
        mu_assert_double_eq(200.0f, testData->clones[i].enemy.health);
        mu_assert_double_eq(testBoss.radius * 0.5f, 
                           testData->clones[i].enemy.radius);
    }
}

MU_TEST(test_second_wind) {
    testBoss.health = 0;
    testData->hasUsedSecondWind = false;
    
    UpdateNightmareLord(&testBoss, (Vector2){0, 0}, 0.016f);
    
    mu_assert(testData->hasUsedSecondWind, "Second wind should trigger");
    mu_assert(testData->isResurrecting, "Should be resurrecting");
    mu_assert(testBoss.isInvulnerable, "Should be invulnerable during resurrection");
}

MU_TEST(test_final_gambit) {
    testData->finalGambitActive = true;
    testData->finalGambitTimer = 0.0f;
    
    // Update for 4.9 seconds
    for (int i = 0; i < 294; i++) {  // 294 frames at 60fps = 4.9s
        ExecuteFinalGambit(&testBoss, testData, 0.016667f);
    }
    
    mu_assert(testBoss.health > 0, "Boss should still be alive");
    
    // Final update to trigger explosion
    ExecuteFinalGambit(&testBoss, testData, 0.2f);
    mu_assert_double_eq(0.0f, testBoss.health);
}

MU_TEST(test_direction_segments) {
    // Test 8-directional segment calculation
    Vector2 center = {400, 300};
    
    // Right (0°)
    Vector2 right = {500, 300};
    float segment = CalculateDirectionSegment(center, right);
    mu_assert_int_eq(0, (int)segment);
    
    // Down-right (45°)
    Vector2 downRight = {500, 400};
    segment = CalculateDirectionSegment(center, downRight);
    mu_assert_int_eq(1, (int)segment);
    
    // Down (90°)
    Vector2 down = {400, 400};
    segment = CalculateDirectionSegment(center, down);
    mu_assert_int_eq(2, (int)segment);
}

MU_TEST_SUITE(test_suite_nightmare_lord) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    
    MU_RUN_TEST(test_nightmare_init);
    MU_RUN_TEST(test_phase_transitions);
    MU_RUN_TEST(test_adaptive_defense);
    MU_RUN_TEST(test_health_regeneration);
    MU_RUN_TEST(test_shadow_clones);
    MU_RUN_TEST(test_second_wind);
    MU_RUN_TEST(test_final_gambit);
    MU_RUN_TEST(test_direction_segments);
}

int main(int argc, char *argv[]) {
    MU_RUN_SUITE(test_suite_nightmare_lord);
    MU_REPORT();
    return MU_EXIT_CODE;
}
```

## Validation Gates

```bash
# Create boss directory
mkdir -p src/entities/bosses

# Build validation (macOS)
clang -o particles src/*.c src/*/*.c src/*/*/*.c \
    -I/opt/homebrew/Cellar/raylib/5.5/include \
    -L/opt/homebrew/Cellar/raylib/5.5/lib \
    -lraylib -framework OpenGL -framework Cocoa -framework IOKit \
    -std=c11 -Wall -Wextra -Werror

# Syntax/Style check
cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem \
    src/entities/bosses/nightmare_lord.c \
    src/entities/enemy.c

# Unit tests
clang -o test_nightmare tests/unit/test_nightmare_lord.c \
    src/entities/bosses/nightmare_lord.c \
    src/entities/enemy.c \
    src/entities/player.c \
    src/core/physics.c \
    src/core/memory_pool.c \
    src/core/event/event_system.c \
    -I/opt/homebrew/Cellar/raylib/5.5/include \
    -L/opt/homebrew/Cellar/raylib/5.5/lib \
    -lraylib -framework OpenGL -framework Cocoa -framework IOKit \
    -std=c11 && ./test_nightmare

# Memory leak check
leaks --atExit -- ./test_nightmare

# Performance test (60 FPS with boss active)
./particles --test-mode --spawn-nightmare
```

## Implementation Tasks (In Order)

1. **Create Boss Infrastructure**
   - [x] Create `src/entities/bosses/` directory
   - [ ] Add ENEMY_TYPE_NIGHTMARE to enemy.h
   - [ ] Create nightmare_lord.h with all structures
   - [ ] Create nightmare_lord.c with core functions

2. **Implement Phase 1 Abilities**
   - [ ] Shadow clone spawning and AI
   - [ ] Phase shift mechanics
   - [ ] Dark wave attack pattern

3. **Implement Phase 2 Abilities**
   - [ ] Chaos vortex system
   - [ ] Particle theft mechanics
   - [ ] Teleport barrage pattern

4. **Implement Phase 3 Abilities**
   - [ ] Berserk mode transformation
   - [ ] Death spiral black hole
   - [ ] Final gambit countdown

5. **Implement Special Mechanics**
   - [ ] Adaptive defense system
   - [ ] Fear aura particle effects
   - [ ] Second wind resurrection

6. **Visual Polish**
   - [ ] Phase transition effects
   - [ ] Ability-specific visuals
   - [ ] Screen shake integration

7. **Integration and Testing**
   - [ ] Update Makefile
   - [ ] Run all unit tests
   - [ ] Performance testing
   - [ ] Manual gameplay testing

## Anti-Patterns to Avoid

1. **Don't hardcode screen dimensions** - Pass as parameters
2. **Don't forget null checks** - Especially for customData pointer
3. **Don't skip phase transition invulnerability** - Critical for boss feel
4. **Don't allow multiple second winds** - Use hasUsedSecondWind flag
5. **Don't spawn abilities during transitions** - Check isTransitioning
6. **Don't forget to free allocated memory** - Clean up stolen particles array
7. **Don't ignore deltaTime** - All timers and movement must use it
8. **Don't create global state** - Keep all data in NightmareLordData

## Notes

- Fear aura uses existing particle slow mechanics
- Adaptive defense visualized with shield indicators
- Phase colors: Black → Dark Purple → Maroon
- Screen shake intensity scales with proximity and abilities
- Clone explosion creates 20 particles (may need particle manager access)
- Performance critical with 100k particles - optimize collision checks

## Confidence Score: 9/10

High confidence due to:
- Clear design document with detailed specifications
- Existing boss patterns (BOSS_1, BOSS_FINAL) as reference
- Established enemy and event systems
- Modular ability system design

Point deducted for:
- Complex particle manipulation may require additional integration work