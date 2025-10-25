#include "enemy.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "raymath.h"
#include "../core/game.h"  // For global screen dimensions
#include "../core/gravity_system.h"  // Gravity system integration

static float LerpFloat(float a, float b, float t);

// Initialize enemy by type
Enemy InitEnemyByType(EnemyType type, int screenWidth, int screenHeight, Vector2 playerPos) {
    Enemy enemy = {0};
    
    // Common initialization
    enemy.type = type;
    enemy.position = (Vector2){
        GetRandomValue(100, screenWidth - 100),
        GetRandomValue(100, screenHeight - 100)
    };
    enemy.spawnTime = GetTime();
    enemy.patternTimer = 0.0f;
    enemy.specialTimer = 0.0f;
    enemy.stateData.phase = 0;
    enemy.stateData.phaseTimer = 0.0f;
    enemy.stateFlags = ENEMY_STATE_NONE;  // Initialize to no state flags
    enemy.gravitySourceId = 0;  // No gravity source initially
    
    // Type-specific initialization
    switch (type) {
        case ENEMY_TYPE_BASIC:
            enemy.radius = GetRandomValue(ENEMY_MIN_SIZE, ENEMY_MAX_SIZE);
            enemy.maxHealth = enemy.radius * 10.0f;
            enemy.movePattern = MOVE_PATTERN_RANDOM;
            enemy.aiState = AI_STATE_PATROL;
            enemy.color = PURPLE;
            // Start with a random initial velocity
            float initialSpeed = 1.0f;
            float initialAngle = GetRandomValue(0, 360) * DEG2RAD;
            enemy.velocity = (Vector2){
                cosf(initialAngle) * initialSpeed,
                sinf(initialAngle) * initialSpeed
            };
            // Initialize wander target ahead of current position
            enemy.wanderAngle = initialAngle;
            enemy.wanderTarget = (Vector2){
                enemy.position.x + cosf(enemy.wanderAngle) * 100.0f,
                enemy.position.y + sinf(enemy.wanderAngle) * 100.0f
            };
            enemy.turnSpeed = 2.0f; // Radians per second
            break;
            
        case ENEMY_TYPE_TRACKER:
            enemy.radius = GetRandomValue(ENEMY_MIN_SIZE - 2, ENEMY_MAX_SIZE - 2);
            enemy.maxHealth = enemy.radius * 12.0f;
            enemy.movePattern = MOVE_PATTERN_TRACKING;
            enemy.aiState = AI_STATE_CHASE;
            enemy.color = RED;
            enemy.velocity = (Vector2){0, 0};
            break;
            
        case ENEMY_TYPE_SPEEDY:
            enemy.radius = GetRandomValue(ENEMY_MIN_SIZE - 3, ENEMY_MIN_SIZE);
            enemy.maxHealth = enemy.radius * 8.0f;
            enemy.movePattern = MOVE_PATTERN_ZIGZAG;
            enemy.aiState = AI_STATE_PATROL;
            enemy.color = SKYBLUE;
            enemy.velocity = (Vector2){
                GetRandomValue(-100, 100) / 50.0f * SPEEDY_SPEED_MULT,
                GetRandomValue(-100, 100) / 50.0f * SPEEDY_SPEED_MULT
            };
            break;
            
        case ENEMY_TYPE_SPLITTER:
            enemy.radius = GetRandomValue(ENEMY_MAX_SIZE, ENEMY_MAX_SIZE + 5);
            enemy.maxHealth = enemy.radius * 15.0f;
            enemy.movePattern = MOVE_PATTERN_STRAIGHT;
            enemy.aiState = AI_STATE_PATROL;
            enemy.color = GREEN;
            enemy.stateData.splitCount = 2;  // Can split twice
            enemy.velocity = (Vector2){
                GetRandomValue(-30, 30) / 50.0f,
                GetRandomValue(-30, 30) / 50.0f
            };
            break;
            
        case ENEMY_TYPE_ORBITER:
            enemy.radius = GetRandomValue(ENEMY_MIN_SIZE, ENEMY_MAX_SIZE);
            enemy.maxHealth = enemy.radius * 11.0f;
            enemy.movePattern = MOVE_PATTERN_CIRCULAR;
            enemy.aiState = AI_STATE_SPECIAL;
            enemy.color = ORANGE;
            enemy.orbitCenter = enemy.position;
            enemy.orbitRadius = 100.0f;
            enemy.angle = 0.0f;
            break;
            
        case ENEMY_TYPE_BOSS_1:
            enemy.radius = ENEMY_MAX_SIZE * BOSS_SIZE_MULT;
            enemy.maxHealth = 500.0f;
            enemy.movePattern = MOVE_PATTERN_AGGRESSIVE;
            enemy.aiState = AI_STATE_ATTACK;
            enemy.color = DARKPURPLE;
            enemy.stateFlags = ENEMY_STATE_SHIELDED;  // Start with shield
            enemy.stateData.shieldHealth = 200.0f;
            enemy.stateData.phase = 0;
            enemy.stateData.phaseTimer = 0.0f;
            enemy.velocity = (Vector2){0, 0};
            break;
            
        case ENEMY_TYPE_TELEPORTER:
            enemy.radius = GetRandomValue(ENEMY_MIN_SIZE, ENEMY_MAX_SIZE - 2);
            enemy.maxHealth = enemy.radius * 10.0f;
            enemy.movePattern = MOVE_PATTERN_TELEPORT;
            enemy.aiState = AI_STATE_SPECIAL;
            enemy.color = VIOLET;
            enemy.velocity = (Vector2){0, 0};
            break;
            
        case ENEMY_TYPE_REPULSOR:
            enemy.radius = GetRandomValue(ENEMY_MAX_SIZE - 5, ENEMY_MAX_SIZE);
            enemy.maxHealth = enemy.radius * 13.0f;
            enemy.movePattern = MOVE_PATTERN_PATROL;
            enemy.aiState = AI_STATE_SPECIAL;
            enemy.color = YELLOW;
            enemy.velocity = (Vector2){
                GetRandomValue(-30, 30) / 50.0f,
                GetRandomValue(-30, 30) / 50.0f
            };
            break;
            
        case ENEMY_TYPE_CLUSTER:
            enemy.radius = GetRandomValue(ENEMY_MIN_SIZE, ENEMY_MAX_SIZE - 3);
            enemy.maxHealth = enemy.radius * 9.0f;
            enemy.movePattern = MOVE_PATTERN_WAVE;
            enemy.aiState = AI_STATE_PATROL;
            enemy.color = MAGENTA;
            enemy.velocity = (Vector2){
                GetRandomValue(-40, 40) / 50.0f,
                GetRandomValue(-40, 40) / 50.0f
            };
            break;
            
        case ENEMY_TYPE_BOSS_FINAL:
            enemy.radius = ENEMY_MAX_SIZE * BOSS_SIZE_MULT * 1.5f;
            enemy.maxHealth = 1000.0f;
            enemy.movePattern = MOVE_PATTERN_AGGRESSIVE;
            enemy.aiState = AI_STATE_ATTACK;
            enemy.color = GOLD;
            enemy.stateFlags = ENEMY_STATE_SHIELDED;  // Start with shield
            enemy.stateData.shieldHealth = 500.0f;
            enemy.stateData.phase = 0;
            enemy.stateData.phaseTimer = 0.0f;
            enemy.velocity = (Vector2){0, 0};
            break;
            
        case ENEMY_TYPE_BLACKHOLE:
            enemy.radius = 40.0f;
            enemy.maxHealth = 1000.0f;
            enemy.color = (Color){50, 0, 100, 255};  // Deep purple
            enemy.movePattern = MOVE_PATTERN_TRACKING;  // Always tracks the player
            enemy.damage = 30.0f;
            enemy.stateFlags = ENEMY_STATE_INVULNERABLE;  // Start invulnerable until other enemies dead
            enemy.aiState = AI_STATE_CHASE;  // Always chasing player
            // hasPulsed = false is default (flag not set)
            enemy.stateData.transformTimer = 0.0f;  // No transformation timer yet
            enemy.stateData.stormCycleTimer = 0.0f;  // Storm cycle timer
            // Blackhole moves faster than normal enemies
            // enemy.velocity.x *= 2.0f;
            // enemy.velocity.y *= 2.0f;
            break;
            
        case ENEMY_TYPE_COUNT:
            // This shouldn't happen - fallback to basic
            return InitEnemyByType(ENEMY_TYPE_BASIC, screenWidth, screenHeight, playerPos);
    }
    
    enemy.health = enemy.maxHealth;
    enemy.originalColor = enemy.color;
    enemy.targetPosition = playerPos;
    
    return enemy;
}

