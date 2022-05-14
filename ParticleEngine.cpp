#include "ParticleEngine.h"
#include "ParticleSimulationCPU.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace pixelpart {
ParticleEngine::ParticleEngine() : ParticleEngine(nullptr, 1000) {

}
ParticleEngine::ParticleEngine(const Effect* effectPtr, uint32_t capacity, uint32_t numThreadsMax) : effect(effectPtr), particleCapacity(capacity) {
	particleSimulation = std::unique_ptr<ParticleSimulationCPU>(new ParticleSimulationCPU(numThreadsMax));

	resetSeed();
	refreshEffect();
}

void ParticleEngine::step(floatd dt) {
	time += dt;

	if(!effect) {
		return;
	}

	for(uint32_t emitterIndex = 0; emitterIndex < effect->getNumParticleEmitters(); emitterIndex++) {
		const ParticleEmitter& emitter = effect->getParticleEmitterByIndex(emitterIndex);
		if(emitter.parentId != NullId) {
			continue;
		}

		const floatd localTime = time - emitter.lifetimeStart;
		const floatd localTimeRep = std::fmod(localTime, emitter.lifetimeDuration);

		if(localTime > 0.0) {
			if(emitter.instantiationMode != ParticleEmitter::InstantiationMode::continuous && (emitter.repeat ? localTimeRep : localTime) <= dt) {
				numParticlesToEmit[emitterIndex] += emitter.numParticles.get();
			}
			else if(emitter.instantiationMode == ParticleEmitter::InstantiationMode::continuous && (emitter.repeat || localTime < emitter.lifetimeDuration)) {
				numParticlesToEmit[emitterIndex] += emitter.numParticles.get(localTimeRep / emitter.lifetimeDuration) * dt;
			}
		}

		int32_t particlesEmitted = static_cast<int32_t>(numParticlesToEmit[emitterIndex]);	
		if(particlesEmitted > 0) {
			particlesEmitted = emitParticles(emitterIndex,
				particlesEmitted,
				localTimeRep);

			numParticlesToEmit[emitterIndex] -= static_cast<floatd>(particlesEmitted);
		}
	}

	for(uint32_t emitterIndex = 0; emitterIndex < effect->getNumParticleEmitters(); emitterIndex++) {
		const ParticleEmitter& emitter = effect->getParticleEmitterByIndex(emitterIndex);
		const std::vector<uint32_t>& subEmitters = subEmitterIndices[emitterIndex];
		ParticleData& emitterParticles = particles[emitterIndex];

		for(uint32_t p = 0; p < numParticles[emitterIndex]; p++) {
			emitterParticles.life[p] += dt / emitterParticles.lifespan[p];
		}

		if(!subEmitters.empty()) {
			const floatd localTime = time - emitter.lifetimeStart;
			const floatd localTimeRep = std::fmod(localTime, emitter.lifetimeDuration);

			for(uint32_t p = 0; p < numParticles[emitterIndex]; p++) {
				for(uint32_t subEmitterIndex : subEmitters) {
					const ParticleEmitter& subEmitter = effect->getParticleEmitterByIndex(subEmitterIndex);
					const floatd localTimeSubEmitter = emitterParticles.life[p] * emitterParticles.lifespan[p] - subEmitter.lifetimeStart;
					const floatd localTimeRepSubEmitter = std::fmod(localTimeSubEmitter, subEmitter.lifetimeDuration);

					if(localTimeSubEmitter > 0.0) {
						if(emitterParticles.life[p] > 1.0) {
							if(subEmitter.instantiationMode == ParticleEmitter::InstantiationMode::burst_death) {
								numParticlesToEmit[subEmitterIndex] += subEmitter.numParticles.get();
							}
						}
						else {
							if(subEmitter.instantiationMode == ParticleEmitter::InstantiationMode::burst && (subEmitter.repeat ? localTimeRepSubEmitter : localTimeSubEmitter) <= dt) {
								numParticlesToEmit[subEmitterIndex] += subEmitter.numParticles.get();
							}
							else if(subEmitter.instantiationMode == ParticleEmitter::InstantiationMode::continuous && (subEmitter.repeat || localTimeSubEmitter < subEmitter.lifetimeDuration)) {
								numParticlesToEmit[subEmitterIndex] += subEmitter.numParticles.get(localTimeRepSubEmitter / subEmitter.lifetimeDuration) * dt;
							}
						}
					}
					
					int32_t particlesEmitted = static_cast<int32_t>(numParticlesToEmit[subEmitterIndex]);
					if(particlesEmitted > 0) {
						particlesEmitted = emitParticles(subEmitterIndex,
							particlesEmitted,
							localTimeRepSubEmitter,
							localTimeRep,
							emitterIndex,
							p);

						numParticlesToEmit[subEmitterIndex] -= static_cast<floatd>(particlesEmitted);
					}
				}
			}
		}

		for(int32_t p = 0; p < static_cast<int32_t>(numParticles[emitterIndex]); p++) {
			if(emitterParticles.life[p] > 1.0) {
				destroyParticle(emitterIndex, p);
			}
		}

		memcpy(particlePositionSnapshot[emitterIndex].data(), emitterParticles.globalPosition.data(), numParticles[emitterIndex] * sizeof(vec2d));
	}

	numActiveThreads = 1;

	for(uint32_t emitterIndex = 0; emitterIndex < effect->getNumParticleEmitters(); emitterIndex++) {
		particleSimulation->simulate(
			effect->getParticleEmitterByIndex(emitterIndex),
			particles[emitterIndex],
			numParticles[emitterIndex],
			forceSolver,
			collisionSolver,
			time,
			dt);

		numActiveThreads = std::max(numActiveThreads, particleSimulation->getNumActiveThreads());
	}
}

