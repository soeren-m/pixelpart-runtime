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
		particleContainers.assign(effect.particleTypes.getCount(), ParticleContainer(particleCapacity));
		emissionCount.assign(effect.particleTypes.getCount(), 0.0);
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

		float_t timeSinceStart = time - particleEmitter.lifetimeStart;
		if(timeSinceStart <= 0.0) {
			continue;
		}

		float_t repeatedTime = std::fmod(timeSinceStart, particleEmitter.lifetimeDuration);
		float_t emissionTime = particleEmitter.repeat ? repeatedTime : timeSinceStart;

		switch(particleEmitter.emissionMode) {
			case ParticleEmitter::EmissionMode::continuous: {
				if(emissionTime <= particleEmitter.lifetimeDuration) {
					emissionCount[particleTypeIndex] += particleType.numParticles.get(repeatedTime / particleEmitter.lifetimeDuration) * dt;
				}

				break;
			}
			case ParticleEmitter::EmissionMode::burst_start: {
				if(emissionTime <= dt) {
					emissionCount[particleTypeIndex] += particleType.numParticles.get();
				}

				break;
			}
			case ParticleEmitter::EmissionMode::burst_end: {
				if(emissionTime >= particleEmitter.lifetimeDuration - dt && emissionTime < particleEmitter.lifetimeDuration) {
					emissionCount[particleTypeIndex] += particleType.numParticles.get();
				}

				break;
			}
			default: {
				break;
			}
		}

		int32_t particlesEmitted = static_cast<int32_t>(emissionCount[particleTypeIndex]);
		if(particlesEmitted > 0) {
			particlesEmitted = particleGenerator.generate(particlesEmitted,
				nullId, particleTypeIndex, nullId, particleEmitterIndex, dt, repeatedTime);

			emissionCount[particleTypeIndex] -= static_cast<float_t>(particlesEmitted);
		}
	}

	for(uint32_t particleTypeIndex = 0u; particleTypeIndex < effect.particleTypes.getCount(); particleTypeIndex++) {
		const ParticleType& particleType = effect.particleTypes.getByIndex(particleTypeIndex);
		const ParticleEmitter& particleEmitter = effect.particleEmitters.get(particleType.parentId);
		ParticleContainer& particleContainer = particleContainers[particleTypeIndex];
		ParticleWritePtr particles = particleContainer.getParticleWritePtr();

		const std::vector<uint32_t>& subTypes = particleSubTypes[particleTypeIndex];
		if(subTypes.empty()) {
			continue;
		}

		for(uint32_t p = 0u; p < particleContainer.getNumParticles(); p++) {
			for(uint32_t subParticleTypeIndex : subTypes) {
				const ParticleType& subParticleType = effect.particleTypes.getByIndex(subParticleTypeIndex);

				uint32_t subParticleEmitterIndex = effect.particleEmitters.findById(subParticleType.parentId);
				const ParticleEmitter& subParticleEmitter = effect.particleEmitters.getByIndex(subParticleEmitterIndex);

				float_t subTimeSinceStart = particles.life[p] * particles.lifespan[p] - subParticleEmitter.lifetimeStart;
				if(subTimeSinceStart <= 0.0) {
					continue;
				}

				float_t subRepeatedTime = std::fmod(subTimeSinceStart, subParticleEmitter.lifetimeDuration);
				float_t subEmissionTime = subParticleEmitter.repeat ? subRepeatedTime : subTimeSinceStart;

				switch(subParticleEmitter.emissionMode) {
					case ParticleEmitter::EmissionMode::continuous: {
						if(subEmissionTime <= subParticleEmitter.lifetimeDuration) {
							emissionCount[subParticleTypeIndex] += subParticleType.numParticles.get(subRepeatedTime / particleEmitter.lifetimeDuration) * dt;
						}

						break;
					}
					case ParticleEmitter::EmissionMode::burst_start: {
						if(subEmissionTime <= dt) {
							emissionCount[subParticleTypeIndex] += subParticleType.numParticles.get();
						}

						break;
					}
					case ParticleEmitter::EmissionMode::burst_end: {
						if(particles.life[p] > 1.0) {
							emissionCount[subParticleTypeIndex] += subParticleType.numParticles.get();
						}

						break;
					}
					default: {
						break;
					}
				}

				int32_t particlesEmitted = static_cast<int32_t>(emissionCount[subParticleTypeIndex]);
				if(particlesEmitted > 0) {
					particlesEmitted = particleGenerator.generate(particlesEmitted,
						p, subParticleTypeIndex, particleTypeIndex, subParticleEmitterIndex, dt, subRepeatedTime);

					emissionCount[subParticleTypeIndex] -= static_cast<float_t>(particlesEmitted);
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
	lifeSolver.refresh(effect);

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
		for(float_t& count : emissionCount) {
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
	float_t objectTime = std::fmod(time - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration);

	particleGenerator.generate(count,
		nullId,
		particleTypeIndex, nullId,
		particleEmitterIndex,
		0.0, objectTime);
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
	lifeSolver.solve(particleEmitter, particleType, particles, numParticles, t, dt);
}
}