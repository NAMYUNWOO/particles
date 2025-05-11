#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "raylib.h"

// Explosion particle structure
typedef struct {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float radius;
    float timeToLive;
} ExplosionParticle;

#define MAX_EXPLOSION_PARTICLES 200

// Explosion functions
void SpawnExplosion(ExplosionParticle* particles, int* particleCount, Vector2 position, Color color, float baseRadius);
void UpdateExplosionParticle(ExplosionParticle* particle, float deltaTime);
void DrawExplosionParticle(ExplosionParticle particle);

#endif // EXPLOSION_H