#pragma once

#include "../common/Id.h"
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
#include <vector>

#ifdef PIXELPART_MULTITHREADING
#include "../common/ThreadPool.h"
#endif

namespace pixelpart {
class CPUParticleEngine : public ParticleEngine {
public:
	CPUParticleEngine(const Effect& fx, uint32_t capacity);
#ifdef PIXELPART_MULTITHREADING
	CPUParticleEngine(const Effect& fx, uint32_t capacity, std::shared_ptr<ThreadPool> thPool);
#endif

	virtual void step(float_t dt) override;
	virtual void restart(bool reset) override;

	virtual float_t currentTime() const override;

	virtual void applySeed(uint32_t seed) override;
	virtual void resetSeed() override;

	virtual void spawnParticles(id_t particleTypeId, uint32_t count) override;

	virtual uint32_t particleCount() const override;
	virtual uint32_t particleCount(uint32_t particleTypeIndex) const override;

	virtual ParticleCollection::ReadPtr particles(uint32_t particleTypeIndex) const override;

	uint32_t particleCapacity() const;

	uint32_t activeThreadCount() const;

private:
	void stepParticles(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
		ParticleCollection::WritePtr particles, uint32_t particleCount,
		float_t t, float_t dt) const;

	std::vector<ParticleCollection> particleCollections;

	std::vector<float_t> emissionCount;
	uint32_t particleCap = 0u;
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

#ifdef PIXELPART_MULTITHREADING
	std::shared_ptr<ThreadPool> threadPool;
#endif
	uint32_t totalActiveThreadCount = 0u;
	uint32_t particleCountPerThread = 128u;
};
}