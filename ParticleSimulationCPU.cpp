#include "ParticleSimulationCPU.h"
#include <algorithm>

namespace pixelpart {
vec4d rgb2hsv(const vec4d& clrRGB) {
	vec4d result(0.0, 0.0, 0.0, clrRGB.a);

	floatd min = clrRGB.r < clrRGB.g ? clrRGB.r : clrRGB.g;
	min = min < clrRGB.b ? min : clrRGB.b;

	floatd max = clrRGB.r > clrRGB.g ? clrRGB.r : clrRGB.g;
	max = max > clrRGB.b ? max : clrRGB.b;
	result.z = max;

	floatd delta = max - min;
	if(delta < 0.00001) {
		result.y = 0.0;
		result.x = 0.0;

		return result;
	}

	if(max > 0.0) {
		result.y = delta / max;
	}
	else {
		result.y = 0.0;
		result.x = 0.0;

		return result;
	}

	if(clrRGB.r >= max) {
		result.x = (clrRGB.g - clrRGB.b) / delta;
	}
	else {
		if(clrRGB.g >= max) {
			result.x = 2.0 + (clrRGB.b - clrRGB.r) / delta;
		}
		else {
			result.x = 4.0 + (clrRGB.r - clrRGB.g) / delta;
		}
	}

	result.x *= 60.0;

	if(result.x < 0.0) {
		result.x += 360.0;
	}

	return result;
}

vec4d hsv2rgb(const vec4d& clrHSV) {
	if(clrHSV.y <= 0.0) {
		return vec4d(clrHSV.z, clrHSV.z, clrHSV.z, clrHSV.a);
	}

	floatd hh = clrHSV.x;
	if(hh >= 360.0) {
		hh = 0.0;
	}

	hh /= 60.0;

	uint32_t i = static_cast<uint32_t>(hh);
	floatd ff = hh - i;
	floatd p = clrHSV.z * (1.0 - clrHSV.y);
	floatd q = clrHSV.z * (1.0 - (clrHSV.y * ff));
	floatd t = clrHSV.z * (1.0 - (clrHSV.y * (1.0 - ff)));

	switch(i) {
		case 0:
			return vec4d(clrHSV.z, t, p, clrHSV.a);
		case 1:
			return vec4d(q, clrHSV.z, p, clrHSV.a);
		case 2:
			return vec4d(p, clrHSV.z, t, clrHSV.a);
		case 3:
			return vec4d(p, q, clrHSV.z, clrHSV.a);
		case 4:
			return vec4d(t, p, clrHSV.z, clrHSV.a);
		case 5:
		default:
			return vec4d(clrHSV.z, p, q, clrHSV.a);
	}

	return vec4d(clrHSV.z, p, q, clrHSV.a);
}

vec4d applyColorOffset(const vec4d& colorRGBA, const vec4d& offset) {
	vec4d hsv = rgb2hsv(colorRGBA);

	floatd hue = std::fmod(hsv.x + offset.x, 360.0);
	hsv.x = (hue < 0.0) ? hue + 360.0 : hue;
	hsv.y = glm::clamp(hsv.y + offset.y, 0.0, 1.0);
	hsv.z = glm::clamp(hsv.z + offset.z, 0.0, 1.0);
	hsv.w *= offset.w;

	return hsv2rgb(hsv);
}

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
				particles.numParticlesToEmit.data() + workgroupIndex,
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
				particles.initialColor.data() + workgroupIndex,
				particles.frame.data() + workgroupIndex,
				particles.initialFrame.data() + workgroupIndex
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
			particles.numParticlesToEmit.data(),
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
			particles.initialColor.data(),
			particles.frame.data(),
			particles.initialFrame.data()
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
		const floatd emitterAlpha = std::fmod(t - emitter.lifetimeStart, emitter.lifetimeDuration) / emitter.lifetimeDuration;
		const floatd particleAlpha = particles.life[p];
		const vec2d emitterPosition = emitter.motionPath.get(emitterAlpha);
		const vec2d forwardDirection = (particles.velocity[p] != vec2d(0.0, 0.0)) ? glm::normalize(particles.velocity[p]) : vec2d(0.0, 0.0);
		const vec2d radialDirection = (emitterPosition != particles.globalPosition[p]) ? glm::normalize(emitterPosition - particles.globalPosition[p]) : vec2d(0.0, 0.0);
		const int32_t frameAdvance = static_cast<int32_t>(particleAlpha * particles.lifespan[p] / emitter.particleSpriteAnimation.duration * static_cast<floatd>(emitter.particleSpriteAnimation.frames));

