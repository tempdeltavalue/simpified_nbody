#include "Particle.h"


Particle::Particle(glm::vec3 pos, glm::vec3 vel, float m) :position(glm::vec4(pos, 0.f)), velocity(glm::vec4(vel, 0.f)), mass(m), acceleration(glm::vec4(0.f, 0.f, 0.f, 0.f)){};

Particle::Particle() = default;
Particle::~Particle() = default;

