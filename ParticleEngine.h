#pragma once

#include "Effect.h"
#include "ParticleSimulation.h"
#include "ParticleContainer.h"

namespace pixelpart {
class ParticleEngine {
public:
	ParticleEngine();
	ParticleEngine(const Effect* fx, uint32_t capacity, uint32_t maxNumThreads = 0);

	void step(floatd dt);

	void reset();
	void restart();
	floatd getTime() const;

	void spawnParticles(uint32_t particleTypeId, uint32_t count);

	void setSeed(uint32_t sd);
	void resetSeed();

	void setEffect(const Effect* fx);
	const Effect* getEffect() const;

	uint32_t getParticleCapacity() const;
	uint32_t getNumParticles() const;
	uint32_t getNumParticles(uint32_t particleTypeIndex) const;

	const ParticleData& getParticles(uint32_t particleTypeIndex) const;

	uint32_t getNumActiveThreads() const;

	void updateSolvers();
	void updateParticleSolver();
	void updateForceSolver();
	void updateCollisionSolver();

private:
	uint32_t spawnParticles(uint32_t count, uint32_t pParent, uint32_t particleTypeIndex, uint32_t parentParticleTypeIndex, uint32_t particleEmitterIndex, floatd t, floatd tParent);

	void createParticle(uint32_t p, uint32_t pParent, uint32_t particleTypeIndex, uint32_t parentParticleTypeIndex, uint32_t particleEmitterIndex, floatd t, floatd tParent);

	const Effect* effect = nullptr;
	std::vector<ParticleContainer> particleContainers;
	std::vector<std::vector<uint32_t>> particleSubTypes;
	std::vector<floatd> particleSpawnCount;
	uint32_t particleCapacity = 0;
	uint32_t particleId = 0;
	uint32_t seed = 0;
	floatd time = 0.0;

	std::unique_ptr<ParticleSimulation> particleSimulation;
	ForceSolver forceSolver;
	CollisionSolver collisionSolver;
	uint32_t numActiveThreads = 0;

	std::mt19937 rng;
};
}