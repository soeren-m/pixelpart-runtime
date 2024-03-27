#pragma once

#include "ParticleEngine.h"
#include "ParticleContainer.h"
#include "SizeSolver.h"
#include "ColorSolver.h"
#include "AccelerationSolver.h"
#include "ForceSolver.h"
#include "CollisionSolver.h"
#include "MotionPathSolver.h"
#include "RotationSolver.h"
#include "IntegrationSolver.h"

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
	virtual const ParticleData& getParticles(uint32_t particleTypeIndex) const override;

	uint32_t getParticleCapacity() const;
	uint32_t getNumActiveThreads() const;

private:
	void stepParticles(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleDataPointer particles, uint32_t numParticles,
		float_t t, float_t dt) const;

	uint32_t spawnParticles(uint32_t count, uint32_t pParent,
		uint32_t particleTypeIndex, uint32_t parentParticleTypeIndex, uint32_t particleEmitterIndex,
		float_t dt, float_t t, float_t tParent);
	void spawnParticle(uint32_t p, uint32_t pParent,
		uint32_t particleTypeIndex, uint32_t parentParticleTypeIndex, uint32_t particleEmitterIndex,
		float_t dt, float_t t, float_t tParent);

	std::vector<ParticleContainer> particleContainers;
	ParticleData emptyParticleData;

	std::vector<float_t> particleSpawnCount;
	std::vector<uint32_t> particleEmitterGridIndices;
	uint32_t particleCapacity = 0u;
	uint32_t particleId = 0u;
	uint32_t activeSeed = 0u;
	float_t time = 0.0;

	SizeSolver sizeSolver;
	ColorSolver colorSolver;
	AccelerationSolver accelerationSolver;
	ForceSolver forceSolver;
	CollisionSolver collisionSolver;
	MotionPathSolver motionPathSolver;
	RotationSolver rotationSolver;
	IntegrationSolver integrationSolver;

#ifndef __EMSCRIPTEN__
	std::shared_ptr<ThreadPool> threadPool;
#endif
	uint32_t numTotalActiveThreads = 0u;
	uint32_t numParticlesPerThread = 128u;

	std::mt19937 rng;
};
}