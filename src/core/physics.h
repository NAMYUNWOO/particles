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

#endif // PHYSICS_H