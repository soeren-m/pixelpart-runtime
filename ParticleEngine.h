#pragma once

#include "Effect.h"
#include "ParticleSimulation.h"
#include <random>

namespace pixelpart {
class ParticleEngine {
public:
	ParticleEngine();
	ParticleEngine(const Effect* effectPtr, uint32_t capacity);

	void step(floatd dt);

	void reset();
	void restart();
	floatd getTime() const;

	void spawnParticles(uint32_t emitterId, uint32_t count);

	void setSeed(uint32_t sd);
	void resetSeed();

	void setEffect(const Effect* effectPtr);
	void setParticleCapacity(uint32_t capacity);
	const Effect* getEffect() const;
	uint32_t getParticleCapacity() const;
	uint32_t getNumParticles() const;
	uint32_t getNumParticles(uint32_t emitterIndex) const;
	const ParticleData& getParticles(uint32_t emitterIndex) const;
	const std::vector<vec2d>& getParticlePositionSnapshot(uint32_t emitterIndex) const;

	uint32_t getNumActiveThreads() const;

	void onEffectUpdate();
	void onParticleEmitterUpdate();
	void onForceFieldUpdate();
	void onColliderUpdate();

private:
	int32_t sampleUniformInt(int32_t min, int32_t max);
	floatd sampleUniform(floatd min, floatd max);
	floatd sampleNormal(floatd min, floatd max);
	floatd sampleNormalReverse(floatd min, floatd max);
	floatd sample(ParticleEmitter::Distribution distribution, floatd min, floatd max);

	vec2d generatePointOnSegment(const vec2d& position, floatd length, floatd angle, ParticleEmitter::Distribution distribution);
	vec2d generatePointInEllipse(const vec2d& position, const vec2d& size, floatd angle, ParticleEmitter::Distribution distribution);
	vec2d generatePointInRectangle(const vec2d& position, const vec2d& size, floatd angle, ParticleEmitter::Distribution distribution);

	uint32_t emitParticles(uint32_t emitterIndex, uint32_t count, floatd t, floatd tParent = 0.0, uint32_t parentEmitterIndex = NullId, uint32_t parentParticle = NullId);
	void createParticle(uint32_t emitterIndex, uint32_t p, floatd t, floatd tParent = 0.0, uint32_t parentEmitterIndex = NullId, uint32_t parentParticle = NullId);
	void destroyParticle(uint32_t emitterIndex, int32_t& p);

	const Effect* effect = nullptr;
	std::vector<ParticleData> particles;
	std::vector<uint32_t> numParticles;
	std::vector<floatd> numParticlesToEmit;
	uint32_t particleCapacity = 0;
	uint32_t particleId = 0;
	uint32_t seed = 0;
	floatd time = 0.0;

	std::unique_ptr<ParticleSimulation> particleSimulation;
	ForceSolver forceSolver;
	CollisionSolver collisionSolver;

	std::vector<std::vector<vec2d>> particlePositionSnapshot;

	std::mt19937 rng;
	std::uniform_int_distribution<int32_t> uniformIntDistribution = std::uniform_int_distribution<int32_t>(0);
	std::uniform_real_distribution<floatd> uniformDistribution = std::uniform_real_distribution<floatd>(0.0, 1.0);
	std::normal_distribution<floatd> normalDistribution = std::normal_distribution<floatd>(0.0, 0.3);
};
}