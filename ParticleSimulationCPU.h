#pragma once

#include "ParticleSimulation.h"

#ifndef __EMSCRIPTEN__
#include "ThreadPool.h"
#endif

namespace pixelpart {
class ParticleSimulationCPU : public ParticleSimulation {
public:
	ParticleSimulationCPU(uint32_t numThreads = 0);

	virtual void simulate(const ParticleEmitter& particleEmitter, const ParticleType& particleType, ParticleData& particles, uint32_t numParticles, const ForceSolver& forceSolver, const CollisionSolver& collisionSolver, floatd t, floatd dt) override;

	virtual uint32_t getNumActiveThreads() const override;

	void setNumParticlesPerThread(uint32_t num);

private:
	static void simulateParticles(const ParticleEmitter& particleEmitter, const ParticleType& particleType, ParticleDataPointer particles, uint32_t workgroupSize, const ForceSolver& forceSolver, const CollisionSolver& collisionSolver, floatd t, floatd dt);

#ifndef __EMSCRIPTEN__
	std::unique_ptr<ThreadPool> threadPool;
#endif
	uint32_t numActiveThreads = 0;
	uint32_t numParticlesPerThread = 128;
};
}