void ParticleEngine::reset() {
	time = 0.0;
}
void ParticleEngine::restart() {
	time = 0.0;
	particleId = 0;

	for(uint32_t& n : numParticles) {
		n = 0;
	}
	for(floatd& n : numParticlesToEmit) {
		n = 0.0;
	}
}
floatd ParticleEngine::getTime() const {
	return time;
}

void ParticleEngine::spawnParticles(uint32_t emitterId, uint32_t count) {
	if(!effect || !effect->hasParticleEmitter(emitterId)) {
		return;
	}

	const ParticleEmitter& emitter = effect->getParticleEmitter(emitterId);
	const floatd localTimeRep = std::fmod(time - emitter.lifetimeStart, emitter.lifetimeDuration);

	emitParticles(
		effect->findParticleEmitterById(emitterId),
		count,
		localTimeRep);
}

void ParticleEngine::setSeed(uint32_t sd) {
	seed = sd;
	resetSeed();
}
void ParticleEngine::resetSeed() {
	rng = std::mt19937(seed);
}

void ParticleEngine::setEffect(const Effect* effectPtr) {
	effect = effectPtr;
	refreshEffect();
}
void ParticleEngine::setParticleCapacity(uint32_t capacity) {
	particleCapacity = capacity;

	for(uint32_t emitterIndex = 0; emitterIndex < particles.size(); emitterIndex++) {
		ParticleData& emitterParticles = particles[emitterIndex];

		emitterParticles.spawnId.resize(capacity);
		emitterParticles.parentId.resize(capacity);
		emitterParticles.numParticlesToEmit.resize(capacity);
		emitterParticles.life.resize(capacity);
		emitterParticles.lifespan.resize(capacity);
		emitterParticles.position.resize(capacity);
		emitterParticles.globalPosition.resize(capacity);
		emitterParticles.velocity.resize(capacity);
		emitterParticles.force.resize(capacity);
		emitterParticles.rotation.resize(capacity);
		emitterParticles.initialRotation.resize(capacity);
		emitterParticles.initialAngularVelocity.resize(capacity);
		emitterParticles.size.resize(capacity);
		emitterParticles.initialSize.resize(capacity);
		emitterParticles.color.resize(capacity);
		emitterParticles.initialColor.resize(capacity);
		emitterParticles.frame.resize(capacity);
		emitterParticles.initialFrame.resize(capacity);
		particlePositionSnapshot[emitterIndex].resize(capacity);
	}
}
const Effect* ParticleEngine::getEffect() const {
	return effect;
}
uint32_t ParticleEngine::getParticleCapacity() const {
	return particleCapacity;
}
uint32_t ParticleEngine::getNumParticles() const {
	uint32_t num = 0;
	for(uint32_t n : numParticles) {
		num += n;
	}

	return num;
}
uint32_t ParticleEngine::getNumParticles(uint32_t emitterIndex) const {
	return numParticles[emitterIndex];
}
const ParticleData& ParticleEngine::getParticles(uint32_t emitterIndex) const {
	return particles[emitterIndex];
}
const std::vector<vec2d>& ParticleEngine::getParticlePositionSnapshot(uint32_t emitterIndex) const {
	return particlePositionSnapshot[emitterIndex];
}

