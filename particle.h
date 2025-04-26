#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"

// Particle entity structure
typedef struct {
    Vector2 position;    // Current position
    Vector2 velocity;    // Current velocity
    Color color;        // Particle color
} Particle;

// Particle initialization
Particle InitParticle(int screenWidth, int screenHeight);

// Particle physics functions
void AttractParticle(Particle* particle, Vector2 target, float force);
void ApplyFriction(Particle* particle, float frictionCoeff);
void MoveParticle(Particle* particle, int screenWidth, int screenHeight);

// Particle rendering
void DrawParticlePixel(Particle particle);

#endif // PARTICLE_H 