// Legacy init function
Enemy InitEnemy(int screenWidth, int screenHeight) {
    return InitEnemyByType(ENEMY_TYPE_BASIC, screenWidth, screenHeight, (Vector2){screenWidth/2, screenHeight/2});
}

// Update enemy AI
void UpdateEnemyAI(Enemy* enemy, Vector2 playerPos, float deltaTime) {
    enemy->patternTimer += deltaTime;
    enemy->specialTimer += deltaTime;
    
    // State-specific AI behavior
    switch (enemy->aiState) {
        case AI_STATE_IDLE:
            // Do nothing special
            break;
            
        case AI_STATE_PATROL:
            if (enemy->type == ENEMY_TYPE_BASIC) {
                // Smooth wandering behavior for basic enemies with border avoidance
                
                // Update wander angle with small random changes for natural movement
                enemy->wanderAngle += (GetRandomValue(-100, 100) / 100.0f) * deltaTime * 3.0f; // More variation
                
                // Occasionally make bigger turns for exploration
                if (GetRandomValue(0, 100) < 2) { // 2% chance per frame
                    enemy->wanderAngle += GetRandomValue(-314, 314) / 100.0f; // -PI to PI
                }
                
                // Calculate new wander target position (project forward from current position)
                float wanderDistance = 60.0f;
                float wanderRadius = 40.0f; // Increased for more variation
                
                // Get current direction
                float currentAngle = atan2f(enemy->velocity.y, enemy->velocity.x);
                
                // Project a point ahead of the enemy
                Vector2 wanderCenter = {
                    enemy->position.x + cosf(currentAngle) * wanderDistance,
                    enemy->position.y + sinf(currentAngle) * wanderDistance
                };
                
                // Add random offset to create the wander target
                enemy->wanderTarget = (Vector2){
                    wanderCenter.x + cosf(enemy->wanderAngle) * wanderRadius,
                    wanderCenter.y + sinf(enemy->wanderAngle) * wanderRadius
                };
                
                // Calculate desired velocity towards wander target
                Vector2 desired = {
                    enemy->wanderTarget.x - enemy->position.x,
                    enemy->wanderTarget.y - enemy->position.y
                };
                
                // Smart border avoidance - only when heading towards border
                float borderMargin = 30.0f;  // Very close to edge
                float avoidanceStrength = 1.5f;  // Gentler force
                
                // Only apply avoidance if moving towards the border
                // Left border
                if (enemy->position.x < borderMargin && enemy->velocity.x < 0) {
                    float force = (borderMargin - enemy->position.x) / borderMargin;
                    desired.x += force * avoidanceStrength;
                }
                // Right border
                if (enemy->position.x > g_screenWidth - borderMargin && enemy->velocity.x > 0) {
                    float force = (enemy->position.x - (g_screenWidth - borderMargin)) / borderMargin;
                    desired.x -= force * avoidanceStrength;
                }
                // Top border
                if (enemy->position.y < borderMargin && enemy->velocity.y < 0) {
                    float force = (borderMargin - enemy->position.y) / borderMargin;
                    desired.y += force * avoidanceStrength;
                }
                // Bottom border
                if (enemy->position.y > g_screenHeight - borderMargin && enemy->velocity.y > 0) {
                    float force = (enemy->position.y - (g_screenHeight - borderMargin)) / borderMargin;
                    desired.y -= force * avoidanceStrength;
                }
                
                // If stuck at border for too long, give a stronger push inward
                float edgeThreshold = 10.0f;
                bool nearEdge = (enemy->position.x < edgeThreshold || 
                                enemy->position.x > g_screenWidth - edgeThreshold ||
                                enemy->position.y < edgeThreshold || 
                                enemy->position.y > g_screenHeight - edgeThreshold);
                
                if (nearEdge && enemy->patternTimer > 2.0f) {
                    // Force a new direction pointing towards center
                    float centerX = g_screenWidth / 2.0f;
                    float centerY = g_screenHeight / 2.0f;
                    desired.x = (centerX - enemy->position.x) * 0.02f;
                    desired.y = (centerY - enemy->position.y) * 0.02f;
                    // Reset timer to prevent constant center-seeking
                    enemy->patternTimer = 0.0f;
                }
                
                // Normalize and scale to desired speed
                float dist = sqrtf(desired.x * desired.x + desired.y * desired.y);
                if (dist > 0) {
                    float targetSpeed = 1.2f; // Slightly faster base speed
                    desired.x = (desired.x / dist) * targetSpeed;
                    desired.y = (desired.y / dist) * targetSpeed;
                }
                
                // Smooth steering - gradually turn towards desired direction
                float steerStrength = 0.12f; // Slightly more responsive
                enemy->velocity.x += (desired.x - enemy->velocity.x) * steerStrength;
                enemy->velocity.y += (desired.y - enemy->velocity.y) * steerStrength;
                
                // Maintain speed within limits
                float currentSpeed = sqrtf(enemy->velocity.x * enemy->velocity.x + enemy->velocity.y * enemy->velocity.y);
                if (currentSpeed > 0) {
                    float maxSpeed = 1.5f;
                    float minSpeed = 0.8f;
                    if (currentSpeed > maxSpeed) {
                        enemy->velocity.x = (enemy->velocity.x / currentSpeed) * maxSpeed;
                        enemy->velocity.y = (enemy->velocity.y / currentSpeed) * maxSpeed;
                    } else if (currentSpeed < minSpeed) {
                        enemy->velocity.x = (enemy->velocity.x / currentSpeed) * minSpeed;
                        enemy->velocity.y = (enemy->velocity.y / currentSpeed) * minSpeed;
                    }
                }
            } else {
                // Original behavior for other enemy types
                if (enemy->patternTimer > 2.0f + GetRandomValue(0, 20) / 10.0f) {
                    enemy->patternTimer = 0.0f;
                    enemy->velocity.x = GetRandomValue(-100, 100) / 100.0f;
                    enemy->velocity.y = GetRandomValue(-100, 100) / 100.0f;
                    
                    // Speed modifier based on type
                    if (enemy->type == ENEMY_TYPE_SPEEDY) {
                        enemy->velocity.x *= SPEEDY_SPEED_MULT;
                        enemy->velocity.y *= SPEEDY_SPEED_MULT;
                    }
                }
            }
            break;
            
        case AI_STATE_CHASE:
            // Update target position
            enemy->targetPosition = playerPos;
            break;
            
        case AI_STATE_ATTACK:
            // Boss attack patterns
            if (enemy->type == ENEMY_TYPE_BOSS_1 || enemy->type == ENEMY_TYPE_BOSS_FINAL) {
                enemy->stateData.phaseTimer += deltaTime;

                // Phase transitions
                if (enemy->health < enemy->maxHealth * 0.7f && enemy->stateData.phase == 0) {
                    enemy->stateData.phase = 1;
                    SetState(&enemy->stateFlags, ENEMY_STATE_INVULNERABLE);
                    enemy->stateData.phaseTimer = 0.0f;
                } else if (enemy->health < enemy->maxHealth * 0.3f && enemy->stateData.phase == 1) {
                    enemy->stateData.phase = 2;
                    SetState(&enemy->stateFlags, ENEMY_STATE_INVULNERABLE);
                    enemy->stateData.phaseTimer = 0.0f;
                }

                // End invulnerability after phase change
                if (HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE) && enemy->stateData.phaseTimer > 1.5f) {
                    ClearState(&enemy->stateFlags, ENEMY_STATE_INVULNERABLE);
                }
            }
            break;
            
        case AI_STATE_FLEE: {
            // Move away from player
            Vector2 awayFromPlayer = {
                enemy->position.x - playerPos.x,
                enemy->position.y - playerPos.y
            };
            float dist = sqrtf(awayFromPlayer.x * awayFromPlayer.x + awayFromPlayer.y * awayFromPlayer.y);
            if (dist > 0) {
                enemy->velocity.x = (awayFromPlayer.x / dist) * 2.0f;
                enemy->velocity.y = (awayFromPlayer.y / dist) * 2.0f;
            }
            break;
        }
            
        case AI_STATE_SPECIAL:
            // Execute special abilities
            ExecuteEnemySpecialAbility(enemy, playerPos);
            break;
    }
}