uint32_t ParticleEngine::getNumActiveThreads() const {
	return numActiveThreads;
}

void ParticleEngine::refreshEffect() {
	refreshParticleSolver();
	refreshForceSolver();
	refreshCollisionSolver();
}
void ParticleEngine::refreshParticleSolver() {
	particles.clear();
	numParticles.clear();
	numParticlesToEmit.clear();
	subEmitterIndices.clear();
	particlePositionSnapshot.clear();

	if(effect) {
		particles.resize(effect->getNumParticleEmitters());
		numParticles.resize(effect->getNumParticleEmitters());
		numParticlesToEmit.resize(effect->getNumParticleEmitters());
		subEmitterIndices.resize(effect->getNumParticleEmitters());
		particlePositionSnapshot.resize(effect->getNumParticleEmitters());

		for(uint32_t emitterIndex = 0; emitterIndex < effect->getNumParticleEmitters(); emitterIndex++) {
			const uint32_t parentEmitterIndex = effect->findParticleEmitterById(effect->getParticleEmitterByIndex(emitterIndex).parentId);
			if(parentEmitterIndex != NullId) {
				subEmitterIndices[parentEmitterIndex].push_back(emitterIndex);
			}
		}

		setParticleCapacity(particleCapacity);
	}

	restart();
}
void ParticleEngine::refreshForceSolver() {
	forceSolver.update(effect ? effect->getForceFields() : std::vector<ForceField>());
}
void ParticleEngine::refreshCollisionSolver() {
	collisionSolver.update(effect ? effect->getColliders() : std::vector<Collider>());
}

int32_t ParticleEngine::sampleUniformInt(int32_t min, int32_t max) {
	uniformIntDistribution.param(std::uniform_int_distribution<int32_t>::param_type(min, max));
	return uniformIntDistribution(rng);
}
floatd ParticleEngine::sampleUniform(floatd min, floatd max) {
	return uniformDistribution(rng) * (max - min) + min;
}
floatd ParticleEngine::sampleNormal(floatd min, floatd max) {
	floatd x = normalDistribution(rng);
	while(x < -1.0 || x > +1.0) {
		x = normalDistribution(rng);
	}

	return (x * 0.5 + 0.5) * (max - min) + min;
}
floatd ParticleEngine::sampleNormalReverse(floatd min, floatd max) {
	floatd x = normalDistribution(rng);
	while(x < -1.0 || x > +1.0) {
		x = normalDistribution(rng);
	}

	x += (x == 0.0) ? std::numeric_limits<floatd>::epsilon() : 0.0;
	x = (1.0 - std::abs(x)) * glm::sign(x);

	return (x * 0.5 + 0.5) * (max - min) + min;
}
floatd ParticleEngine::sample(ParticleEmitter::Distribution distribution, floatd min, floatd max) {
	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary:
			return sampleUniform(min, max);
		case ParticleEmitter::Distribution::center:
			return sampleNormal(min, max);
		case ParticleEmitter::Distribution::center_reverse:
			return sampleNormalReverse(min, max);
		default:
			return 0.0;
	}
}

