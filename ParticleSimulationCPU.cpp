#include "ParticleSimulationCPU.h"
#include "Color.h"
#include <algorithm>

namespace pixelpart {
ParticleSimulationCPU::ParticleSimulationCPU(uint32_t numThreads) {
#ifndef __EMSCRIPTEN__
	threadPool = std::unique_ptr<ThreadPool>(new ThreadPool((numThreads > 0) ? numThreads : std::thread::hardware_concurrency()));
#endif
}

void ParticleSimulationCPU::simulate(const ParticleEmitter& emitter, ParticleData& particles, uint32_t numParticles, const ForceSolver& forceSolver, const CollisionSolver& collisionSolver, floatd t, floatd dt) {
#ifndef __EMSCRIPTEN__
	numActiveThreads = std::min(std::max(numParticles / numParticlesPerThread, 1U), static_cast<uint32_t>(threadPool->getNumThreads()));

	if(numActiveThreads > 1) {
		const uint32_t numParticlesPerThread = numParticles / numActiveThreads;
		const uint32_t numThreadsWithLargerLoad = numParticles % numActiveThreads;
		uint32_t workgroupIndex = 0;

		for(uint32_t i = 0; i < numActiveThreads; i++) {
			uint32_t workgroupSize = (i < numActiveThreads - numThreadsWithLargerLoad)
				? numParticlesPerThread
				: numParticlesPerThread + 1;

			const ParticleDataPointer workgroupParticles{
				particles.spawnId.data() + workgroupIndex,
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

			threadPool->enqueue(i, &simulateParticles, emitter, workgroupParticles, workgroupSize, forceSolver, collisionSolver, t, dt);

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
		const ParticleDataPointer workgroupParticles{
			particles.spawnId.data(),
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

		simulateParticles(emitter, workgroupParticles, numParticles, forceSolver, collisionSolver, t, dt);
	}
}

uint32_t ParticleSimulationCPU::getNumActiveThreads() const {
	return numActiveThreads;
}

void ParticleSimulationCPU::setNumParticlesPerThread(uint32_t num) {
	numParticlesPerThread = std::max(num, 1U);
}

void ParticleSimulationCPU::simulateParticles(const ParticleEmitter& emitter, ParticleDataPointer particles, uint32_t workgroupSize, const ForceSolver& forceSolver, const CollisionSolver& collisionSolver, floatd t, floatd dt) {
	for(uint32_t p = 0; p < workgroupSize; p++) {
		particles.size[p] = vec2d(emitter.particleSize.get(particles.life[p]) * particles.initialSize[p]) * vec2d(
			emitter.particleWidth.get(particles.life[p]),
			emitter.particleHeight.get(particles.life[p]));
		particles.color[p] = hsvAdd(
			emitter.particleColor.get(particles.life[p]),
			particles.initialColor[p],
			emitter.particleOpacity.get(particles.life[p]));
	}

	for(uint32_t p = 0; p < workgroupSize; p++) {
		const floatd alpha = std::fmod(t - emitter.lifetimeStart, emitter.lifetimeDuration) / emitter.lifetimeDuration;
		const vec2d emitterPosition = emitter.motionPath.get(alpha);
		const vec2d forwardDirection = (particles.velocity[p] != vec2d(0.0, 0.0)) ? glm::normalize(particles.velocity[p]) : vec2d(0.0, 0.0);
		const vec2d radialDirection = (emitterPosition != particles.globalPosition[p]) ? glm::normalize(emitterPosition - particles.globalPosition[p]) : vec2d(0.0, 0.0);

		particles.force[p] = forwardDirection * emitter.particleAcceleration.get(particles.life[p]);
		particles.force[p] += radialDirection * emitter.particleRadialAcceleration.get(particles.life[p]);

		forceSolver.solve(emitter,
			particles, p,
			emitter.particleWeight.get(particles.life[p]),
			t, dt);

		collisionSolver.solve(emitter,
			particles, p,
			emitter.particleBounce.get(particles.life[p]),
			emitter.particleFriction.get(particles.life[p]),
			t, dt);
	}

	switch(emitter.particleRotationMode) {
		case ParticleEmitter::RotationMode::angle: {
			for(uint32_t p = 0; p < workgroupSize; p++) {
				particles.rotation[p] = particles.initialRotation[p] + emitter.particleRotation.get(particles.life[p]);
			}

			break;
		}
		case ParticleEmitter::RotationMode::velocity: {
			for(uint32_t p = 0; p < workgroupSize; p++) {
				particles.rotation[p] += (particles.initialAngularVelocity[p] + emitter.particleRotation.get(particles.life[p])) * dt;
			}

			break;
		}
		case ParticleEmitter::RotationMode::align: {
			for(uint32_t p = 0; p < workgroupSize; p++) {
				particles.rotation[p] = particles.initialRotation[p] + glm::degrees(
					glm::orientedAngle(vec2d(0.0, 1.0), (particles.velocity[p] != vec2d(0.0, 0.0))
						? glm::normalize(particles.velocity[p])
						: vec2d(0.0, 0.0)));
			}

			break;
		}
		case ParticleEmitter::RotationMode::alignpath: {
			for(uint32_t p = 0; p < workgroupSize; p++) {
				const vec2d nextWaypoint = emitter.particleMotionPath.get(particles.life[p] + 0.01);
				particles.rotation[p] = (particles.position[p] != nextWaypoint)
					? (particles.initialRotation[p] + glm::degrees(glm::orientedAngle(vec2d(0.0, 1.0), glm::normalize(nextWaypoint - particles.position[p]))))
					: particles.rotation[p];
			}

			break;
		}
		default: {
			break;
		}
	}

	for(uint32_t p = 0; p < workgroupSize; p++) {
		const floatd alpha = std::fmod(t - emitter.lifetimeStart, emitter.lifetimeDuration) / emitter.lifetimeDuration;
		const vec2d emitterPosition = emitter.motionPath.get(alpha);
	
		particles.velocity[p] += particles.force[p] * dt;
		particles.velocity[p] *= std::pow(emitter.particleDamping.get(particles.life[p]), dt);
		particles.position[p] += particles.velocity[p] * dt;

		if(emitter.particleMotionPath.containsPoints()) {
			particles.position[p] = glm::mix(
				particles.position[p],
				emitter.particleMotionPath.get(particles.life[p]),
				emitter.particleMotionPathInfluence * 0.01);
		}

		particles.globalPosition[p] = emitter.particlePositionRelative
			? particles.position[p] + emitterPosition
			: particles.position[p];
	}
}
}