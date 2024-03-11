#include "ParticleSolverCPU.h"
#include "../common/Color.h"

namespace pixelpart {
ParticleSolverCPU::ParticleSolverCPU() {

}
#ifndef __EMSCRIPTEN__
ParticleSolverCPU::ParticleSolverCPU(std::shared_ptr<ThreadPool> pool) : threadPool(pool) {

}
#endif

void ParticleSolverCPU::solve(const ParticleEmitter& particleEmitter, const ParticleType& particleType, ParticleData& particles, uint32_t numParticles, float_t t, float_t dt) {
#ifndef __EMSCRIPTEN__
	uint32_t numAvailableThreads = threadPool != nullptr ? static_cast<uint32_t>(threadPool->getNumThreads()) : 1u;
	numActiveThreads = std::min(std::max(numParticles / numParticlesPerThread, 1u), numAvailableThreads);

	if(numActiveThreads > 1u) {
		uint32_t numParticlesPerThread = numParticles / numActiveThreads;
		uint32_t numThreadsWithLargerLoad = numParticles % numActiveThreads;
		uint32_t workgroupIndex = 0u;

		for(uint32_t i = 0u; i < numActiveThreads; i++) {
			uint32_t workgroupSize = (i < numActiveThreads - numThreadsWithLargerLoad)
				? numParticlesPerThread
				: numParticlesPerThread + 1u;

			ParticleDataPointer workgroupParticles{
				particles.id.data() + workgroupIndex,
				particles.parentId.data() + workgroupIndex,
				particles.life.data() + workgroupIndex,
				particles.lifespan.data() + workgroupIndex,
				particles.position.data() + workgroupIndex,
				particles.globalPosition.data() + workgroupIndex,
				particles.velocity.data() + workgroupIndex,
				particles.force.data() + workgroupIndex,
				particles.rotation.data() + workgroupIndex,
				particles.initialRotation.data() + workgroupIndex,
				particles.initialAngularVelocity.data() + workgroupIndex,
				particles.size.data() + workgroupIndex,
				particles.initialSize.data() + workgroupIndex,
				particles.color.data() + workgroupIndex,
				particles.initialColor.data() + workgroupIndex
			};

			threadPool->enqueue(i, &simulateParticles,
				particleEmitter, particleType,
				workgroupParticles, workgroupSize,
				t, dt,
				forceSolver,
				collisionSolver,
				motionPathSolver);

			workgroupIndex += workgroupSize;
		}

		for(uint32_t i = 0u; i < numActiveThreads; i++) {
			threadPool->wait(i);
		}
	}

#else
	numActiveThreads = 1u;

#endif

	if(numActiveThreads <= 1u) {
		ParticleDataPointer workgroupParticles{
			particles.id.data(),
			particles.parentId.data(),
			particles.life.data(),
			particles.lifespan.data(),
			particles.position.data(),
			particles.globalPosition.data(),
			particles.velocity.data(),
			particles.force.data(),
			particles.rotation.data(),
			particles.initialRotation.data(),
			particles.initialAngularVelocity.data(),
			particles.size.data(),
			particles.initialSize.data(),
			particles.color.data(),
			particles.initialColor.data()
		};

		simulateParticles(
			particleEmitter, particleType,
			workgroupParticles, numParticles,
			t, dt,
			forceSolver,
			collisionSolver,
			motionPathSolver);
	}
}
void ParticleSolverCPU::refresh(const Effect* effect, uint32_t flags) {
	if(flags & refresh_force) {
		forceSolver.update(effect);
	}
	if(flags & refresh_collision) {
		collisionSolver.update(effect);
	}
}

uint32_t ParticleSolverCPU::getNumActiveThreads() const {
	return numActiveThreads;
}

void ParticleSolverCPU::setNumParticlesPerThread(uint32_t num) {
	numParticlesPerThread = std::max(num, 1u);
}

void ParticleSolverCPU::simulateParticles(const ParticleEmitter& particleEmitter, const ParticleType& particleType,
	ParticleDataPointer particles, uint32_t workgroupSize,
	float_t t, float_t dt,
	const ForceSolver& forceSolver,
	const CollisionSolver& collisionSolver,
	const MotionPathSolver& motionPathSolver) {
	float_t particleEmitterLife = std::fmod(t - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration) / particleEmitter.lifetimeDuration;
	vec3_t particleEmitterPosition = particleEmitter.position.get(particleEmitterLife);

	for(uint32_t p = 0u; p < workgroupSize; p++) {
		particles.size[p] = particleType.size.get(particles.life[p]) * particles.initialSize[p];
		particles.size[p] += particleType.stretch.get(particles.life[p]) * particles.size[p] * glm::length(particles.velocity[p]);
	}

	for(uint32_t p = 0u; p < workgroupSize; p++) {
		particles.color[p] = hsvAdd(
			particleType.color.get(particles.life[p]),
			particles.initialColor[p],
			particleType.opacity.get(particles.life[p]));
	}

	for(uint32_t p = 0u; p < workgroupSize; p++) {
		vec3_t forwardDirection = (particles.velocity[p] != vec3_t(0.0))
			? glm::normalize(particles.velocity[p])
			: vec3_t(0.0);
		vec3_t radialDirection = (particleEmitterPosition != particles.globalPosition[p])
			? glm::normalize(particleEmitterPosition - particles.globalPosition[p])
			: vec3_t(0.0);

		particles.force[p] = forwardDirection * particleType.acceleration.get(particles.life[p]);
		particles.force[p] += radialDirection * particleType.radialAcceleration.get(particles.life[p]);
	}

	forceSolver.solve(particleType, particles, workgroupSize, t, dt);
	collisionSolver.solve(particleType, particles, workgroupSize, t, dt);
	motionPathSolver.solve(particleType, particles, workgroupSize, t, dt);

	switch(particleType.rotationMode) {
		case RotationMode::angle: {
			for(uint32_t p = 0u; p < workgroupSize; p++) {
				particles.rotation[p] = particles.initialRotation[p] + particleType.rotation.get(particles.life[p]);
				particles.rotation[p] += particleType.rotationBySpeed.get(particles.life[p]) * glm::length(particles.velocity[p]);
			}

			break;
		}
		case RotationMode::velocity: {
			for(uint32_t p = 0u; p < workgroupSize; p++) {
				particles.rotation[p] += (particles.initialAngularVelocity[p] + particleType.rotation.get(particles.life[p])) * dt;
				particles.rotation[p] += particleType.rotationBySpeed.get(particles.life[p]) * glm::length(particles.velocity[p]) * dt;
			}

			break;
		}
		default: {
			break;
		}
	}

	for(uint32_t p = 0u; p < workgroupSize; p++) {
		particles.velocity[p] += particles.force[p] * dt;
		particles.position[p] += particles.velocity[p] * dt;

		particles.globalPosition[p] = particleType.positionRelative
			? particles.position[p] + particleEmitterPosition
			: particles.position[p];
	}
}
}