#include "ParticleEngine.h"
#include "ParticleEmission.h"
#include "ParticleSimulationCPU.h"

namespace pixelpart {
ParticleEngine::ParticleEngine() : ParticleEngine(nullptr, 1000) {

}
ParticleEngine::ParticleEngine(const Effect* fx, uint32_t capacity, uint32_t maxNumThreads) : effect(fx), particleCapacity(capacity) {
	particleSimulation = std::unique_ptr<ParticleSimulationCPU>(new ParticleSimulationCPU(maxNumThreads));

	resetSeed();
	updateSolvers();
}

void ParticleEngine::step(floatd dt) {
	time += dt;

	if(!effect) {
		return;
	}

	for(uint32_t particleTypeIndex = 0; particleTypeIndex < effect->particleTypes.getCount(); particleTypeIndex++) {
		const ParticleType& particleType = effect->particleTypes.getByIndex(particleTypeIndex);

		uint32_t particleEmitterIndex = effect->particleEmitters.findById(particleType.parentId);
		if(particleEmitterIndex == nullId) {
			continue;
		}

		const ParticleEmitter& particleEmitter = effect->particleEmitters.getByIndex(particleEmitterIndex);
		if(particleEmitter.parentId != nullId) {
			continue;
		}

		floatd localTime = time - particleEmitter.lifetimeStart;
		floatd localTimeWrapped = std::fmod(localTime, particleEmitter.lifetimeDuration);
		floatd internalTime = particleEmitter.repeat ? localTimeWrapped : localTime;

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
				localTimeWrapped, 0.0);

			particleSpawnCount[particleTypeIndex] -= static_cast<floatd>(particlesSpawned);
		}
	}

	for(uint32_t particleTypeIndex = 0; particleTypeIndex < effect->particleTypes.getCount(); particleTypeIndex++) {
		const ParticleType& particleType = effect->particleTypes.getByIndex(particleTypeIndex);
		const ParticleEmitter& particleEmitter = effect->particleEmitters.get(particleType.parentId);
		const std::vector<uint32_t>& subTypes = particleSubTypes[particleTypeIndex];
		ParticleContainer& particleContainer = particleContainers[particleTypeIndex];
		ParticleData& particleData = particleContainer.get();

		for(uint32_t p = 0; p < particleContainer.getNumParticles(); p++) {
			particleData.life[p] += dt / particleData.lifespan[p];
		}

		if(!subTypes.empty()) {
			floatd localTime = time - particleEmitter.lifetimeStart;
			floatd localTimeWrapped = std::fmod(localTime, particleEmitter.lifetimeDuration);

			for(uint32_t p = 0; p < particleContainer.getNumParticles(); p++) {
				for(uint32_t subParticleTypeIndex : subTypes) {
					const ParticleType& subParticleType = effect->particleTypes.getByIndex(subParticleTypeIndex);

					uint32_t subParticleEmitterIndex = effect->particleEmitters.findById(subParticleType.parentId);
					const ParticleEmitter& subParticleEmitter = effect->particleEmitters.getByIndex(subParticleEmitterIndex);
					floatd localTimeSubEmitter = particleData.life[p] * particleData.lifespan[p] - subParticleEmitter.lifetimeStart;
					floatd localTimeSubEmitterWrapped = std::fmod(localTimeSubEmitter, subParticleEmitter.lifetimeDuration);
					floatd internalTimeSubEmitter = subParticleEmitter.repeat ? localTimeSubEmitterWrapped : localTimeSubEmitter;

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
							localTimeSubEmitterWrapped, localTimeWrapped);

						particleSpawnCount[subParticleTypeIndex] -= static_cast<floatd>(particlesSpawned);
					}
				}
			}
		}
	}

	for(ParticleContainer& particleContainer : particleContainers) {
		for(uint32_t p = 0; p < particleContainer.getNumParticles(); ) {
			if(particleContainer.get().life[p] > 1.0) {
				particleContainer.kill(p);
			}
			else {
				p++;
			}
		}
	}

	numActiveThreads = 1;

	for(uint32_t particleTypeIndex = 0; particleTypeIndex < effect->particleTypes.getCount(); particleTypeIndex++) {
		const ParticleType& particleType = effect->particleTypes.getByIndex(particleTypeIndex);
		const ParticleEmitter& particleEmitter = effect->particleEmitters.get(particleType.parentId);
		ParticleContainer& particleContainer = particleContainers[particleTypeIndex];

		particleSimulation->simulate(
			particleEmitter,
			particleType,
			particleContainer.get(),
			particleContainer.getNumParticles(),
			forceSolver,
			collisionSolver,
			time,
			dt);

		if(!effect->is3d) {
			ParticleData& particleData = particleContainer.get();
			for(uint32_t p = 0; p < particleContainer.getNumParticles(); p++) {
				particleData.rotation[p].y = 0.0;
				particleData.rotation[p].z = 0.0;
				particleData.force[p].z = 0.0;
				particleData.velocity[p].z = 0.0;
				particleData.position[p].z =  0.0;
				particleData.globalPosition[p].z = 0.0;
			}
		}

		numActiveThreads = std::max(numActiveThreads, particleSimulation->getNumActiveThreads());
	}
}