// Update enemy movement based on pattern
void UpdateEnemyMovement(Enemy* enemy, Vector2 playerPos, float deltaTime) {
    switch (enemy->movePattern) {
        case MOVE_PATTERN_RANDOM:
            // Already handled in AI update for smooth wandering
            break;
            
        case MOVE_PATTERN_STRAIGHT:
            // Just move in current direction
            break;
            
        case MOVE_PATTERN_TRACKING:
            // Move toward player
            if (enemy->aiState == AI_STATE_CHASE) {
                Vector2 toPlayer = {
                    playerPos.x - enemy->position.x,
                    playerPos.y - enemy->position.y
                };
                float dist = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
                if (dist > 0) {
                    float speed = TRACKER_SPEED_MULT;
                    if (enemy->type == ENEMY_TYPE_BOSS_1 || enemy->type == ENEMY_TYPE_BOSS_FINAL) {
                        speed = 0.5f + enemy->stateData.phase * 0.3f;  // Bosses get faster in later phases
                    }
                    enemy->velocity.x = (toPlayer.x / dist) * speed;
                    enemy->velocity.y = (toPlayer.y / dist) * speed;
                }
            }
            break;
            
        case MOVE_PATTERN_CIRCULAR:
            // Orbit around center point
            enemy->angle += deltaTime * 2.0f;  // 2 radians per second
            enemy->position.x = enemy->orbitCenter.x + cosf(enemy->angle) * enemy->orbitRadius;
            enemy->position.y = enemy->orbitCenter.y + sinf(enemy->angle) * enemy->orbitRadius;
            return;  // Skip normal position update
            
        case MOVE_PATTERN_ZIGZAG:
            // Change direction frequently
            if (enemy->patternTimer > 0.5f) {
                enemy->patternTimer = 0.0f;
                enemy->velocity.x = -enemy->velocity.x + GetRandomValue(-50, 50) / 100.0f;
                enemy->velocity.y = -enemy->velocity.y + GetRandomValue(-50, 50) / 100.0f;
                
                // Maintain speed
                float speed = sqrtf(enemy->velocity.x * enemy->velocity.x + enemy->velocity.y * enemy->velocity.y);
                if (speed > 0) {
                    float targetSpeed = (enemy->type == ENEMY_TYPE_SPEEDY) ? SPEEDY_SPEED_MULT * 2.0f : 1.5f;
                    enemy->velocity.x = (enemy->velocity.x / speed) * targetSpeed;
                    enemy->velocity.y = (enemy->velocity.y / speed) * targetSpeed;
                }
            }
            break;
            
        case MOVE_PATTERN_SPIRAL:
            enemy->angle += deltaTime * 3.0f;
            enemy->orbitRadius += deltaTime * 20.0f;  // Expanding spiral
            enemy->position.x = enemy->orbitCenter.x + cosf(enemy->angle) * enemy->orbitRadius;
            enemy->position.y = enemy->orbitCenter.y + sinf(enemy->angle) * enemy->orbitRadius;
            return;  // Skip normal position update
            
        case MOVE_PATTERN_TELEPORT:
            // Handled in special ability
            break;
            
        case MOVE_PATTERN_PATROL:
            // Move between waypoints
            if (Vector2Distance(enemy->position, enemy->targetPosition) < 50.0f) {
                // Reached waypoint, pick new one
                enemy->targetPosition.x = GetRandomValue(100, 700);
                enemy->targetPosition.y = GetRandomValue(100, 700);
            }
            
            Vector2 toTarget = {
                enemy->targetPosition.x - enemy->position.x,
                enemy->targetPosition.y - enemy->position.y
            };
            float dist = sqrtf(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
            if (dist > 0) {
                enemy->velocity.x = (toTarget.x / dist) * 1.0f;
                enemy->velocity.y = (toTarget.y / dist) * 1.0f;
            }
            break;
            
        case MOVE_PATTERN_WAVE:
            // Sine wave movement
            enemy->angle += deltaTime * 4.0f;
            enemy->velocity.y = sinf(enemy->angle) * 2.0f;
            break;
            
        case MOVE_PATTERN_AGGRESSIVE: {
            // Fast tracking with prediction
            Vector2 toPlayer = {
                playerPos.x - enemy->position.x,
                playerPos.y - enemy->position.y
            };
            float playerDist = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
            if (playerDist > 0) {
                float speed = 2.0f + enemy->stateData.phase * 0.5f;
                enemy->velocity.x = (toPlayer.x / playerDist) * speed;
                enemy->velocity.y = (toPlayer.y / playerDist) * speed;
            }
            break;
        }
    }
    
    // Update position based on velocity
    enemy->position.x += enemy->velocity.x;
    enemy->position.y += enemy->velocity.y;
}

// Execute special abilities
void ExecuteEnemySpecialAbility(Enemy* enemy, Vector2 playerPos) {
    switch (enemy->type) {
        case ENEMY_TYPE_TELEPORTER:
            if (enemy->specialTimer > TELEPORT_COOLDOWN) {
                enemy->specialTimer = 0.0f;
                // Teleport to random position
                enemy->position.x = GetRandomValue(100, 700);
                enemy->position.y = GetRandomValue(100, 700);
                
                // Flash effect
                enemy->color = WHITE;
            } else if (enemy->specialTimer > 0.2f && enemy->color.r == 255) {
                enemy->color = enemy->originalColor;
            }
            break;
            
        case ENEMY_TYPE_REPULSOR:
            // Repulsion field is always active, handled in physics
            break;
            
        case ENEMY_TYPE_BOSS_1:
        case ENEMY_TYPE_BOSS_FINAL:
            // Boss special attacks based on phase
            if (enemy->stateData.phase >= 1 && enemy->specialTimer > 3.0f) {
                enemy->specialTimer = 0.0f;
                // Burst movement
                enemy->velocity.x = GetRandomValue(-300, 300) / 100.0f;
                enemy->velocity.y = GetRandomValue(-300, 300) / 100.0f;
            }

            if (enemy->stateData.phase >= 2) {
                // Rage mode - faster and more aggressive
                enemy->color = RED;
            }
            break;
            
        case ENEMY_TYPE_BLACKHOLE:
            // Blackhole pulse effect
            if (enemy->aiState == AI_STATE_SPECIAL) {
                // Increase size during pulse
                enemy->radius = enemy->radius * 1.1f;
                if (enemy->radius > 60.0f) {
                    enemy->radius = 40.0f;  // Reset to normal size
                }
                // Darker color during pulse
                enemy->color = (Color){20, 0, 50, 255};
            } else {
                enemy->radius = 40.0f;  // Normal size
                enemy->color = enemy->originalColor;
            }
            break;
            
        // These enemy types don't have special abilities
        case ENEMY_TYPE_BASIC:
        case ENEMY_TYPE_TRACKER:
        case ENEMY_TYPE_SPEEDY:
        case ENEMY_TYPE_SPLITTER:
        case ENEMY_TYPE_ORBITER:
        case ENEMY_TYPE_CLUSTER:
        case ENEMY_TYPE_COUNT:
            // No special abilities
            break;
    }
}

// Main update function
void UpdateEnemy(Enemy* enemy, int screenWidth, int screenHeight, float deltaTime) {
    // Don't update if invulnerable (phase transition)
    if (HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE)) {
        enemy->color = ((int)(GetTime() * 10) % 2 == 0) ? WHITE : enemy->originalColor;
    }
    
    // Update position based on velocity (moved to UpdateEnemyMovement)
    // enemy->position.x += enemy->velocity.x;
    // enemy->position.y += enemy->velocity.y;
    
    // Increase radius over time for basic enemies
    if (enemy->type == ENEMY_TYPE_BASIC) {
        enemy->radius += deltaTime * 5.0f; // Slower growth
    }

    // Update maxHealth and health as radius grows
    if (enemy->type == ENEMY_TYPE_BASIC) {
        float prevMaxHealth = enemy->maxHealth;
        enemy->maxHealth = enemy->radius * 10.0f;
        if (enemy->health == prevMaxHealth) {
            enemy->health = enemy->maxHealth;
        } else {
            float ratio = prevMaxHealth > 0 ? (enemy->health / prevMaxHealth) : 1.0f;
            enemy->health = enemy->maxHealth * ratio;
        }
    }

    // Screen boundary check - softer for ENEMY_TYPE_BASIC
    if (enemy->type == ENEMY_TYPE_BASIC) {
        // Allow centers to reach edges for corner accessibility
        if (enemy->position.x < 0) {
            enemy->position.x = 0;
        }
        if (enemy->position.x > screenWidth) {
            enemy->position.x = screenWidth;
        }
        if (enemy->position.y < 0) {
            enemy->position.y = 0;
        }
        if (enemy->position.y > screenHeight) {
            enemy->position.y = screenHeight;
        }
    } else {
        // Original hard boundary check for other enemy types
        float margin = enemy->radius * 0.7f;
        if (enemy->position.x < -margin) {
            enemy->position.x = -margin;
            enemy->velocity.x *= -1;
        }
        if (enemy->position.x > screenWidth + margin) {
            enemy->position.x = screenWidth + margin;
            enemy->velocity.x *= -1;
        }
        if (enemy->position.y < -margin) {
            enemy->position.y = -margin;
            enemy->velocity.y *= -1;
        }
        if (enemy->position.y > screenHeight + margin) {
            enemy->position.y = screenHeight + margin;
            enemy->velocity.y *= -1;
        }
    }

    // Gravity system integration - BLACKHOLE
    if (enemy->type == ENEMY_TYPE_BLACKHOLE) {
        bool shouldHaveGravity = HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE) &&
                                 !HasState(enemy->stateFlags, ENEMY_STATE_PULSED);

        if (shouldHaveGravity) {
            if (enemy->gravitySourceId == 0) {
                // Register new gravity source
                GravitySource source = {
                    .position = enemy->position,
                    .radius = 200.0f,          // BLACKHOLE_RADIUS
                    .strength = 5.0f,          // BLACKHOLE_FORCE
                    .type = GRAVITY_TYPE_ATTRACTION,
                    .active = true,
                    .sourcePtr = enemy,
                    .sourceType = 0,  // Enemy
                    .sourceId = 0     // Will be assigned
                };
                enemy->gravitySourceId = RegisterGravitySource(source);
            } else {
                // Update existing source position
                UpdateGravitySource(enemy->gravitySourceId, enemy->position);
            }
        } else {
            // Remove gravity source if conditions no longer met
            if (enemy->gravitySourceId != 0) {
                UnregisterGravitySource(enemy->gravitySourceId);
                enemy->gravitySourceId = 0;
            }
        }
    }

    // Gravity system integration - REPULSOR
    if (enemy->type == ENEMY_TYPE_REPULSOR) {
        if (enemy->gravitySourceId == 0) {
            // Register repulsion source
            GravitySource source = {
                .position = enemy->position,
                .radius = 150.0f,          // REPULSE_RADIUS
                .strength = 2.0f,          // Repulsion strength
                .type = GRAVITY_TYPE_REPULSION,
                .active = true,
                .sourcePtr = enemy,
                .sourceType = 0,
                .sourceId = 0
            };
            enemy->gravitySourceId = RegisterGravitySource(source);
        } else {
            // Update position
            UpdateGravitySource(enemy->gravitySourceId, enemy->position);
        }
    }
}

