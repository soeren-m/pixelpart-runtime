#include "CPUParticleEngine.h"
#include <cmath>
#include <algorithm>

namespace pixelpart {
CPUParticleEngine::CPUParticleEngine(const Effect& fx, uint32_t capacity) : ParticleEngine(fx), particleCap(capacity), particleGenerator(fx, particleCollections) {

}
#ifdef PIXELPART_MULTITHREADING
CPUParticleEngine::CPUParticleEngine(const Effect& fx, uint32_t capacity, std::shared_ptr<ThreadPool> thPool) : CPUParticleEngine(fx, capacity) {
	threadPool = thPool;
}
#endif

void CPUParticleEngine::step(float_t dt) {
	time += dt;

	// TODO
	if(particleEffect.particleTypes().count() != particleCollections.size()) {
		particleCollections.assign(particleEffect.particleTypes().count(), ParticleCollection(particleCap));
		emissionCount.assign(particleEffect.particleTypes().count(), 0.0);
	}

	particleGenerator.generate(dt, time);

	

	



	for(ParticleCollection& particleCollection : particleCollections) {
		ParticleCollection::ReadPtr particles = particleCollection.readPtr();

		for(uint32_t p = 0u; p < particleCollection.count(); ) {
			if(particles.life[p] > 1.0) {
				particleCollection.remove(p);
			}
			else {
				p++;
			}
		}
	}

	totalActiveThreadCount = 1u;

	sizeSolver.prepare(particleEffect);
	colorSolver.prepare(particleEffect);
	accelerationSolver.prepare(particleEffect);
	forceSolver.prepare(particleEffect);
	collisionSolver.prepare(particleEffect);
	motionPathSolver.prepare(particleEffect);
	rotationSolver.prepare(particleEffect);
	integrationSolver.prepare(particleEffect);
	lifeSolver.prepare(particleEffect);

	for(uint32_t particleTypeIndex = 0u; particleTypeIndex < particleEffect.particleTypes().count(); particleTypeIndex++) {
		const ParticleType& particleType = particleEffect.particleTypes().atIndex(particleTypeIndex);
		const ParticleEmitter& particleEmitter = particleEffect.particleEmitters().at(particleType.parentId());
		ParticleCollection& particleCollection = particleCollections[particleTypeIndex];

		uint32_t particleCount = particleCollection.count();
		uint32_t activeThreadCount = 1u;

#ifdef PIXELPART_MULTITHREADING
		uint32_t availableThreadCount = threadPool != nullptr ? static_cast<uint32_t>(threadPool->threadCount()) : 1u;
		activeThreadCount = std::min(std::max(particleCount / particleCountPerThread, 1u), availableThreadCount);

		if(activeThreadCount > 1u) {
			uint32_t particleCountInThisThread = particleCount / activeThreadCount;
			uint32_t threadsWithLargerLoadCount = particleCount % activeThreadCount;
			uint32_t workgroupIndex = 0u;

			for(uint32_t threadIndex = 0u; threadIndex < activeThreadCount; threadIndex++) {
				uint32_t workgroupSize = (threadIndex < activeThreadCount - threadsWithLargerLoadCount)
					? particleCountInThisThread
					: particleCountInThisThread + 1u;

				threadPool->enqueue(threadIndex, &CPUParticleEngine::stepParticles, this,
					particleEmitter, particleType,
					particleCollection.writePtr(workgroupIndex), workgroupSize,
					time, dt);

				workgroupIndex += workgroupSize;
			}

			for(uint32_t threadIndex = 0u; threadIndex < activeThreadCount; threadIndex++) {
				threadPool->wait(threadIndex);
			}
		}
#endif

		if(activeThreadCount <= 1u) {
			stepParticles(particleEmitter, particleType,
				particleCollection.writePtr(), particleCount,
				time, dt);
		}

		if(!particleEffect.is3d()) {
			ParticleCollection::WritePtr particles = particleCollection.writePtr();
			for(uint32_t p = 0u; p < particleCollection.count(); p++) {
				particles.rotation[p].y = 0.0;
				particles.rotation[p].z = 0.0;
				particles.force[p].z = 0.0;
				particles.velocity[p].z = 0.0;
				particles.position[p].z =  0.0;
				particles.globalPosition[p].z = 0.0;
			}
		}

		totalActiveThreadCount = std::max(totalActiveThreadCount, activeThreadCount);
	}
}
void CPUParticleEngine::restart(bool reset) {
	time = 0.0;

	if(reset) {
		for(ParticleCollection& particleCollection : particleCollections) {
			particleCollection.removeAll();
		}
		for(float_t& count : emissionCount) {
			count = 0.0;
		}

		particleGenerator.reset();
	}
}

float_t CPUParticleEngine::currentTime() const {
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
	uint32_t particleTypeIndex = particleEffect.particleTypes().indexOf(particleTypeId);
	if(particleTypeIndex == id_t::nullValue) {
		return;
	}

	const ParticleType& particleType = particleEffect.particleTypes().atIndex(particleTypeIndex);
	uint32_t particleEmitterIndex = particleEffect.particleEmitters().indexOf(particleType.parentId());
	if(particleEmitterIndex == id_t::nullValue) {
		return;
	}

	const ParticleEmitter& particleEmitter = particleEffect.particleEmitters().atIndex(particleEmitterIndex);
	float_t objectTime = std::fmod(time - particleEmitter.start(), particleEmitter.duration());

	particleGenerator.generate(count,
		id_t::nullValue,
		particleTypeIndex, id_t::nullValue,
		particleEmitterIndex,
		0.0, objectTime);
}

uint32_t CPUParticleEngine::particleCount() const {
	uint32_t count = 0u;
	for(const ParticleCollection& particleCollection : particleCollections) {
		count += particleCollection.count();
	}

	return count;
}
uint32_t CPUParticleEngine::particleCount(uint32_t particleTypeIndex) const {
	if(particleTypeIndex >= particleCollections.size()) {
		return 0u;
	}

	return particleCollections[particleTypeIndex].count();
}

ParticleCollection::ReadPtr CPUParticleEngine::particles(uint32_t particleTypeIndex) const {
	if(particleTypeIndex >= particleCollections.size()) {
		return ParticleCollection::ReadPtr();
	}

	return particleCollections[particleTypeIndex].readPtr();
}

uint32_t CPUParticleEngine::particleCapacity() const {
	return particleCap;
}

uint32_t CPUParticleEngine::activeThreadCount() const {
	return totalActiveThreadCount;
}

void CPUParticleEngine::stepParticles(const SceneGraph& sceneGraph, const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleCollection::WritePtr particles, uint32_t particleCount,
	float_t t, float_t dt) const {
	sizeSolver.solve(sceneGraph, particleEmitter, particleType, particles, particleCount, t, dt);
	colorSolver.solve(sceneGraph, particleEmitter, particleType, particles, particleCount, t, dt);
	accelerationSolver.solve(sceneGraph, particleEmitter, particleType, particles, particleCount, t, dt);
	forceSolver.solve(sceneGraph, particleEmitter, particleType, particles, particleCount, t, dt);
	collisionSolver.solve(sceneGraph, particleEmitter, particleType, particles, particleCount, t, dt);
	motionPathSolver.solve(sceneGraph, particleEmitter, particleType, particles, particleCount, t, dt);
	rotationSolver.solve(sceneGraph, particleEmitter, particleType, particles, particleCount, t, dt);
	integrationSolver.solve(sceneGraph, particleEmitter, particleType, particles, particleCount, t, dt);
	lifeSolver.solve(sceneGraph, particleEmitter, particleType, particles, particleCount, t, dt);
}
}