vec2d ParticleEngine::generatePointOnSegment(const vec2d& position, floatd length, floatd angle, ParticleEmitter::Distribution distribution) {
	const vec2d direction = glm::rotate(vec2d(0.0, 1.0), glm::radians(angle));
	const floatd x = sample(distribution, -length, +length);

	return position + direction * x * 0.5;
}
vec2d ParticleEngine::generatePointInEllipse(const vec2d& position, const vec2d& size, floatd angle, ParticleEmitter::Distribution distribution) {
	floatd phi = 0.0;
	floatd rho = 0.0;
	vec2d p = vec2d(0.0);
	
	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			phi = sampleUniform(0.0, 2.0 * M_PI);
			rho = sampleUniform(0.0, 1.0);
			p = vec2d(std::cos(phi), std::sin(phi)) * std::sqrt(rho);

			break;
		}
		case ParticleEmitter::Distribution::center: {
			do {
				phi = sampleUniform(0.0, 2.0 * M_PI);
				rho = sampleUniform(0.0, 1.0);
				p = vec2d(std::cos(phi), std::sin(phi)) * std::sqrt(rho);
			} while(std::pow(sampleUniform(0.0, 1.0), 2) < p.x * p.x + p.y * p.y);

			break;
		}
		case ParticleEmitter::Distribution::center_reverse: {
			do {
				phi = sampleUniform(0.0, 2.0 * M_PI);
				rho = sampleUniform(0.0, 1.0);
				p = vec2d(std::cos(phi), std::sin(phi)) * std::sqrt(rho);
			} while(std::pow(sampleUniform(0.0, 1.0), 2) > p.x * p.x + p.y * p.y);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			phi = sampleUniform(0.0, 2.0 * M_PI);
			rho = sampleUniform(0.0, 1.0);
			p = vec2d(std::cos(phi), std::sin(phi));

			break;
		}
		default: {
			break;
		}
	}

	p *= size * 0.5;

	return position + vec2d(
		std::cos(glm::radians(angle)) * p.x + std::sin(glm::radians(angle)) * p.y,
		std::sin(glm::radians(angle)) * p.x - std::cos(glm::radians(angle)) * p.y);
}
vec2d ParticleEngine::generatePointInRectangle(const vec2d& position, const vec2d& size, floatd angle, ParticleEmitter::Distribution distribution) {
	const vec2d up = glm::rotate(vec2d(0.0, 1.0), glm::radians(angle));
	const vec2d right = glm::rotate(up, -M_PI * 0.5);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			floatd x = sampleUniform(-size.x, +size.x);
			floatd y = sampleUniform(-size.y, +size.y);

			return position + (up * y + right * x) * 0.5;
		}
		case ParticleEmitter::Distribution::center: {
			floatd x = sampleNormal(-size.x, +size.x);
			floatd y = sampleNormal(-size.y, +size.y);

			return position + (up * y + right * x) * 0.5;
		}
		case ParticleEmitter::Distribution::center_reverse: {
			floatd x;
			floatd y;
			do {
				x = sampleUniform(-1.0, +1.0);
				y = sampleUniform(-1.0, +1.0);
			} while(sampleUniform(0.0, 1.0) >= (x * x + y * y) * 0.5);

			return position + (up * size.y * x + right * size.x * y) * 0.5;
		}
		case ParticleEmitter::Distribution::boundary: {
			vec2d corner = -up * size.y * 0.5 - right * size.x * 0.5;
			floatd r = sampleUniform(0.0, (size.x + size.y) * 2.0);

			if(r < size.y) {
				return position + corner + up * r;
			}
			else if(r < size.y + size.x) {
				return position + corner + up * size.y + right * (r - size.y);
			}
			else if(r < (size.y + size.x) + size.y) {
				return position + corner + right * size.x + up * (r - (size.y + size.x));
			}
			else {
				return position + corner + right * (r - (size.y + size.x + size.y));
			}
		}
		default: {
			return position;
		}
	}
}