// Draw enemy
void DrawEnemy(Enemy enemy) {
    float timeSinceSpawn = GetTime() - enemy.spawnTime;
    // Blink for first 0.5 seconds
    if (timeSinceSpawn < 0.5f && ((int)(GetTime() * 10) % 2 == 0)) {
        return; // Skip drawing (blink)
    }
    
    // Draw shield first if active
    if (HasState(enemy.stateFlags, ENEMY_STATE_SHIELDED) && enemy.stateData.shieldHealth > 0) {
        DrawEnemyShield(enemy);
    }
    
    // Color based on health
    float ratio = (enemy.maxHealth > 0) ? (enemy.health / enemy.maxHealth) : 0.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    
    Color c = enemy.color;
    
    // Special color handling for certain types
    if (enemy.type == ENEMY_TYPE_BOSS_FINAL && enemy.stateData.phase >= 2) {
        c = RED;  // Rage mode
    } else if (HasState(enemy.stateFlags, ENEMY_STATE_INVULNERABLE)) {
        // Handled in update
    } else if (enemy.type != ENEMY_TYPE_TELEPORTER || enemy.color.r != 255) {
        // Health-based color for non-teleporting enemies
        if (ratio < 0.5f) {
            float t = ratio * 2.0f;
            c.r = (unsigned char)LerpFloat(255, enemy.originalColor.r, t);
            c.g = (unsigned char)LerpFloat(255, enemy.originalColor.g, t);
            c.b = (unsigned char)LerpFloat(255, enemy.originalColor.b, t);
        }
    }
    
    // Special rendering for blackhole enemy
    if (enemy.type == ENEMY_TYPE_BLACKHOLE) {
        if (HasState(enemy.stateFlags, ENEMY_STATE_INVULNERABLE) &&
            !HasState(enemy.stateFlags, ENEMY_STATE_PULSED)) {
            // Draw gravitational rings when invulnerable
            for (int i = 3; i >= 0; i--) {
                float ringRadius = enemy.radius * (2.0f + i * 0.5f);
                Color ringColor = (Color){c.r, c.g, c.b, (unsigned char)(30 - i * 7)};
                DrawCircleLines(enemy.position.x, enemy.position.y, ringRadius, ringColor);
            }
            // Draw invulnerability shield effect
            DrawCircleLines(enemy.position.x, enemy.position.y, enemy.radius + 5,
                          (Color){100, 100, 255, 100});
            // Draw dark core
            DrawCircle(enemy.position.x, enemy.position.y, enemy.radius, BLACK);
            DrawCircle(enemy.position.x, enemy.position.y, enemy.radius * 0.8f, c);
        } else if (HasState(enemy.stateFlags, ENEMY_STATE_PULSED)) {
            // After transformation - semi-magnetic storm with fast movement
            DrawCircle(enemy.position.x, enemy.position.y, enemy.radius, c);

            // Check if storm is active based on cycle timer
            bool stormActive = fmodf(enemy.stateData.stormCycleTimer, 10.0f) < 5.0f;
            
            // Draw storm field based on state
            if (stormActive) {
                // Draw active storm rings (red)
                float stormTime = GetTime() * 4.0f;
                for (int i = 0; i < 3; i++) {
                    float ringRadius = 150.0f - i * 40.0f; // Match SEMI_STORM_RADIUS
                    float waveOffset = sinf(stormTime + i * 1.5f) * 8.0f;
                    unsigned char alpha = (unsigned char)(60 - i * 15);
                    DrawCircleLines(enemy.position.x, enemy.position.y, ringRadius + waveOffset, 
                                  (Color){255, 50, 50, alpha});
                }
                // Draw warning circle
                DrawCircleLines(enemy.position.x, enemy.position.y, enemy.radius + 5, 
                              (Color){255, 100, 100, 150});
            } else {
                // Draw vulnerable state (green glow)
                DrawCircleLines(enemy.position.x, enemy.position.y, enemy.radius + 5, 
                              (Color){100, 255, 100, 100});
                // Pulsing effect to indicate vulnerability
                float pulse = sinf(GetTime() * 5.0f) * 10.0f + 60.0f;
                DrawCircleLines(enemy.position.x, enemy.position.y, pulse, 
                              (Color){100, 255, 100, 50});
            }
            
            // Draw speed lines
            Vector2 vel = enemy.velocity;
            float speed = sqrtf(vel.x * vel.x + vel.y * vel.y);
            if (speed > 0.1f) {
                Vector2 norm = (Vector2){-vel.x / speed, -vel.y / speed};
                for (int i = 0; i < 3; i++) {
                    float offset = i * 10.0f;
                    DrawLine(enemy.position.x + norm.x * offset, 
                           enemy.position.y + norm.y * offset,
                           enemy.position.x + norm.x * (offset + 5),
                           enemy.position.y + norm.y * (offset + 5),
                           (Color){c.r, c.g, c.b, (unsigned char)(100 - i * 30)});
                }
            }
        } else {
            // When vulnerable, draw as a fast-moving enemy
            DrawCircle(enemy.position.x, enemy.position.y, enemy.radius, c);
            // Draw speed lines
            Vector2 vel = enemy.velocity;
            float speed = sqrtf(vel.x * vel.x + vel.y * vel.y);
            if (speed > 0.1f) {
                Vector2 norm = (Vector2){-vel.x / speed, -vel.y / speed};
                for (int i = 0; i < 3; i++) {
                    float offset = i * 10.0f;
                    DrawLine(enemy.position.x + norm.x * offset, 
                           enemy.position.y + norm.y * offset,
                           enemy.position.x + norm.x * (offset + 5),
                           enemy.position.y + norm.y * (offset + 5),
                           (Color){c.r, c.g, c.b, (unsigned char)(100 - i * 30)});
                }
            }
        }
    } else {
        DrawCircle(enemy.position.x, enemy.position.y, enemy.radius, c);
    }
    
    // Draw type indicator for special enemies
    if (enemy.type != ENEMY_TYPE_BASIC) {
        const char* typeChar = "";
        switch (enemy.type) {
            case ENEMY_TYPE_TRACKER: typeChar = "T"; break;
            case ENEMY_TYPE_SPEEDY: typeChar = "S"; break;
            case ENEMY_TYPE_SPLITTER: typeChar = "X"; break;
            case ENEMY_TYPE_ORBITER: typeChar = "O"; break;
            case ENEMY_TYPE_TELEPORTER: typeChar = "!"; break;
            case ENEMY_TYPE_REPULSOR: typeChar = "R"; break;
            case ENEMY_TYPE_CLUSTER: typeChar = "C"; break;
            case ENEMY_TYPE_BOSS_1: typeChar = "B1"; break;
            case ENEMY_TYPE_BOSS_FINAL: typeChar = "BF"; break;
            case ENEMY_TYPE_BLACKHOLE: typeChar = "BH"; break;
            default: break;
        }
        
        if (strlen(typeChar) > 0) {
            int fontSize = (enemy.type == ENEMY_TYPE_BOSS_1 || enemy.type == ENEMY_TYPE_BOSS_FINAL) ? 24 : 16;
            int textWidth = MeasureText(typeChar, fontSize);
            DrawText(typeChar, enemy.position.x - textWidth/2, enemy.position.y - fontSize/2, fontSize, WHITE);
        }
    }
    
    // Draw health text
    char healthText[32];
    sprintf(healthText, "%d/%d", (int)enemy.health, (int)enemy.maxHealth);
    int textWidth = MeasureText(healthText, 16);
    DrawText(healthText, enemy.position.x - textWidth/2, enemy.position.y - enemy.radius - 20, 16, BLACK);
}

