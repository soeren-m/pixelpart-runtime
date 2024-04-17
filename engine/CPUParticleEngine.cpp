#include "CPUParticleEngine.h"
#include "ParticleEmission.h"
#include "../common/Constants.h"
#include "../glm/glm/gtx/euler_angles.hpp"
#include "../glm/glm/gtx/rotate_vector.hpp"

namespace pixelpart {
CPUParticleEngine::CPUParticleEngine(const Effect& fx, uint32_t capacity) : ParticleEngine(fx), particleCapacity(capacity) {

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

	if(effect.particleEmitters.getCount() != particleEmitterGridIndices.size()) {
		particleEmitterGridIndices.assign(effect.particleEmitters.getCount(), 0u);
	}

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
					if(internalTime >= particleEmitter.lifetimeDuration - dt &&
					internalTime < particleEmitter.lifetimeDuration) {
						particleSpawnCount[particleTypeIndex] += particleType.numParticles.get();
					}

					break;
				}
			}
		}

		int32_t particlesSpawned = static_cast<int32_t>(particleSpawnCount[particleTypeIndex]);
		if(particlesSpawned > 0) {
			particlesSpawned = spawnParticles(particlesSpawned,
				nullId,
				particleTypeIndex, nullId,
				particleEmitterIndex,
				dt, localTimeWrapped, 0.0);

			particleSpawnCount[particleTypeIndex] -= static_cast<float_t>(particlesSpawned);
		}
	}

	for(uint32_t particleTypeIndex = 0u; particleTypeIndex < effect.particleTypes.getCount(); particleTypeIndex++) {
		const ParticleType& particleType = effect.particleTypes.getByIndex(particleTypeIndex);
		const ParticleEmitter& particleEmitter = effect.particleEmitters.get(particleType.parentId);
		const std::vector<uint32_t>& subTypes = particleSubTypes[particleTypeIndex];
		ParticleContainer& particleContainer = particleContainers[particleTypeIndex];
		ParticleWritePtr particleData = particleContainer.getParticleWritePtr();

		for(uint32_t p = 0u; p < particleContainer.getNumParticles(); p++) {
			particleData.life[p] += dt / particleData.lifespan[p];
		}

		if(!subTypes.empty()) {
			float_t localTime = time - particleEmitter.lifetimeStart;
			float_t localTimeWrapped = std::fmod(localTime, particleEmitter.lifetimeDuration);

			for(uint32_t p = 0u; p < particleContainer.getNumParticles(); p++) {
				for(uint32_t subParticleTypeIndex : subTypes) {
					const ParticleType& subParticleType = effect.particleTypes.getByIndex(subParticleTypeIndex);

					uint32_t subParticleEmitterIndex = effect.particleEmitters.findById(subParticleType.parentId);
					const ParticleEmitter& subParticleEmitter = effect.particleEmitters.getByIndex(subParticleEmitterIndex);
					float_t localTimeSubEmitter = particleData.life[p] * particleData.lifespan[p] - subParticleEmitter.lifetimeStart;
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
								if(particleData.life[p] > 1.0) {
									particleSpawnCount[subParticleTypeIndex] += subParticleType.numParticles.get();
								}

								break;
							}
						}
					}

					int32_t particlesSpawned = static_cast<int32_t>(particleSpawnCount[subParticleTypeIndex]);
					if(particlesSpawned > 0) {
						particlesSpawned = spawnParticles(particlesSpawned,
							p,
							subParticleTypeIndex, particleTypeIndex,
							subParticleEmitterIndex,
							dt, localTimeSubEmitterWrapped, localTimeWrapped);

						particleSpawnCount[subParticleTypeIndex] -= static_cast<float_t>(particlesSpawned);
					}
				}
			}
		}
	}

	for(ParticleContainer& particleContainer : particleContainers) {
		ParticleReadPtr particleData = particleContainer.getParticleReadPtr();

		for(uint32_t p = 0u; p < particleContainer.getNumParticles(); ) {
			if(particleData.life[p] > 1.0) {
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

#ifndef __EMSCRIPTEN__
		uint32_t numAvailableThreads = threadPool != nullptr ? static_cast<uint32_t>(threadPool->getNumThreads()) : 1u;
		uint32_t numActiveThreads = std::min(std::max(numParticles / numParticlesPerThread, 1u), numAvailableThreads);

		if(numActiveThreads > 1u) {
			uint32_t numParticlesPerThread = numParticles / numActiveThreads;
			uint32_t numThreadsWithLargerLoad = numParticles % numActiveThreads;
			uint32_t workgroupIndex = 0u;

			for(uint32_t threadIndex = 0u; threadIndex < numActiveThreads; threadIndex++) {
				uint32_t workgroupSize = (threadIndex < numActiveThreads - numThreadsWithLargerLoad)
					? numParticlesPerThread
					: numParticlesPerThread + 1u;

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

#else
		numActiveThreads = 1u;

#endif

		if(numActiveThreads <= 1u) {
			stepParticles(particleEmitter, particleType,
				particleContainer.getParticleWritePtr(), numParticles,
				time, dt);
		}

		if(!effect.is3d) {
			ParticleWritePtr particleData = particleContainer.getParticleWritePtr();
			for(uint32_t p = 0u; p < particleContainer.getNumParticles(); p++) {
				particleData.rotation[p].y = 0.0;
				particleData.rotation[p].z = 0.0;
				particleData.force[p].z = 0.0;
				particleData.velocity[p].z = 0.0;
				particleData.position[p].z =  0.0;
				particleData.globalPosition[p].z = 0.0;
			}
		}

		numTotalActiveThreads = std::max(numTotalActiveThreads, numActiveThreads);
	}
}
void CPUParticleEngine::restart(bool reset) {
	time = 0.0;

	if(reset) {
		particleId = 0u;

		for(ParticleContainer& particleContainer : particleContainers) {
			particleContainer.killAll();
		}
		for(float_t& count : particleSpawnCount) {
			count = 0.0;
		}
		for(uint32_t& index : particleEmitterGridIndices) {
			index = 0u;
		}
	}
}

float_t CPUParticleEngine::getTime() const {
	return time;
}

void CPUParticleEngine::applySeed(uint32_t seed) {
	activeSeed = seed;
	resetSeed();
}
void CPUParticleEngine::resetSeed() {
	rng = std::mt19937(activeSeed);
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

	spawnParticles(count,
		nullId,
		particleTypeIndex, nullId,
		particleEmitterIndex,
		0.0, localTime, 0.0);
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

uint32_t CPUParticleEngine::spawnParticles(uint32_t count, uint32_t pParent,
	uint32_t particleTypeIndex, uint32_t parentParticleTypeIndex, uint32_t particleEmitterIndex,
	float_t dt, float_t t, float_t tParent) {
	ParticleContainer& particleContainer = particleContainers[particleTypeIndex];
	count = particleContainer.spawn(count);

	for(uint32_t i = 0u; i < count; i++) {
		spawnParticle(
			particleContainer.getNumParticles() - count + i,
			pParent,
			particleTypeIndex,
			parentParticleTypeIndex,
			particleEmitterIndex,
			dt, t, tParent);
	}

	return count;
}
void CPUParticleEngine::spawnParticle(uint32_t p, uint32_t pParent,
	uint32_t particleTypeIndex, uint32_t parentParticleTypeIndex, uint32_t particleEmitterIndex,
	float_t dt, float_t t, float_t tParent) {
	const ParticleType& particleType = effect.particleTypes.getByIndex(particleTypeIndex);
	const ParticleEmitter& particleEmitter = effect.particleEmitters.getByIndex(particleEmitterIndex);
	ParticleWritePtr particles = particleContainers[particleTypeIndex].getParticleWritePtr();

	float_t alpha = t / particleEmitter.lifetimeDuration;

	particles.id[p] = particleId++;
	particles.parentId[p] = (pParent != nullId) ? particleContainers[parentParticleTypeIndex].getParticleReadPtr().id[pParent] : nullId;

	particles.life[p] = 0.0;
	particles.lifespan[p] = std::max(particleType.lifespan.get(alpha) + random::uniform(rng, -particleType.lifespanVariance.get(), +particleType.lifespanVariance.get()), 0.000001);

	vec3_t particleEmitterPosition = particleEmitter.position.get(alpha);
	vec3_t particleEmitterVelocity = (particleEmitterPosition - particleEmitter.position.get(alpha - dt / particleEmitter.lifetimeDuration)) / dt;

	vec3_t particleSpawnPosition = vec3_t(0.0);
	switch(particleEmitter.shape) {
		case ParticleEmitter::Shape::line:
			particleSpawnPosition = emitOnSegment(rng,
				particleEmitter.size.get(alpha).x,
				particleEmitter.distribution,
				particleEmitter.gridOrder,
				particleEmitter.gridSize[0],
				particleEmitterGridIndices[particleEmitterIndex]);
			break;
		case ParticleEmitter::Shape::ellipse:
			particleSpawnPosition = emitInEllipse(rng,
				particleEmitter.size.get(alpha),
				particleEmitter.distribution,
				particleEmitter.gridOrder,
				particleEmitter.gridSize[0], particleEmitter.gridSize[1],
				particleEmitterGridIndices[particleEmitterIndex]);
			break;
		case ParticleEmitter::Shape::rectangle:
			particleSpawnPosition = emitInRectangle(rng,
				particleEmitter.size.get(alpha),
				particleEmitter.distribution,
				particleEmitter.gridOrder,
				particleEmitter.gridSize[0], particleEmitter.gridSize[1],
				particleEmitterGridIndices[particleEmitterIndex]);
			break;
		case ParticleEmitter::Shape::path:
			particleSpawnPosition = emitOnPath(rng,
				particleEmitter.path,
				particleEmitter.distribution,
				particleEmitter.gridOrder,
				particleEmitter.gridSize[0],
				particleEmitterGridIndices[particleEmitterIndex]);
			break;
		case ParticleEmitter::Shape::ellipsoid:
			particleSpawnPosition = emitInEllipsoid(rng,
				particleEmitter.size.get(alpha),
				particleEmitter.distribution,
				particleEmitter.gridOrder,
				particleEmitter.gridSize[0], particleEmitter.gridSize[1], particleEmitter.gridSize[2],
				particleEmitterGridIndices[particleEmitterIndex]);
			break;
		case ParticleEmitter::Shape::cuboid:
			particleSpawnPosition = emitInCuboid(rng,
				particleEmitter.size.get(alpha),
				particleEmitter.distribution,
				particleEmitter.gridOrder,
				particleEmitter.gridSize[0], particleEmitter.gridSize[1], particleEmitter.gridSize[2],
				particleEmitterGridIndices[particleEmitterIndex]);
			break;
		case ParticleEmitter::Shape::cylinder:
			particleSpawnPosition = emitInCylinder(rng,
				particleEmitter.size.get(alpha),
				particleEmitter.distribution,
				particleEmitter.gridOrder,
				particleEmitter.gridSize[0], particleEmitter.gridSize[1], particleEmitter.gridSize[2],
				particleEmitterGridIndices[particleEmitterIndex]);
			break;
		default:
			break;
	}

	particleSpawnPosition = effect.is3d
		? rotate3d(particleSpawnPosition, particleEmitter.orientation.get(alpha))
		: rotate2d(particleSpawnPosition, particleEmitter.orientation.get(alpha).x);

	vec3_t particleSpawnCenter = pParent != nullId
		? (particleContainers[parentParticleTypeIndex].getParticleReadPtr().globalPosition[pParent] + particleEmitterPosition)
		: (particleType.positionRelative ? vec3_t(0.0) : particleEmitterPosition);

	particles.position[p] = particleSpawnCenter + particleSpawnPosition;
	particles.globalPosition[p] = particleType.positionRelative
		? particles.position[p] + particleEmitterPosition
		: particles.position[p];

	vec3_t particleParentVelocity = pParent != nullId
		? particleContainers[parentParticleTypeIndex].getParticleReadPtr().velocity[pParent]
		: particleEmitterVelocity;

	if(effect.is3d) {
		mat3_t directionMatrix = mat3_t(glm::yawPitchRoll(
			glm::radians(particleEmitter.direction.get(alpha).y + particleEmitter.spread.get(alpha) * random::uniform(rng, -0.5, +0.5)),
			glm::radians(particleEmitter.direction.get(alpha).z + particleEmitter.spread.get(alpha) * random::uniform(rng, -0.5, +0.5)),
			glm::radians(particleEmitter.direction.get(alpha).x + particleEmitter.spread.get(alpha) * random::uniform(rng, -0.5, +0.5))));

		switch(particleEmitter.directionMode) {
			case ParticleEmitter::DirectionMode::outwards:
				particles.velocity[p] = directionMatrix *
					((particleSpawnPosition != vec3_t(0.0)) ? glm::normalize(+particleSpawnPosition) : worldUpVector3);
				break;
			case ParticleEmitter::DirectionMode::inwards:
				particles.velocity[p] = directionMatrix *
					((particleSpawnPosition != vec3_t(0.0)) ? glm::normalize(-particleSpawnPosition) : worldUpVector3);
				break;
			case ParticleEmitter::DirectionMode::inherit:
				particles.velocity[p] = directionMatrix * glm::normalize(particleParentVelocity != vec3_t(0.0) ? +particleParentVelocity : worldUpVector3);
				break;
			case ParticleEmitter::DirectionMode::inherit_inverse:
				particles.velocity[p] = directionMatrix * glm::normalize(particleParentVelocity != vec3_t(0.0) ? -particleParentVelocity : worldUpVector3);
				break;
			default:
				particles.velocity[p] = directionMatrix * worldUpVector3;
				break;
		}
	}
	else {
		float_t direction = glm::radians(particleEmitter.direction.get(alpha).x + particleEmitter.spread.get(alpha) * random::uniform(rng,-0.5, +0.5));

		switch(particleEmitter.directionMode) {
			case ParticleEmitter::DirectionMode::outwards:
				particles.velocity[p] = vec3_t(glm::rotate(
					(particleSpawnPosition != vec3_t(0.0)) ? vec2_t(glm::normalize(+particleSpawnPosition)) : worldUpVector2,
					direction), 0.0);
				break;
			case ParticleEmitter::DirectionMode::inwards:
				particles.velocity[p] = vec3_t(glm::rotate(
					(particleSpawnPosition != vec3_t(0.0)) ? vec2_t(glm::normalize(-particleSpawnPosition)) : worldUpVector2,
					direction), 0.0);
				break;
			case ParticleEmitter::DirectionMode::inherit:
				particles.velocity[p] = vec3_t(glm::rotate(vec2_t(glm::normalize(particleParentVelocity != vec3_t(0.0) ? +particleParentVelocity : worldUpVector3)), direction), 0.0);
				break;
			case ParticleEmitter::DirectionMode::inherit_inverse:
				particles.velocity[p] = vec3_t(glm::rotate(vec2_t(glm::normalize(particleParentVelocity != vec3_t(0.0) ? -particleParentVelocity : worldUpVector3)), direction), 0.0);
				break;
			default:
				particles.velocity[p] = vec3_t(glm::rotate(worldUpVector2, direction), 0.0);
				break;
		}
	}

	particles.velocity[p] *= glm::mix(
		particleType.initialVelocity.get(alpha),
		glm::length(particleParentVelocity),
		particleType.inheritedVelocity.get(alpha)) + random::uniform(rng, -particleType.velocityVariance.get(), +particleType.velocityVariance.get());
	particles.force[p] = vec3_t(0.0);

	particles.initialRotation[p] = particleType.initialRotation.get(alpha) + vec3_t(
		random::uniform(rng, -particleType.rotationVariance.get().x, +particleType.rotationVariance.get().x),
		random::uniform(rng, -particleType.rotationVariance.get().y, +particleType.rotationVariance.get().y),
		random::uniform(rng, -particleType.rotationVariance.get().z, +particleType.rotationVariance.get().z));
	particles.initialAngularVelocity[p] = vec3_t(
		random::uniform(rng, -particleType.angularVelocityVariance.get().x, +particleType.angularVelocityVariance.get().x),
		random::uniform(rng, -particleType.angularVelocityVariance.get().y, +particleType.angularVelocityVariance.get().y),
		random::uniform(rng, -particleType.angularVelocityVariance.get().z, +particleType.angularVelocityVariance.get().z));
	particles.rotation[p] = particles.initialRotation[p];

	particles.initialSize[p] = particleType.initialSize.get(alpha) + random::uniform(rng, -particleType.sizeVariance.get(), +particleType.sizeVariance.get());
	particles.size[p] = particleType.size.get() * particles.initialSize[p];

	particles.initialColor[p] = vec4_t(
		random::uniform(rng, -particleType.colorVariance.get().x, +particleType.colorVariance.get().x),
		random::uniform(rng, -particleType.colorVariance.get().y, +particleType.colorVariance.get().y),
		random::uniform(rng, -particleType.colorVariance.get().z, +particleType.colorVariance.get().z),
		particleType.initialOpacity.get(alpha) + random::uniform(rng, -particleType.opacityVariance.get(), +particleType.opacityVariance.get()));
	particles.color[p] = vec4_t(vec3_t(particleType.color.get()), particleType.opacity.get());
}
}