uint32_t ParticleEngine::emitParticles(uint32_t emitterIndex, uint32_t count, floatd t, floatd tParent, uint32_t parentEmitterIndex, uint32_t parentParticle) {
	const uint32_t numParticlesToSpawn = (numParticles[emitterIndex] + count > getParticleCapacity())
		? getParticleCapacity() - numParticles[emitterIndex]
		: count;

	for(uint32_t i = 0; i < numParticlesToSpawn; i++) {
		createParticle(emitterIndex, numParticles[emitterIndex] + i, t, tParent, parentEmitterIndex, parentParticle);
	}

	numParticles[emitterIndex] += numParticlesToSpawn;

	return numParticlesToSpawn;
}
void ParticleEngine::createParticle(uint32_t emitterIndex, uint32_t p, floatd t, floatd tParent, uint32_t parentEmitterIndex, uint32_t parentParticle) {
	const ParticleEmitter& emitter = effect->getParticleEmitterByIndex(emitterIndex);
	ParticleData& emitterParticles = particles[emitterIndex];

	const floatd alpha = t / emitter.lifetimeDuration;
	const vec2d emitterPosition = emitter.motionPath.get(alpha);
	const vec2d spawnPosition = (parentParticle != NullId)
		? (particles[parentEmitterIndex].globalPosition[parentParticle] + emitterPosition)
		: (emitter.particlePositionRelative ? vec2d(0.0) : emitterPosition);
	const floatd initialDirection = emitter.direction.get(alpha) + emitter.spread.get(alpha) * sampleUniform(-0.5, +0.5);
	const floatd initialSpeed = emitter.particleInitialVelocity.get(alpha) + sampleUniform(-emitter.particleInitialVelocityVariance, +emitter.particleInitialVelocityVariance);

	emitterParticles.spawnId[p] = particleId++;
	emitterParticles.parentId[p] = (parentParticle != NullId) ? particles[parentEmitterIndex].spawnId[parentParticle] : NullId;
	emitterParticles.numParticlesToEmit[p] = 0.0;
	emitterParticles.life[p] = 0.0;
	emitterParticles.lifespan[p] = std::max(emitter.particleLifespan.get(alpha) + sampleUniform(-emitter.particleLifespanVariance, +emitter.particleLifespanVariance), 0.0);

	switch(emitter.shape) {
		case ParticleEmitter::Shape::line:
			emitterParticles.position[p] = generatePointOnSegment(spawnPosition, emitter.width.get(alpha), emitter.orientation.get(alpha), emitter.distribution);
			break;
		case ParticleEmitter::Shape::ellipse:
			emitterParticles.position[p] = generatePointInEllipse(spawnPosition, vec2d(emitter.width.get(alpha), emitter.height.get(alpha)), emitter.orientation.get(alpha), emitter.distribution);
			break;
		case ParticleEmitter::Shape::rectangle:
			emitterParticles.position[p] = generatePointInRectangle(spawnPosition, vec2d(emitter.width.get(alpha), emitter.height.get(alpha)), emitter.orientation.get(alpha), emitter.distribution);
			break;
		default:
			emitterParticles.position[p] = spawnPosition;
			break;
	}

	emitterParticles.globalPosition[p] = emitter.particlePositionRelative
		? emitterParticles.position[p] + emitterPosition
		: emitterParticles.position[p];

	switch(emitter.spawnMode) {
		case ParticleEmitter::SpawnMode::out:
			emitterParticles.velocity[p] = glm::rotate((emitterParticles.position[p] != spawnPosition) ? glm::normalize(emitterParticles.position[p] - spawnPosition) : vec2d(0.0, 1.0), glm::radians(initialDirection)) * initialSpeed;
			break;
		case ParticleEmitter::SpawnMode::in:
			emitterParticles.velocity[p] = glm::rotate((emitterParticles.position[p] != spawnPosition) ? glm::normalize(spawnPosition - emitterParticles.position[p]) : vec2d(0.0, 1.0), glm::radians(initialDirection)) * initialSpeed;
			break;
		default:
			emitterParticles.velocity[p] = glm::rotate(vec2d(0.0, 1.0), glm::radians(initialDirection)) * initialSpeed;
			break;
	}

	emitterParticles.force[p] = vec2d(0.0);
	emitterParticles.initialRotation[p] = emitter.particleInitialRotation.get(alpha) + sampleUniform(-emitter.particleRotationVariance, +emitter.particleRotationVariance);
	emitterParticles.initialAngularVelocity[p] = sampleUniform(-emitter.particleAngularVelocityVariance, +emitter.particleAngularVelocityVariance);
	emitterParticles.rotation[p] = emitterParticles.initialRotation[p];
	emitterParticles.initialSize[p] = emitter.particleInitialSize.get(alpha) + sampleUniform(-emitter.particleSizeVariance, +emitter.particleSizeVariance);
	emitterParticles.size[p] = vec2d(emitter.particleSize.get() * emitterParticles.initialSize[p]) * vec2d(emitter.particleWidth.get(), emitter.particleHeight.get());
	emitterParticles.initialColor[p] = vec4d(
		sampleUniform(-emitter.particleColorVariance.x, +emitter.particleColorVariance.x),
		sampleUniform(-emitter.particleColorVariance.y, +emitter.particleColorVariance.y),
		sampleUniform(-emitter.particleColorVariance.z, +emitter.particleColorVariance.z),
		emitter.particleInitialOpacity.get(alpha) + sampleUniform(-emitter.particleOpacityVariance, +emitter.particleOpacityVariance));
	emitterParticles.color[p] = vec4d(vec3d(emitter.particleColor.get()), emitter.particleOpacity.get());
	emitterParticles.initialFrame[p] = emitter.particleSprite.randomFrame 
		? sampleUniformInt(0, emitter.particleSprite.framesRow * emitter.particleSprite.framesColumn - 1)
		: emitter.particleSprite.frame;
	emitterParticles.frame[p] = emitterParticles.initialFrame[p];
}
void ParticleEngine::destroyParticle(uint32_t emitterIndex, int32_t& p) {
	ParticleData& emitterParticles = particles[emitterIndex];
	uint32_t& n = numParticles[emitterIndex];
	n--;

	std::swap(emitterParticles.spawnId[p], emitterParticles.spawnId[n]);
	std::swap(emitterParticles.parentId[p], emitterParticles.parentId[n]);
	std::swap(emitterParticles.numParticlesToEmit[p], emitterParticles.numParticlesToEmit[n]);
	std::swap(emitterParticles.life[p], emitterParticles.life[n]);
	std::swap(emitterParticles.lifespan[p], emitterParticles.lifespan[n]);
	std::swap(emitterParticles.position[p], emitterParticles.position[n]);
	std::swap(emitterParticles.globalPosition[p], emitterParticles.globalPosition[n]);
	std::swap(emitterParticles.velocity[p], emitterParticles.velocity[n]);
	std::swap(emitterParticles.force[p], emitterParticles.force[n]);
	std::swap(emitterParticles.rotation[p], emitterParticles.rotation[n]);
	std::swap(emitterParticles.initialRotation[p], emitterParticles.initialRotation[n]);
	std::swap(emitterParticles.initialAngularVelocity[p], emitterParticles.initialAngularVelocity[n]);
	std::swap(emitterParticles.size[p], emitterParticles.size[n]);
	std::swap(emitterParticles.initialSize[p], emitterParticles.initialSize[n]);
	std::swap(emitterParticles.color[p], emitterParticles.color[n]);
	std::swap(emitterParticles.initialColor[p], emitterParticles.initialColor[n]);
	std::swap(emitterParticles.frame[p], emitterParticles.frame[n]);
	std::swap(emitterParticles.initialFrame[p], emitterParticles.initialFrame[n]);
	p--;
}
}