#include "CPUParticleEngine.h"

namespace pixelpart {
CPUParticleEngine::CPUParticleEngine(const Effect& fx, uint32_t capacity) : ParticleEngine(fx), particleCapacity(capacity), particleGenerator(fx, particleContainers) {

}
#ifndef __EMSCRIPTEN__
CPUParticleEngine::CPUParticleEngine(const Effect& fx, uint32_t capacity, std::shared_ptr<ThreadPool> thPool) : CPUParticleEngine(fx, capacity) {
	threadPool = thPool;
}
#endif

void CPUParticleEngine::step(float_t dt) {
	time += dt;

	if(effect.particleTypes.getCount() != particleContainers.size()) {
		particleSpawnCount.assign(effect.particleTypes.getCount(), 0.0);
		particleContainers.assign(effect.particleTypes.getCount(), ParticleContainer(particleCapacity));
	}

	particleGenerator.prepare();

	std::vector<std::vector<uint32_t>> particleSubTypes(effect.particleTypes.getCount(), std::vector<uint32_t>());
	for(uint32_t particleTypeIndex = 0u; particleTypeIndex < effect.particleTypes.getCount(); particleTypeIndex++) {
		id_t parentParticleEmitterId = effect.particleTypes.getByIndex(particleTypeIndex).parentId;

		if(effect.particleEmitters.contains(parentParticleEmitterId)) {
			const ParticleEmitter& parentParticleEmitter = effect.particleEmitters.get(parentParticleEmitterId);

			uint32_t parentParticleTypeIndex = effect.particleTypes.findById(parentParticleEmitter.parentId);
			if(parentParticleTypeIndex != nullId) {
				particleSubTypes[parentParticleTypeIndex].push_back(particleTypeIndex);
			}
		}
	}

	for(uint32_t particleTypeIndex = 0u; particleTypeIndex < effect.particleTypes.getCount(); particleTypeIndex++) {
		const ParticleType& particleType = effect.particleTypes.getByIndex(particleTypeIndex);

		uint32_t particleEmitterIndex = effect.particleEmitters.findById(particleType.parentId);
		if(particleEmitterIndex == nullId) {
			continue;
		}

		const ParticleEmitter& particleEmitter = effect.particleEmitters.getByIndex(particleEmitterIndex);
		if(particleEmitter.parentId != nullId) {
			continue;
		}

		float_t localTime = time - particleEmitter.lifetimeStart;
		float_t localTimeWrapped = std::fmod(localTime, particleEmitter.lifetimeDuration);
		float_t internalTime = particleEmitter.repeat ? localTimeWrapped : localTime;

		if(localTime > 0.0) {
			switch(particleEmitter.emissionMode) {
				case ParticleEmitter::EmissionMode::continuous: {
					if(particleEmitter.repeat || localTime < particleEmitter.lifetimeDuration) {
						particleSpawnCount[particleTypeIndex] += particleType.numParticles.get(localTimeWrapped / particleEmitter.lifetimeDuration) * dt;
					}

					break;
				}

				case ParticleEmitter::EmissionMode::burst_start: {
					if(internalTime <= dt) {
						particleSpawnCount[particleTypeIndex] += particleType.numParticles.get();
					}

					break;
				}

				case ParticleEmitter::EmissionMode::burst_end: {
					if(internalTime >= particleEmitter.lifetimeDuration - dt && internalTime < particleEmitter.lifetimeDuration) {
						particleSpawnCount[particleTypeIndex] += particleType.numParticles.get();
					}

					break;
				}
			}
		}

		int32_t particlesSpawned = static_cast<int32_t>(particleSpawnCount[particleTypeIndex]);
		if(particlesSpawned > 0) {
			particlesSpawned = particleGenerator.generate(particlesSpawned,
				nullId,
				particleTypeIndex, nullId,
				particleEmitterIndex,
				dt, localTimeWrapped);

			particleSpawnCount[particleTypeIndex] -= static_cast<float_t>(particlesSpawned);
		}
	}

	for(uint32_t particleTypeIndex = 0u; particleTypeIndex < effect.particleTypes.getCount(); particleTypeIndex++) {
		const ParticleType& particleType = effect.particleTypes.getByIndex(particleTypeIndex);
		const ParticleEmitter& particleEmitter = effect.particleEmitters.get(particleType.parentId);
		ParticleContainer& particleContainer = particleContainers[particleTypeIndex];
		ParticleWritePtr particles = particleContainer.getParticleWritePtr();

		for(uint32_t p = 0u; p < particleContainer.getNumParticles(); p++) {
			particles.life[p] += dt / particles.lifespan[p];
		}

		const std::vector<uint32_t>& subTypes = particleSubTypes[particleTypeIndex];
		if(subTypes.empty()) {
			continue;
		}

		for(uint32_t p = 0u; p < particleContainer.getNumParticles(); p++) {
			for(uint32_t subParticleTypeIndex : subTypes) {
				const ParticleType& subParticleType = effect.particleTypes.getByIndex(subParticleTypeIndex);

				uint32_t subParticleEmitterIndex = effect.particleEmitters.findById(subParticleType.parentId);
				const ParticleEmitter& subParticleEmitter = effect.particleEmitters.getByIndex(subParticleEmitterIndex);
				float_t localTimeSubEmitter = particles.life[p] * particles.lifespan[p] - subParticleEmitter.lifetimeStart;
				float_t localTimeSubEmitterWrapped = std::fmod(localTimeSubEmitter, subParticleEmitter.lifetimeDuration);
				float_t internalTimeSubEmitter = subParticleEmitter.repeat ? localTimeSubEmitterWrapped : localTimeSubEmitter;

				if(localTimeSubEmitter > 0.0) {
					switch(subParticleEmitter.emissionMode) {
						case ParticleEmitter::EmissionMode::continuous: {
							if(subParticleEmitter.repeat || localTimeSubEmitter < subParticleEmitter.lifetimeDuration) {
								particleSpawnCount[subParticleTypeIndex] += subParticleType.numParticles.get(localTimeSubEmitterWrapped / particleEmitter.lifetimeDuration) * dt;
							}

							break;
						}

						case ParticleEmitter::EmissionMode::burst_start: {
							if(internalTimeSubEmitter <= dt) {
								particleSpawnCount[subParticleTypeIndex] += subParticleType.numParticles.get();
							}

							break;
						}

						case ParticleEmitter::EmissionMode::burst_end: {
							if(particles.life[p] > 1.0) {
								particleSpawnCount[subParticleTypeIndex] += subParticleType.numParticles.get();
							}

							break;
						}
					}
				}

				int32_t particlesSpawned = static_cast<int32_t>(particleSpawnCount[subParticleTypeIndex]);
				if(particlesSpawned > 0) {
					particlesSpawned = particleGenerator.generate(particlesSpawned,
						p,
						subParticleTypeIndex, particleTypeIndex,
						subParticleEmitterIndex,
						dt, localTimeSubEmitterWrapped);

					particleSpawnCount[subParticleTypeIndex] -= static_cast<float_t>(particlesSpawned);
				}
			}
		}
	}

	for(ParticleContainer& particleContainer : particleContainers) {
		ParticleReadPtr particles = particleContainer.getParticleReadPtr();

		for(uint32_t p = 0u; p < particleContainer.getNumParticles(); ) {
			if(particles.life[p] > 1.0) {
				particleContainer.kill(p);
			}
			else {
				p++;
			}
		}
	}

	numTotalActiveThreads = 1u;

	sizeSolver.refresh(effect);
	colorSolver.refresh(effect);
	accelerationSolver.refresh(effect);
	forceSolver.refresh(effect);
	collisionSolver.refresh(effect);
	motionPathSolver.refresh(effect);
	rotationSolver.refresh(effect);
	integrationSolver.refresh(effect);

	for(uint32_t particleTypeIndex = 0u; particleTypeIndex < effect.particleTypes.getCount(); particleTypeIndex++) {
		const ParticleType& particleType = effect.particleTypes.getByIndex(particleTypeIndex);
		const ParticleEmitter& particleEmitter = effect.particleEmitters.get(particleType.parentId);
		ParticleContainer& particleContainer = particleContainers[particleTypeIndex];

		uint32_t numParticles = particleContainer.getNumParticles();
		uint32_t numActiveThreads = 1u;

#ifndef __EMSCRIPTEN__
		uint32_t numAvailableThreads = threadPool != nullptr ? static_cast<uint32_t>(threadPool->getNumThreads()) : 1u;
		numActiveThreads = std::min(std::max(numParticles / numParticlesPerThread, 1u), numAvailableThreads);

		if(numActiveThreads > 1u) {
			uint32_t numParticlesInThisThread = numParticles / numActiveThreads;
			uint32_t numThreadsWithLargerLoad = numParticles % numActiveThreads;
			uint32_t workgroupIndex = 0u;

			for(uint32_t threadIndex = 0u; threadIndex < numActiveThreads; threadIndex++) {
				uint32_t workgroupSize = (threadIndex < numActiveThreads - numThreadsWithLargerLoad)
					? numParticlesInThisThread
					: numParticlesInThisThread + 1u;

				threadPool->enqueue(threadIndex, &CPUParticleEngine::stepParticles, this,
					particleEmitter, particleType,
					particleContainer.getParticleWritePtr(workgroupIndex), workgroupSize,
					time, dt);

				workgroupIndex += workgroupSize;
			}

			for(uint32_t threadIndex = 0u; threadIndex < numActiveThreads; threadIndex++) {
				threadPool->wait(threadIndex);
			}
		}
#endif

		if(numActiveThreads <= 1u) {
			stepParticles(particleEmitter, particleType,
				particleContainer.getParticleWritePtr(), numParticles,
				time, dt);
		}

		if(!effect.is3d) {
			ParticleWritePtr particles = particleContainer.getParticleWritePtr();
			for(uint32_t p = 0u; p < particleContainer.getNumParticles(); p++) {
				particles.rotation[p].y = 0.0;
				particles.rotation[p].z = 0.0;
				particles.force[p].z = 0.0;
				particles.velocity[p].z = 0.0;
				particles.position[p].z =  0.0;
				particles.globalPosition[p].z = 0.0;
			}
		}

		numTotalActiveThreads = std::max(numTotalActiveThreads, numActiveThreads);
	}
}
void CPUParticleEngine::restart(bool reset) {
	time = 0.0;

	if(reset) {
		for(ParticleContainer& particleContainer : particleContainers) {
			particleContainer.killAll();
		}
		for(float_t& count : particleSpawnCount) {
			count = 0.0;
		}

		particleGenerator.reset();
	}
}

float_t CPUParticleEngine::getTime() const {
	return time;
}

void CPUParticleEngine::applySeed(uint32_t seed) {
	activeSeed = seed;
	particleGenerator.applySeed(seed);
}
void CPUParticleEngine::resetSeed() {
	particleGenerator.applySeed(activeSeed);
}

void CPUParticleEngine::spawnParticles(id_t particleTypeId, uint32_t count) {
	uint32_t particleTypeIndex = effect.particleTypes.findById(particleTypeId);
	if(particleTypeIndex == nullId) {
		return;
	}

	const ParticleType& particleType = effect.particleTypes.getByIndex(particleTypeIndex);
	uint32_t particleEmitterIndex = effect.particleEmitters.findById(particleType.parentId);
	if(particleEmitterIndex == nullId) {
		return;
	}

	const ParticleEmitter& particleEmitter = effect.particleEmitters.getByIndex(particleEmitterIndex);
	float_t localTime = std::fmod(time - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration);

	particleGenerator.generate(count,
		nullId,
		particleTypeIndex, nullId,
		particleEmitterIndex,
		0.0, localTime);
}


uint32_t CPUParticleEngine::getNumParticles() const {
	uint32_t count = 0u;
	for(const ParticleContainer& particleContainer : particleContainers) {
		count += particleContainer.getNumParticles();
	}

	return count;
}
uint32_t CPUParticleEngine::getNumParticles(uint32_t particleTypeIndex) const {
	if(particleTypeIndex >= particleContainers.size()) {
		return 0u;
	}

	return particleContainers[particleTypeIndex].getNumParticles();
}
ParticleReadPtr CPUParticleEngine::getParticles(uint32_t particleTypeIndex) const {
	if(particleTypeIndex >= particleContainers.size()) {
		return ParticleReadPtr();
	}

	return particleContainers[particleTypeIndex].getParticleReadPtr();
}

uint32_t CPUParticleEngine::getParticleCapacity() const {
	return particleCapacity;
}

uint32_t CPUParticleEngine::getNumActiveThreads() const {
	return numTotalActiveThreads;
}

void CPUParticleEngine::stepParticles(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleWritePtr particles, uint32_t numParticles,
	float_t t, float_t dt) const {
	sizeSolver.solve(particleEmitter, particleType, particles, numParticles, t, dt);
	colorSolver.solve(particleEmitter, particleType, particles, numParticles, t, dt);
	accelerationSolver.solve(particleEmitter, particleType, particles, numParticles, t, dt);
	forceSolver.solve(particleEmitter, particleType, particles, numParticles, t, dt);
	collisionSolver.solve(particleEmitter, particleType, particles, numParticles, t, dt);
	motionPathSolver.solve(particleEmitter, particleType, particles, numParticles, t, dt);
	rotationSolver.solve(particleEmitter, particleType, particles, numParticles, t, dt);
	integrationSolver.solve(particleEmitter, particleType, particles, numParticles, t, dt);
}
}