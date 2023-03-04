#include "ParticleSimulationCPU.h"
#include "Color.h"
#include <algorithm>

namespace pixelpart {
ParticleSimulationCPU::ParticleSimulationCPU(uint32_t numThreads) {
#ifndef __EMSCRIPTEN__
	threadPool = std::unique_ptr<ThreadPool>(
		new ThreadPool((numThreads > 0)
			? numThreads
			: std::thread::hardware_concurrency()));
#endif
}

void ParticleSimulationCPU::simulate(const ParticleEmitter& particleEmitter, const ParticleType& particleType, ParticleData& particles, uint32_t numParticles, const ForceSolver& forceSolver, const CollisionSolver& collisionSolver, floatd t, floatd dt) {
#ifndef __EMSCRIPTEN__
	numActiveThreads = std::min(std::max(numParticles / numParticlesPerThread, 1U), static_cast<uint32_t>(threadPool->getNumThreads()));

	if(numActiveThreads > 1) {
		uint32_t numParticlesPerThread = numParticles / numActiveThreads;
		uint32_t numThreadsWithLargerLoad = numParticles % numActiveThreads;
		uint32_t workgroupIndex = 0;

		for(uint32_t i = 0; i < numActiveThreads; i++) {
			uint32_t workgroupSize = (i < numActiveThreads - numThreadsWithLargerLoad)
				? numParticlesPerThread
				: numParticlesPerThread + 1;

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

			threadPool->enqueue(i, &simulateParticles, particleEmitter, particleType, workgroupParticles, workgroupSize, forceSolver, collisionSolver, t, dt);

			workgroupIndex += workgroupSize;
		}

		for(uint32_t i = 0; i < numActiveThreads; i++) {
			threadPool->wait(i);
		}
	}
#else
	numActiveThreads = 1;
#endif

	if(numActiveThreads == 1) {
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

		simulateParticles(particleEmitter, particleType, workgroupParticles, numParticles, forceSolver, collisionSolver, t, dt);
	}
}

uint32_t ParticleSimulationCPU::getNumActiveThreads() const {
	return numActiveThreads;
}

void ParticleSimulationCPU::setNumParticlesPerThread(uint32_t num) {
	numParticlesPerThread = std::max(num, 1U);
}

void ParticleSimulationCPU::simulateParticles(const ParticleEmitter& particleEmitter, const ParticleType& particleType, ParticleDataPointer particles, uint32_t workgroupSize, const ForceSolver& forceSolver, const CollisionSolver& collisionSolver, floatd t, floatd dt) {
	for(uint32_t p = 0; p < workgroupSize; p++) {
		particles.size[p] = particleType.size.get(particles.life[p]) * particles.initialSize[p];
		particles.color[p] = hsvAdd(
			particleType.color.get(particles.life[p]),
			particles.initialColor[p],
			particleType.opacity.get(particles.life[p]));
	}

	for(uint32_t p = 0; p < workgroupSize; p++) {
		floatd particleEmitterLife = std::fmod(t - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration) / particleEmitter.lifetimeDuration;
		vec3d prticleEmitterPosition = particleEmitter.position.get(particleEmitterLife);
		vec3d forwardDirection = (particles.velocity[p] != vec3d(0.0))
			? glm::normalize(particles.velocity[p])
			: vec3d(0.0);
		vec3d radialDirection = (prticleEmitterPosition != particles.globalPosition[p])
			? glm::normalize(prticleEmitterPosition - particles.globalPosition[p])
			: vec3d(0.0);

		particles.force[p] = forwardDirection * particleType.acceleration.get(particles.life[p]);
		particles.force[p] += radialDirection * particleType.radialAcceleration.get(particles.life[p]);

		forceSolver.solve(particleType,
			particles, p,
			t, dt);

		collisionSolver.solve(particleType,
			particles, p,
			t, dt);

		MotionPathSolver::solve(particleType,
			particles, p,
			t, dt);
	}

	switch(particleType.rotationMode) {
		case RotationMode::angle: {
			for(uint32_t p = 0; p < workgroupSize; p++) {
				particles.rotation[p] = particles.initialRotation[p] + particleType.rotation.get(particles.life[p]);
			}

			break;
		}
		case RotationMode::velocity: {
			for(uint32_t p = 0; p < workgroupSize; p++) {
				particles.rotation[p] += (particles.initialAngularVelocity[p] + particleType.rotation.get(particles.life[p])) * dt;
			}

			break;
		}
		default: {
			break;
		}
	}

	for(uint32_t p = 0; p < workgroupSize; p++) {
		floatd particleEmitterLife = std::fmod(t - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration) / particleEmitter.lifetimeDuration;

		particles.velocity[p] += particles.force[p] * dt;
		particles.velocity[p] *= std::pow(particleType.damping.get(particles.life[p]), dt);
		particles.position[p] += particles.velocity[p] * dt;

		particles.globalPosition[p] = particleType.positionRelative
			? particles.position[p] + particleEmitter.position.get(particleEmitterLife)
			: particles.position[p];
	}
}
}