		particles.size[p] = vec2d(emitter.particleSize.get(particleAlpha) * particles.initialSize[p]) * vec2d(
			emitter.particleWidth.get(particleAlpha),
			emitter.particleHeight.get(particleAlpha));
		particles.color[p] = applyColorOffset(
			vec4d(vec3d(emitter.particleColor.get(particleAlpha)), emitter.visible ? emitter.particleOpacity.get(particleAlpha) : 0.0),
			particles.initialColor[p]);
		particles.frame[p] = particles.initialFrame[p];
		particles.frame[p] += emitter.particleSpriteAnimation.loop 
				? frameAdvance % emitter.particleSpriteAnimation.frames
				: std::min(frameAdvance, static_cast<int32_t>(emitter.particleSpriteAnimation.frames) - 1);
		particles.frame[p] %= static_cast<int32_t>(emitter.particleSprite.framesRow * emitter.particleSprite.framesColumn);
	
		particles.force[p] = forwardDirection * emitter.particleAcceleration.get(particleAlpha);
		particles.force[p] += radialDirection * emitter.particleRadialAcceleration.get(particleAlpha);

		forceSolver.solve(emitter.id,
			particles, p,
			emitter.particleWeight.get(particleAlpha),
			t, dt);

		collisionSolver.solve(emitter.id, 
			particles, p,
			emitter.particleBounce.get(particleAlpha),
			emitter.particleFriction.get(particleAlpha),
			t, dt);

		switch(emitter.particleRotationMode) {
			case ParticleEmitter::RotationMode::angle:
				particles.rotation[p] = particles.initialRotation[p] + emitter.particleRotation.get(particleAlpha);
				break;
			case ParticleEmitter::RotationMode::velocity:
				particles.rotation[p] += (particles.initialAngularVelocity[p] + emitter.particleRotation.get(particleAlpha)) * dt;
				break;
			case ParticleEmitter::RotationMode::align:
				particles.rotation[p] = particles.initialRotation[p] + glm::degrees(glm::orientedAngle(vec2d(0.0, 1.0), forwardDirection));
				break;
			case ParticleEmitter::RotationMode::alignpath: {
				vec2d nextWaypoint = emitter.particleMotionPath.get(particleAlpha + 0.01);
				particles.rotation[p] = (particles.globalPosition[p] != nextWaypoint)
					? (particles.initialRotation[p] + glm::degrees(glm::orientedAngle(vec2d(0.0, 1.0), glm::normalize(nextWaypoint - particles.globalPosition[p]))))
					: particles.rotation[p];
				break;
			}
			default:
				break;
		}
	
		particles.velocity[p] += particles.force[p] * dt;
		particles.velocity[p] *= std::pow(emitter.particleDamping.get(particleAlpha), dt);
		particles.position[p] += particles.velocity[p] * dt;

		if(emitter.particleMotionPath.containsPoints()) {
			particles.position[p] = glm::mix(
				particles.globalPosition[p],
				emitter.particleMotionPath.get(particleAlpha),
				emitter.particleMotionPathInfluence * 0.01) - (emitter.particlePositionRelative ? emitterPosition : vec2d(0.0));
		}

		particles.globalPosition[p] = emitter.particlePositionRelative
			? particles.position[p] + emitterPosition
			: particles.position[p];
	}
}
}