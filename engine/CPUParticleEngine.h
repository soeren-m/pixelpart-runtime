#pragma once

#include "ParticleEngine.h"
#include "ParticleGenerator.h"
#include "SizeSolver.h"
#include "ColorSolver.h"
#include "AccelerationSolver.h"
#include "ForceSolver.h"
#include "CollisionSolver.h"
#include "MotionPathSolver.h"
#include "RotationSolver.h"
#include "IntegrationSolver.h"
#include "LifeSolver.h"

#ifndef __EMSCRIPTEN__
#include "../common/ThreadPool.h"
#endif

namespace pixelpart {
class CPUParticleEngine : public ParticleEngine {
public:
	CPUParticleEngine(const Effect& fx, uint32_t capacity);
#ifndef __EMSCRIPTEN__
	CPUParticleEngine(const Effect& fx, uint32_t capacity, std::shared_ptr<ThreadPool> thPool);
#endif

	virtual void step(float_t dt) override;
	virtual void restart(bool reset) override;

	virtual float_t getTime() const override;

	virtual void applySeed(uint32_t seed) override;
	virtual void resetSeed() override;

	virtual void spawnParticles(id_t particleTypeId, uint32_t count) override;

	virtual uint32_t getNumParticles() const override;
	virtual uint32_t getNumParticles(uint32_t particleTypeIndex) const override;
	virtual ParticleReadPtr getParticles(uint32_t particleTypeIndex) const override;

	uint32_t getParticleCapacity() const;
	uint32_t getNumActiveThreads() const;

private:
	void stepParticles(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleWritePtr particles, uint32_t numParticles,
		float_t t, float_t dt) const;

	std::vector<ParticleContainer> particleContainers;

	std::vector<float_t> emissionCount;
	uint32_t particleCapacity = 0u;
	uint32_t activeSeed = 0u;
	float_t time = 0.0;

	ParticleGenerator particleGenerator;

	SizeSolver sizeSolver;
	ColorSolver colorSolver;
	AccelerationSolver accelerationSolver;
	ForceSolver forceSolver;
	CollisionSolver collisionSolver;
	MotionPathSolver motionPathSolver;
	RotationSolver rotationSolver;
	IntegrationSolver integrationSolver;
	LifeSolver lifeSolver;

#ifndef __EMSCRIPTEN__
	std::shared_ptr<ThreadPool> threadPool;
#endif
	uint32_t numTotalActiveThreads = 0u;
	uint32_t numParticlesPerThread = 128u;
};
}