void ParticleEngine::reset() {
	time = 0.0;
}
void ParticleEngine::restart() {
	time = 0.0;
	particleId = 0;

	for(ParticleContainer& particleContainer : particleContainers) {
		particleContainer.kill();
	}
	for(floatd& count : particleSpawnCount) {
		count = 0.0;
	}
}
floatd ParticleEngine::getTime() const {
	return time;
}

void ParticleEngine::spawnParticles(uint32_t particleTypeId, uint32_t count) {
	uint32_t particleTypeIndex = effect->particleTypes.findById(particleTypeId);
	if(!effect || particleTypeIndex == nullId) {
		return;
	}

	const ParticleType& particleType = effect->particleTypes.getByIndex(particleTypeIndex);
	uint32_t particleEmitterIndex = effect->particleEmitters.findById(particleType.parentId);
	if(particleEmitterIndex == nullId) {
		return;
	}

	const ParticleEmitter& particleEmitter = effect->particleEmitters.getByIndex(particleEmitterIndex);
	floatd localTime = std::fmod(time - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration);

	spawnParticles(count,
		nullId,
		particleTypeIndex, nullId,
		particleEmitterIndex,
		localTime, 0.0);
}

void ParticleEngine::setSeed(uint32_t sd) {
	seed = sd;
	resetSeed();
}
void ParticleEngine::resetSeed() {
	rng = std::mt19937(seed);
}

void ParticleEngine::setEffect(const Effect* fx) {
	effect = fx;

	updateSolvers();
}
const Effect* ParticleEngine::getEffect() const {
	return effect;
}

uint32_t ParticleEngine::getParticleCapacity() const {
	return particleCapacity;
}
uint32_t ParticleEngine::getNumParticles() const {
	uint32_t count = 0;
	for(const ParticleContainer& particleContainer : particleContainers) {
		count += particleContainer.getNumParticles();
	}

	return count;
}
uint32_t ParticleEngine::getNumParticles(uint32_t particleTypeIndex) const {
	return particleContainers[particleTypeIndex].getNumParticles();
}

const ParticleData& ParticleEngine::getParticles(uint32_t particleTypeIndex) const {
	return particleContainers[particleTypeIndex].get();
}

uint32_t ParticleEngine::getNumActiveThreads() const {
	return numActiveThreads;
}

void ParticleEngine::updateSolvers() {
	updateParticleSolver();
	updateForceSolver();
	updateCollisionSolver();
}
void ParticleEngine::updateParticleSolver() {
	particleContainers.clear();
	particleSubTypes.clear();
	particleSpawnCount.clear();

	if(effect) {
		particleContainers.resize(effect->particleTypes.getCount());
		particleSubTypes.resize(effect->particleTypes.getCount());
		particleSpawnCount.resize(effect->particleTypes.getCount());

		for(ParticleContainer& particleContainer : particleContainers) {
			particleContainer.reserve(particleCapacity);
		}

		for(uint32_t particleTypeIndex = 0; particleTypeIndex < effect->particleTypes.getCount(); particleTypeIndex++) {
			uint32_t parentParticleEmitterId = effect->particleTypes.getByIndex(particleTypeIndex).parentId;

			if(effect->particleEmitters.contains(parentParticleEmitterId)) {
				const ParticleEmitter& parentParticleEmitter = effect->particleEmitters.get(parentParticleEmitterId);

				uint32_t parentParticleTypeIndex = effect->particleTypes.findById(parentParticleEmitter.parentId);
				if(parentParticleTypeIndex != nullId) {
					particleSubTypes[parentParticleTypeIndex].push_back(particleTypeIndex);
				}
			}
		}
	}

	restart();
}
void ParticleEngine::updateForceSolver() {
	forceSolver.update(effect);
}
void ParticleEngine::updateCollisionSolver() {
	collisionSolver.update(effect);
}

