#include "explosion.h"
#include <math.h>
#include <stdlib.h>

// Helper function for linear interpolation
static float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

void SpawnExplosion(ExplosionParticle* particles, int* particleCount, Vector2 position, Color color, float baseRadius) {
    int numParticles = 20 + GetRandomValue(0, 10); // 20~30개
    for (int i = 0; i < numParticles && *particleCount < MAX_EXPLOSION_PARTICLES; i++) {
        float angle = ((float)i / numParticles) * 2 * PI + ((float)GetRandomValue(-100, 100) / 100.0f) * 0.2f;
        float speed = 2.0f + (float)GetRandomValue(0, 100) / 100.0f * 2.0f;
        ExplosionParticle ep = {
            .position = position,
            .velocity = { cosf(angle) * speed, sinf(angle) * speed },
            .color = (Color){
                (unsigned char)Lerp(color.r, YELLOW.r, GetRandomValue(0,100)/100.0f),
                (unsigned char)Lerp(color.g, YELLOW.g, GetRandomValue(0,100)/100.0f),
                (unsigned char)Lerp(color.b, YELLOW.b, GetRandomValue(0,100)/100.0f),
                255
            },
            .radius = baseRadius * (0.2f + (float)GetRandomValue(0,100)/500.0f),
            .timeToLive = 0.5f + (float)GetRandomValue(0,100)/200.0f // 0.5~1.0초
        };
        particles[(*particleCount)++] = ep;
    }
}

void UpdateExplosionParticle(ExplosionParticle* particle, float deltaTime) {
    particle->position.x += particle->velocity.x;
    particle->position.y += particle->velocity.y;
    particle->velocity.x *= 0.95f;
    particle->velocity.y *= 0.95f;
    particle->timeToLive -= deltaTime;
}

void DrawExplosionParticle(ExplosionParticle particle) {
    Color c = particle.color;
    float t = particle.timeToLive;
    if (t < 0.2f) c.a = (unsigned char)(255 * (t/0.2f));
    DrawCircleV(particle.position, particle.radius, c);
}