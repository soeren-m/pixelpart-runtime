#pragma once

#include "ParticleSolver.h"
#include "ForceSolver.h"
#include "CollisionSolver.h"
#include "MotionPathSolver.h"

#ifndef __EMSCRIPTEN__
#include "../common/ThreadPool.h"
#endif

namespace pixelpart {
class ParticleSolverCPU : public ParticleSolver {
public:
	ParticleSolverCPU(uint32_t numThreads = 0u);

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType, ParticleData& particles, uint32_t numParticles, float_t t, float_t dt) override;
	virtual void refresh(const Effect* effect, uint32_t flags) override;

	virtual uint32_t getNumActiveThreads() const override;

	void setNumParticlesPerThread(uint32_t num);

private:
	static void simulateParticles(
		const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleDataPointer particles, uint32_t workgroupSize,
		float_t t, float_t dt,
		const ForceSolver& forceSolver,
		const CollisionSolver& collisionSolver,
		const MotionPathSolver& motionPathSolver);

#ifndef __EMSCRIPTEN__
	std::unique_ptr<ThreadPool> threadPool;
#endif
	uint32_t numActiveThreads = 0u;
	uint32_t numParticlesPerThread = 128u;

	ForceSolver forceSolver;
	CollisionSolver collisionSolver;
	MotionPathSolver motionPathSolver;
};
}