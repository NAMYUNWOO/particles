#ifndef PHYSICS_H
#define PHYSICS_H

#include "game.h"
#include "raylib.h"
#include "../entities/enemy.h"
#include "../entities/particle.h"
#include "../entities/explosion.h"

// Physics functions
bool CheckCollisionEnemyParticle(Enemy enemy, Particle particle);
void ProcessEnemyCollisions(Game* game);

// 메모리 풀 관리 함수
void InitPhysicsMemoryPools(void);
void CleanupPhysicsMemoryPools(void);

#endif // PHYSICS_H