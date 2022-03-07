#pragma once

#include "ParticleEmitter.h"
#include "ParticleData.h"
#include "ForceSolver.h"
#include "CollisionSolver.h"

namespace pixelpart {
class ParticleSimulation {
public:
	virtual void simulate(const ParticleEmitter& emitter, ParticleData& particles, uint32_t numParticles, const ForceSolver& forceSolver, const CollisionSolver& collisionSolver, floatd t, floatd dt) = 0;

	virtual uint32_t getNumActiveThreads() const = 0;
};
}