// Draw enemy shield
void DrawEnemyShield(Enemy enemy) {
    float shieldRatio = enemy.stateData.shieldHealth / (enemy.type == ENEMY_TYPE_BOSS_FINAL ? 500.0f : 200.0f);
    Color shieldColor = Fade(SKYBLUE, 0.3f + shieldRatio * 0.3f);
    DrawCircleLines(enemy.position.x, enemy.position.y, enemy.radius + 10, shieldColor);
    DrawCircleLines(enemy.position.x, enemy.position.y, enemy.radius + 12, shieldColor);
}

// Damage enemy
void DamageEnemy(Enemy* enemy, float damage) {
    if (HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE)) return;

    // Damage shield first
    if (HasState(enemy->stateFlags, ENEMY_STATE_SHIELDED) && enemy->stateData.shieldHealth > 0) {
        enemy->stateData.shieldHealth -= damage;
        if (enemy->stateData.shieldHealth < 0) {
            enemy->health += enemy->stateData.shieldHealth;  // Apply remaining damage to health
            enemy->stateData.shieldHealth = 0;
            ClearState(&enemy->stateFlags, ENEMY_STATE_SHIELDED);
        }
    } else {
        enemy->health -= damage;
    }
}

// Check if enemy should split
bool ShouldEnemySplit(Enemy* enemy) {
    return enemy->type == ENEMY_TYPE_SPLITTER &&
           enemy->health <= 0 &&
           enemy->stateData.splitCount > 0 &&
           enemy->radius > ENEMY_MIN_SIZE;
}

// Apply repulsion field to particles
void ApplyRepulsionField(Enemy* enemy, void* particles, int particleCount) {
    if (enemy->type != ENEMY_TYPE_REPULSOR) return;
    
    // This will be implemented in physics.c to avoid circular dependency
    // The function signature is here for reference
}

// Change AI state
void ChangeEnemyAIState(Enemy* enemy, AIState newState) {
    enemy->aiState = newState;
    enemy->patternTimer = 0.0f;
    
    // State entry logic
    switch (newState) {
        case AI_STATE_FLEE:
            enemy->velocity.x *= 1.5f;
            enemy->velocity.y *= 1.5f;
            break;
        case AI_STATE_ATTACK:
            enemy->color = Fade(enemy->originalColor, 0.8f);
            break;
        default:
            break;
    }
}

// Helper function for color interpolation
static float LerpFloat(float a, float b, float t) {
    return a + (b - a) * t;
} 