uint32_t ParticleEngine::spawnParticles(uint32_t count, uint32_t pParent, uint32_t particleTypeIndex, uint32_t parentParticleTypeIndex, uint32_t particleEmitterIndex, floatd t, floatd tParent) {
	ParticleContainer& particleContainer = particleContainers[particleTypeIndex];
	count = particleContainer.spawn(count);

	for(uint32_t i = 0; i < count; i++) {
		createParticle(
			particleContainer.getNumParticles() - count + i,
			pParent,
			particleTypeIndex,
			parentParticleTypeIndex,
			particleEmitterIndex,
			t,
			tParent);
	}

	return count;
}

void ParticleEngine::createParticle(uint32_t p, uint32_t pParent, uint32_t particleTypeIndex, uint32_t parentParticleTypeIndex, uint32_t particleEmitterIndex, floatd t, floatd tParent) {
	const ParticleType& particleType = effect->particleTypes.getByIndex(particleTypeIndex);
	const ParticleEmitter& particleEmitter = effect->particleEmitters.getByIndex(particleEmitterIndex);
	ParticleData& particles = particleContainers[particleTypeIndex].get();

	floatd alpha = t / particleEmitter.lifetimeDuration;

	particles.id[p] = particleId++;
	particles.parentId[p] = (pParent != nullId) ? particleContainers[parentParticleTypeIndex].get().id[pParent] : nullId;

	particles.life[p] = 0.0;
	particles.lifespan[p] = std::max(particleType.lifespan.get(alpha) + random::uniform(rng, -particleType.lifespanVariance, +particleType.lifespanVariance), 0.000001);

	vec3d particleEmitterPosition = particleEmitter.position.get(alpha);
	vec3d particleSpawnCenter = (pParent != nullId)
		? (particleContainers[parentParticleTypeIndex].get().globalPosition[pParent] + particleEmitterPosition)
		: (particleType.positionRelative ? vec3d(0.0) : particleEmitterPosition);
	vec3d particleSpawnPosition;

	switch(particleEmitter.shape) {
		case ParticleEmitter::Shape::line:
			particleSpawnPosition = generateOnSegment(rng,
				particleEmitter.size.get(alpha).x,
				particleEmitter.distribution);
			break;
		case ParticleEmitter::Shape::ellipse:
			particleSpawnPosition = generateInEllipse(rng,
				particleEmitter.size.get(alpha),
				particleEmitter.distribution);
			break;
		case ParticleEmitter::Shape::rectangle:
			particleSpawnPosition = generateInRectangle(rng,
				particleEmitter.size.get(alpha),
				particleEmitter.distribution);
			break;
		case ParticleEmitter::Shape::path:
			particleSpawnPosition = generateOnPath(rng,
				particleEmitter.path,
				particleEmitter.distribution);
			break;
		case ParticleEmitter::Shape::ellipsoid:
			particleSpawnPosition = generateInEllipsoid(rng,
				particleEmitter.size.get(alpha),
				particleEmitter.distribution);
			break;
		case ParticleEmitter::Shape::cuboid:
			particleSpawnPosition = generateInCuboid(rng,
				particleEmitter.size.get(alpha),
				particleEmitter.distribution);
			break;
		case ParticleEmitter::Shape::cylinder:
			particleSpawnPosition = generateInCylinder(rng,
				particleEmitter.size.get(alpha),
				particleEmitter.distribution);
			break;
		default:
			particleSpawnPosition = vec3d(0.0);
			break;
	}

	particleSpawnPosition = effect->is3d
		? rotate3d(particleSpawnPosition, particleEmitter.orientation.get(alpha))
		: rotate2d(particleSpawnPosition, particleEmitter.orientation.get(alpha).x);

	particles.position[p] = particleSpawnCenter + particleSpawnPosition;
	particles.globalPosition[p] = particleType.positionRelative
		? particles.position[p] + particleEmitterPosition
		: particles.position[p];

	if(effect->is3d) {
		mat4d directionMatrix = glm::yawPitchRoll(
			glm::radians(particleEmitter.direction.get(alpha).y + particleEmitter.spread.get(alpha) * random::uniform(rng, -0.5, +0.5)),
			glm::radians(particleEmitter.direction.get(alpha).z + particleEmitter.spread.get(alpha) * random::uniform(rng, -0.5, +0.5)),
			glm::radians(particleEmitter.direction.get(alpha).x + particleEmitter.spread.get(alpha) * random::uniform(rng, -0.5, +0.5)));

		switch(particleEmitter.directionMode) {
			case ParticleEmitter::DirectionMode::outwards:
				particles.velocity[p] = vec3d(directionMatrix *
					((particleSpawnPosition != vec3d(0.0)) ? vec4d(glm::normalize(+particleSpawnPosition), 0.0) : worldUpVector4));
				break;
			case ParticleEmitter::DirectionMode::inwards:
				particles.velocity[p] = vec3d(directionMatrix *
					((particleSpawnPosition != vec3d(0.0)) ? vec4d(glm::normalize(-particleSpawnPosition), 0.0) : worldUpVector4));
				break;
			default:
				particles.velocity[p] = vec3d(directionMatrix *
					worldUpVector4);
				break;
		}
	}
	else {
		floatd direction = glm::radians(particleEmitter.direction.get(alpha).x + particleEmitter.spread.get(alpha) * random::uniform(rng,-0.5, +0.5));

		switch(particleEmitter.directionMode) {
			case ParticleEmitter::DirectionMode::outwards:
				particles.velocity[p] = vec3d(glm::rotate(
					(particleSpawnPosition != vec3d(0.0)) ? vec2d(glm::normalize(+particleSpawnPosition)) : worldUpVector2,
					direction),
					0.0);
				break;
			case ParticleEmitter::DirectionMode::inwards:
				particles.velocity[p] = vec3d(glm::rotate(
					(particleSpawnPosition != vec3d(0.0)) ? vec2d(glm::normalize(-particleSpawnPosition)) : worldUpVector2,
					direction),
					0.0);
				break;
			default:
				particles.velocity[p] = vec3d(glm::rotate(
					worldUpVector2,
					direction),
					0.0);
				break;
		}
	}

	particles.velocity[p] *= particleType.initialVelocity.get(alpha) + random::uniform(rng, -particleType.velocityVariance, +particleType.velocityVariance);
	particles.force[p] = vec3d(0.0);

	particles.initialRotation[p] = particleType.initialRotation.get(alpha) + vec3d(
		random::uniform(rng, -particleType.rotationVariance.x, +particleType.rotationVariance.x),
		random::uniform(rng, -particleType.rotationVariance.y, +particleType.rotationVariance.y),
		random::uniform(rng, -particleType.rotationVariance.z, +particleType.rotationVariance.z));
	particles.initialAngularVelocity[p] = vec3d(
		random::uniform(rng, -particleType.angularVelocityVariance.x, +particleType.angularVelocityVariance.x),
		random::uniform(rng, -particleType.angularVelocityVariance.y, +particleType.angularVelocityVariance.y),
		random::uniform(rng, -particleType.angularVelocityVariance.z, +particleType.angularVelocityVariance.z));
	particles.rotation[p] = particles.initialRotation[p];

	particles.initialSize[p] = particleType.initialSize.get(alpha) + random::uniform(rng, -particleType.sizeVariance, +particleType.sizeVariance);
	particles.size[p] = particleType.size.get() * particles.initialSize[p];

	particles.initialColor[p] = vec4d(
		random::uniform(rng, -particleType.colorVariance.x, +particleType.colorVariance.x),
		random::uniform(rng, -particleType.colorVariance.y, +particleType.colorVariance.y),
		random::uniform(rng, -particleType.colorVariance.z, +particleType.colorVariance.z),
		particleType.initialOpacity.get(alpha) + random::uniform(rng, -particleType.opacityVariance, +particleType.opacityVariance));
	particles.color[p] = vec4d(vec3d(particleType.color.get()), particleType.opacity.get());
}
}