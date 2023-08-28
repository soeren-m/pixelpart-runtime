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
	ParticleSolverCPU(uint32_t numThreads = 0);

	virtual void solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType, ParticleData& particles, uint32_t numParticles, floatd t, floatd dt) override;
	virtual void refresh(const Effect* effect, uint32_t flags) override;

	virtual uint32_t getNumActiveThreads() const override;

	void setNumParticlesPerThread(uint32_t num);

private:
	static void simulateParticles(
		const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleDataPointer particles, uint32_t workgroupSize,
		floatd t, floatd dt,
		const ForceSolver& forceSolver,
		const CollisionSolver& collisionSolver,
		const MotionPathSolver& motionPathSolver);

#ifndef __EMSCRIPTEN__
	std::unique_ptr<ThreadPool> threadPool;
#endif
	uint32_t numActiveThreads = 0;
	uint32_t numParticlesPerThread = 128;

	ForceSolver forceSolver;
	CollisionSolver collisionSolver;
	MotionPathSolver motionPathSolver;
};
}