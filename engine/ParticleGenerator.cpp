#include "ParticleGenerator.h"
#include "../common/Transform.h"
#include "../effect/ParticleRuntimeId.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "../glm/gtx/euler_angles.hpp"
#include "../glm/gtx/rotate_vector.hpp"
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>

namespace pixelpart {
const float_t pi = 3.14159265358979323846;

ParticleGenerator::ParticleGenerator(const Effect& fx, ParticleRuntimeInstanceCollection& particleRuntimeInsts) :
	effect(fx), particleRuntimeInstances(particleRuntimeInsts) {

}

void ParticleGenerator::generate(const RuntimeContext& runtimeContext) {
	for(ParticleRuntimeInstance& runtimeInstance : particleRuntimeInstances) {
		const ParticleType& particleType = effect.particleTypes().at(runtimeInstance.typeId());

		std::uint32_t particleEmitterIndex = effect.sceneGraph().indexOf(runtimeInstance.emitterId());
		if(particleEmitterIndex == id_t::nullValue) {
			continue;
		}

		const ParticleEmitter& particleEmitter = effect.sceneGraph().atIndex<ParticleEmitter>(particleEmitterIndex);
		if(!particleEmitter.primary() || !particleEmitter.active(runtimeContext)) {
			continue;
		}

		float_t startTime = particleEmitter.startTrigger()
			? particleEmitter.start() + runtimeContext.triggerActivationTime(particleEmitter.startTrigger())
			: particleEmitter.start();

		float_t emissionTime = particleEmitter.repeat()
			? std::fmod(runtimeContext.time() - startTime, particleEmitter.duration())
			: runtimeContext.time() - startTime;

		switch(particleEmitter.emissionMode()) {
			case ParticleEmitter::EmissionMode::continuous:
				runtimeInstance.emissionCount() +=
					particleType.count().at(emissionTime / particleEmitter.duration()) * runtimeContext.deltaTime();
				break;
			case ParticleEmitter::EmissionMode::burst_start:
				runtimeInstance.emissionCount() +=
					emissionTime < runtimeContext.deltaTime() ? particleType.count().at(0) : 0.0;
				break;
			case ParticleEmitter::EmissionMode::burst_end:
				runtimeInstance.emissionCount() +=
					emissionTime > particleEmitter.duration() - runtimeContext.deltaTime() ? particleType.count().at(1) : 0.0;
				break;
			default:
				break;
		}

		std::uint32_t emittedParticleCount = static_cast<std::uint32_t>(std::max(runtimeInstance.emissionCount(), 0.0));
		if(emittedParticleCount > 0) {
			runtimeInstance.emissionCount() -= static_cast<float_t>(
				generate(runtimeInstance, emittedParticleCount, runtimeContext, true));
		}
	}

	std::unordered_map<ParticleRuntimeId, std::vector<ParticleRuntimeInstanceCollection::iterator>> instanceSubInstances;

	for(auto instanceIt = particleRuntimeInstances.begin(); instanceIt != particleRuntimeInstances.end(); instanceIt++) {
		std::vector<ParticleRuntimeInstanceCollection::iterator>& subInstances = instanceSubInstances[instanceIt->id()];
		const ParticleType& particleType = effect.particleTypes().at(instanceIt->typeId());

		for(auto otherInstanceIt = particleRuntimeInstances.begin(); otherInstanceIt != particleRuntimeInstances.end(); otherInstanceIt++) {
			if(instanceIt == otherInstanceIt) {
				continue;
			}

			const ParticleEmitter& otherParticleEmitter = effect.sceneGraph().at<ParticleEmitter>(otherInstanceIt->emitterId());
			if(otherParticleEmitter.primary() || !otherParticleEmitter.activatedByTrigger(runtimeContext)) {
				continue;
			}

			const ParticleType& otherParticleType = effect.particleTypes().at(otherInstanceIt->typeId());
			if(otherParticleType.parentId() == particleType.id()) {
				subInstances.emplace_back(otherInstanceIt);
			}
		}
	}

	for(ParticleRuntimeInstance& runtimeInstance : particleRuntimeInstances) {
		const std::vector<ParticleRuntimeInstanceCollection::iterator>& subInstances = instanceSubInstances.at(runtimeInstance.id());
		if(subInstances.empty()) {
			continue;
		}

		const ParticleEmitter& particleEmitter = effect.sceneGraph().at<ParticleEmitter>(runtimeInstance.emitterId());
		ParticleCollection::WritePtr particles = runtimeInstance.particles().writePtr();

		for(ParticleRuntimeInstanceCollection::iterator subInstanceIt : subInstances) {
			ParticleRuntimeInstance& subRuntimeInstance = *subInstanceIt;
			const ParticleType& subParticleType = effect.particleTypes().at(subRuntimeInstance.typeId());
			const ParticleEmitter& subParticleEmitter = effect.sceneGraph().at<ParticleEmitter>(subRuntimeInstance.emitterId());

			for(std::uint32_t p = 0; p < runtimeInstance.particles().count(); p++) {
				float_t particleTime = particles.life[p] * particles.lifespan[p];
				if(particleTime > subParticleEmitter.duration() && !subParticleEmitter.repeat()) {
					continue;
				}

				float_t emissionTime = subParticleEmitter.repeat()
					? std::fmod(particleTime, subParticleEmitter.duration())
					: particleTime;

				switch(subParticleEmitter.emissionMode()) {
					case ParticleEmitter::EmissionMode::continuous:
						subRuntimeInstance.emissionCount() +=
							subParticleType.count().at(emissionTime / particleEmitter.duration()) * runtimeContext.deltaTime();
						break;
					case ParticleEmitter::EmissionMode::burst_start:
						subRuntimeInstance.emissionCount() +=
							particles.life[p] < runtimeContext.deltaTime() / particles.lifespan[p] ? subParticleType.count().at(0) : 0.0;
						break;
					case ParticleEmitter::EmissionMode::burst_end:
						subRuntimeInstance.emissionCount() +=
							particles.life[p] > 1.0 ? subParticleType.count().at(1) : 0.0;
						break;
					default:
						break;
				}

				std::uint32_t emittedParticleCount = static_cast<std::uint32_t>(std::max(subRuntimeInstance.emissionCount(), 0.0));
				if(emittedParticleCount > 0) {
					RuntimeContext particleRuntimeContext(particleTime, 0.0);

					subRuntimeInstance.emissionCount() -= static_cast<float_t>(
						generate(subRuntimeInstance, runtimeInstance, emittedParticleCount, p, particleRuntimeContext));
				}
			}
		}
	}
}
void ParticleGenerator::generate(id_t particleEmitterId, id_t particleTypeId, std::uint32_t count, float_t time) {
	ParticleRuntimeInstance* runtimeInstance = particleRuntimeInstances.find(particleEmitterId, particleTypeId);
	if(!runtimeInstance) {
		return;
	}

	generate(*runtimeInstance, count, RuntimeContext(time, 0.0), false);
}

void ParticleGenerator::reset() {
	for(ParticleRuntimeInstance& runtimeInstance : particleRuntimeInstances) {
		runtimeInstance.reset();
	}

	nextParticleId = 0;
}

void ParticleGenerator::seed(std::uint32_t seed) {
	rng = std::mt19937(seed);
}

std::uint32_t ParticleGenerator::generate(ParticleRuntimeInstance& runtimeInstance, const ParticleRuntimeInstance& parentRuntimeInstance, std::uint32_t count,
	std::uint32_t parentParticle, const RuntimeContext& runtimeContext) {
	ParticleCollection& particleCollection = runtimeInstance.particles();
	count = particleCollection.add(count);

	const ParticleType& particleType = effect.particleTypes().at(runtimeInstance.typeId());
	const ParticleEmitter& emitter = effect.sceneGraph().at<ParticleEmitter>(runtimeInstance.emitterId());
	ParticleCollection::WritePtr particles = particleCollection.writePtr();
	ParticleCollection::ReadPtr parentParticles = parentRuntimeInstance.particles().readPtr();

	float_t alpha = emitter.life(runtimeContext, false);

	Transform localEmitterTransform = effect.sceneGraph().localTransform(emitter.id(), runtimeContext, false);
	float3_t localEmitterPosition = localEmitterTransform.position();

	Transform globalEmitterTransform = effect.sceneGraph().globalTransform(emitter.id(), runtimeContext, false);
	float3_t globalEmitterPosition = globalEmitterTransform.position();
	float3_t globalEmitterRotation = globalEmitterTransform.rotation();
	float3_t globalEmitterSize = globalEmitterTransform.scale();

	float3_t emissionPosition = parentParticles.globalPosition[parentParticle] + localEmitterPosition +
		(particleType.positionRelative() ? -globalEmitterPosition : float3_t(0.0));
	std::uint32_t parentId = parentParticles.id[parentParticle];
	float3_t parentVelocity = parentParticles.velocity[parentParticle];

	for(std::uint32_t addIndex = 0; addIndex < count; addIndex++) {
		std::uint32_t p = particleCollection.count() - count + addIndex;

		initializeParticle(runtimeInstance, particles, p, parentId,
			emitter, particleType, alpha,
			globalEmitterPosition, globalEmitterRotation, globalEmitterSize,
			emissionPosition, parentVelocity);
	}

	return count;
}
std::uint32_t ParticleGenerator::generate(ParticleRuntimeInstance& runtimeInstance, std::uint32_t count,
	const RuntimeContext& runtimeContext, bool useTriggers) {
	ParticleCollection& particleCollection = runtimeInstance.particles();
	count = particleCollection.add(count);

	const ParticleType& particleType = effect.particleTypes().at(runtimeInstance.typeId());
	const ParticleEmitter& emitter = effect.sceneGraph().at<ParticleEmitter>(runtimeInstance.emitterId());
	ParticleCollection::WritePtr particles = particleCollection.writePtr();

	RuntimeContext prevRuntimeContext(runtimeContext.time() - 0.1, 0.0, runtimeContext.triggerActivationTimes());

	float_t alpha = emitter.life(runtimeContext, useTriggers);

	Transform globalEmitterTransform = effect.sceneGraph().globalTransform(emitter.id(), runtimeContext, useTriggers);
	Transform globalEmitterPrevTransform = effect.sceneGraph().globalTransform(emitter.id(), prevRuntimeContext, useTriggers);
	float3_t globalEmitterPosition = globalEmitterTransform.position();
	float3_t globalEmitterRotation = globalEmitterTransform.rotation();
	float3_t globalEmitterSize = globalEmitterTransform.scale();

	float3_t emissionPosition = !particleType.positionRelative()
		? globalEmitterPosition
		: float3_t(0.0);
	float3_t parentVelocity = (globalEmitterPosition - globalEmitterPrevTransform.position()) / (emitter.duration() * 0.1);

	for(std::uint32_t addIndex = 0; addIndex < count; addIndex++) {
		std::uint32_t p = particleCollection.count() - count + addIndex;

		initializeParticle(runtimeInstance, particles, p, id_t::nullValue,
			emitter, particleType, alpha,
			globalEmitterPosition, globalEmitterRotation, globalEmitterSize,
			emissionPosition, parentVelocity);
	}

	return count;
}

void ParticleGenerator::initializeParticle(ParticleRuntimeInstance& runtimeInstance, ParticleCollection::WritePtr particles, std::uint32_t p, std::uint32_t parentId,
	const ParticleEmitter& particleEmitter, const ParticleType& particleType, float_t alpha,
	const float3_t& globalEmitterPosition, const float3_t& globalEmitterRotation, const float3_t& globalEmitterSize,
	const float3_t& emissionPosition, const float3_t& parentVelocity) {
	particles.id[p] = nextParticleId++;
	particles.parentId[p] = parentId;
	particles.life[p] = 0.0;
	particles.lifespan[p] = std::max(particleType.lifespan().at(alpha) +
		random::uniform(rng, -particleType.lifespanVariance().value(), +particleType.lifespanVariance().value()), 0.000001);

	float3_t particleSpawnPosition = float3_t(0.0);
	switch(particleEmitter.shape()) {
		case ParticleEmitter::Shape::line:
			particleSpawnPosition = emitOnSegment(
				globalEmitterSize.x,
				particleEmitter.distribution(),
				particleEmitter.gridOrder(),
				particleEmitter.gridSizeX(),
				runtimeInstance.emitterGridIndex());
			break;
		case ParticleEmitter::Shape::ellipse:
			particleSpawnPosition = emitInEllipse(
				globalEmitterSize,
				particleEmitter.distribution(),
				particleEmitter.gridOrder(),
				particleEmitter.gridSizeX(), particleEmitter.gridSizeY(),
				runtimeInstance.emitterGridIndex());
			break;
		case ParticleEmitter::Shape::rectangle:
			particleSpawnPosition = emitInRectangle(
				globalEmitterSize,
				particleEmitter.distribution(),
				particleEmitter.gridOrder(),
				particleEmitter.gridSizeX(), particleEmitter.gridSizeY(),
				runtimeInstance.emitterGridIndex());
			break;
		case ParticleEmitter::Shape::path:
			particleSpawnPosition = emitOnPath(
				globalEmitterSize,
				particleEmitter.path(),
				particleEmitter.distribution(),
				particleEmitter.gridOrder(),
				particleEmitter.gridSizeX(),
				runtimeInstance.emitterGridIndex());
			break;
		case ParticleEmitter::Shape::ellipsoid:
			particleSpawnPosition = emitInEllipsoid(
				globalEmitterSize,
				particleEmitter.distribution(),
				particleEmitter.gridOrder(),
				particleEmitter.gridSizeX(), particleEmitter.gridSizeY(), particleEmitter.gridSizeZ(),
				runtimeInstance.emitterGridIndex());
			break;
		case ParticleEmitter::Shape::cuboid:
			particleSpawnPosition = emitInCuboid(
				globalEmitterSize,
				particleEmitter.distribution(),
				particleEmitter.gridOrder(),
				particleEmitter.gridSizeX(), particleEmitter.gridSizeY(), particleEmitter.gridSizeZ(),
				runtimeInstance.emitterGridIndex());
			break;
		case ParticleEmitter::Shape::cylinder:
			particleSpawnPosition = emitInCylinder(
				globalEmitterSize,
				particleEmitter.distribution(),
				particleEmitter.gridOrder(),
				particleEmitter.gridSizeX(), particleEmitter.gridSizeY(), particleEmitter.gridSizeZ(),
				runtimeInstance.emitterGridIndex());
			break;
		default:
			break;
	}

	particleSpawnPosition = effect.is3d()
		? rotate3d(particleSpawnPosition, globalEmitterRotation)
		: rotate2d(particleSpawnPosition, globalEmitterRotation.x);

	particles.position[p] = emissionPosition + particleSpawnPosition;
	particles.globalPosition[p] = particleType.positionRelative()
		? particles.position[p] + globalEmitterPosition
		: particles.position[p];

	if(effect.is3d()) {
		mat3_t rotationMatrix = mat3_t(glm::yawPitchRoll(
			glm::radians(globalEmitterRotation.y),
			glm::radians(globalEmitterRotation.z),
			glm::radians(globalEmitterRotation.x)));
		mat3_t directionMatrix = mat3_t(glm::yawPitchRoll(
			glm::radians(particleEmitter.direction().at(alpha).y + particleEmitter.spread().at(alpha) * random::uniform(rng, -0.5, +0.5)),
			glm::radians(particleEmitter.direction().at(alpha).z + particleEmitter.spread().at(alpha) * random::uniform(rng, -0.5, +0.5)),
			glm::radians(particleEmitter.direction().at(alpha).x + particleEmitter.spread().at(alpha) * random::uniform(rng, -0.5, +0.5))));

		switch(particleEmitter.directionMode()) {
			case ParticleEmitter::DirectionMode::outwards:
				particles.velocity[p] = directionMatrix *
					((particleSpawnPosition != float3_t(0.0)) ? glm::normalize(+particleSpawnPosition) : worldUpVector3);
				break;
			case ParticleEmitter::DirectionMode::inwards:
				particles.velocity[p] = directionMatrix *
					((particleSpawnPosition != float3_t(0.0)) ? glm::normalize(-particleSpawnPosition) : worldUpVector3);
				break;
			case ParticleEmitter::DirectionMode::inherit:
				particles.velocity[p] = directionMatrix *
					glm::normalize(parentVelocity != float3_t(0.0) ? +parentVelocity : worldUpVector3);
				break;
			case ParticleEmitter::DirectionMode::inherit_inverse:
				particles.velocity[p] = directionMatrix *
					glm::normalize(parentVelocity != float3_t(0.0) ? -parentVelocity : worldUpVector3);
				break;
			default:
				particles.velocity[p] = rotationMatrix * directionMatrix * worldUpVector3;
				break;
		}
	}
	else {
		float_t direction = glm::radians(
			particleEmitter.direction().at(alpha).x +
			particleEmitter.spread().at(alpha) * random::uniform(rng,-0.5, +0.5));

		switch(particleEmitter.directionMode()) {
			case ParticleEmitter::DirectionMode::outwards:
				particles.velocity[p] = float3_t(glm::rotate(
					(particleSpawnPosition != float3_t(0.0)) ? float2_t(glm::normalize(+particleSpawnPosition)) : worldUpVector2,
					direction), 0.0);
				break;
			case ParticleEmitter::DirectionMode::inwards:
				particles.velocity[p] = float3_t(glm::rotate(
					(particleSpawnPosition != float3_t(0.0)) ? float2_t(glm::normalize(-particleSpawnPosition)) : worldUpVector2,
					direction), 0.0);
				break;
			case ParticleEmitter::DirectionMode::inherit:
				particles.velocity[p] = float3_t(glm::rotate(
					float2_t(glm::normalize(parentVelocity != float3_t(0.0) ? +parentVelocity : worldUpVector3)), direction), 0.0);
				break;
			case ParticleEmitter::DirectionMode::inherit_inverse:
				particles.velocity[p] = float3_t(glm::rotate(
					float2_t(glm::normalize(parentVelocity != float3_t(0.0) ? -parentVelocity : worldUpVector3)), direction), 0.0);
				break;
			default:
				particles.velocity[p] = float3_t(glm::rotate(worldUpVector2, glm::radians(globalEmitterRotation.x) + direction), 0.0);
				break;
		}
	}

	particles.velocity[p] *= glm::mix(
			particleType.initialVelocity().at(alpha),
			glm::length(parentVelocity),
			particleType.inheritedVelocity().at(alpha)) + random::uniform(rng, -particleType.velocityVariance().value(), +particleType.velocityVariance().value());
	particles.force[p] = float3_t(0.0);

	particles.initialRotation[p] = particleType.initialRotation().at(alpha) + float3_t(
		random::uniform(rng, -particleType.rotationVariance().value().x, +particleType.rotationVariance().value().x),
		random::uniform(rng, -particleType.rotationVariance().value().y, +particleType.rotationVariance().value().y),
		random::uniform(rng, -particleType.rotationVariance().value().z, +particleType.rotationVariance().value().z));
	particles.initialAngularVelocity[p] = float3_t(
		random::uniform(rng, -particleType.angularVelocityVariance().value().x, +particleType.angularVelocityVariance().value().x),
		random::uniform(rng, -particleType.angularVelocityVariance().value().y, +particleType.angularVelocityVariance().value().y),
		random::uniform(rng, -particleType.angularVelocityVariance().value().z, +particleType.angularVelocityVariance().value().z));
	particles.rotation[p] = particles.initialRotation[p];

	particles.initialSize[p] = particleType.initialSize().at(alpha) + random::uniform(rng, -particleType.sizeVariance().value(), +particleType.sizeVariance().value());
	particles.size[p] = particleType.size().at() * particles.initialSize[p];

	particles.initialColor[p] = float4_t(
		random::uniform(rng, -particleType.colorVariance().value().x, +particleType.colorVariance().value().x),
		random::uniform(rng, -particleType.colorVariance().value().y, +particleType.colorVariance().value().y),
		random::uniform(rng, -particleType.colorVariance().value().z, +particleType.colorVariance().value().z),
		particleType.initialOpacity().at(alpha) + random::uniform(rng, -particleType.opacityVariance().value(), +particleType.opacityVariance().value()));
	particles.color[p] = float4_t(float3_t(particleType.color().at()), particleType.opacity().at());
}

float_t ParticleGenerator::sampleGrid1D(std::uint32_t gridIndex, std::uint32_t gridSize, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex % gridSize) / static_cast<float_t>(gridSize - 1) * (max - min) + min;
}
float_t ParticleGenerator::sampleGrid2D(std::uint32_t gridIndex, std::uint32_t gridSize1, std::uint32_t gridSize2, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex / gridSize1 % gridSize2) / static_cast<float_t>(gridSize2 - 1) * (max - min) + min;
}
float_t ParticleGenerator::sampleGrid3D(std::uint32_t gridIndex, std::uint32_t gridSize1, std::uint32_t gridSize2, std::uint32_t gridSize3, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex / gridSize1 / gridSize2 % gridSize3) / static_cast<float_t>(gridSize3 - 1) * (max - min) + min;
}

float3_t ParticleGenerator::rotate2d(const float3_t& v, float_t a) {
	return float3_t(glm::rotate(float2_t(v), glm::radians(a)), 0.0);
}
float3_t ParticleGenerator::rotate3d(const float3_t& v, const float3_t& a) {
	return float3_t(glm::yawPitchRoll(glm::radians(a.y), glm::radians(a.z), glm::radians(a.x)) * float4_t(v, 0.0));
}

float3_t ParticleGenerator::emitOnSegment(float_t length,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSize, std::uint32_t& gridIndex) {
	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary: {
			return float3_t(0.0, random::uniform(rng, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::center: {
			return float3_t(0.0, random::normal(rng, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::hole: {
			return float3_t(0.0, random::normalReverse(rng, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::grid_random: {
			return float3_t(0.0, random::uniformGrid(rng, gridSize, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			float_t y = sampleGrid1D(gridIndex, gridSize, -length, +length) * 0.5;
			gridIndex = (gridIndex + 1) % gridSize;
			return float3_t(0.0, y, 0.0);
		}
		default: {
			return float3_t(0.0, 0.0, 0.0);
		}
	}
}

float3_t ParticleGenerator::emitInEllipse(const float2_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t& gridIndex) {
	float_t r = 0.0;
	float_t phi = 0.0;
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			r = std::sqrt(random::uniform(rng, 0.0, 1.0));
			phi = random::uniform(rng, 0.0, 2.0 * pi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				point = float3_t(
					std::cos(phi),
					std::sin(phi),
					0.0) * r;
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 2) < point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				point = float3_t(
					std::cos(phi),
					std::sin(phi),
					0.0) * r;
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 2) > point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			phi = random::uniform(rng, 0.0, 2.0 * pi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			r = random::uniformGrid(rng, gridSizeX);
			phi = random::uniformGrid(rng, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
				case ParticleEmitter::GridOrder::x_z_y:
				case ParticleEmitter::GridOrder::z_x_y:
					r = sampleGrid1D(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2D(gridIndex, gridSizeX, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					break;
				case ParticleEmitter::GridOrder::y_x_z:
				case ParticleEmitter::GridOrder::y_z_x:
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid2D(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1D(gridIndex, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1) % (gridSizeX * gridSizeY);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		default: {
			break;
		}
	}

	point.x *= size.x * 0.5;
	point.y *= size.y * 0.5;

	return point;
}

float3_t ParticleGenerator::emitInRectangle(const float2_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t& gridIndex) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = float3_t(
				random::uniform(rng, -size.x, +size.x),
				random::uniform(rng, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = float3_t(
				random::normal(rng, -size.x, +size.x),
				random::normal(rng, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				point = float3_t(
					random::uniform(rng, -1.0, +1.0),
					random::uniform(rng, -1.0, +1.0),
					0.0);
			}
			while(random::uniform(rng, 0.0, 1.0) > (point.x * point.x + point.y * point.y) * 0.5);

			point.x *= size.x * 0.5;
			point.y *= size.y * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t r = random::uniform(rng, 0.0, (size.x + size.y) * 2.0);
			point = float3_t(-size * 0.5, 0.0);

			if(r < size.y) {
				point += float3_t(0.0, r, 0.0);
			}
			else if(r < size.y + size.x) {
				point += float3_t(r - size.y, size.y, 0.0);
			}
			else if(r < (size.y + size.x) + size.y) {
				point += float3_t(size.x, r - (size.y + size.x), 0.0);
			}
			else {
				point += float3_t(r - (size.y + size.x + size.y), 0.0, 0.0);
			}

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			point = float3_t(
				random::uniformGrid(rng, gridSizeX, -size.x, +size.x),
				random::uniformGrid(rng, gridSizeY, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
				case ParticleEmitter::GridOrder::x_z_y:
				case ParticleEmitter::GridOrder::z_x_y:
					point = float3_t(
						sampleGrid1D(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid2D(gridIndex, gridSizeX, gridSizeY, -size.y, +size.y),
						0.0) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_x_z:
				case ParticleEmitter::GridOrder::y_z_x:
				case ParticleEmitter::GridOrder::z_y_x:
					point = float3_t(
						sampleGrid2D(gridIndex, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid1D(gridIndex, gridSizeY, -size.y, +size.y),
						0.0) * 0.5;
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1) % (gridSizeX * gridSizeY);

			break;
		}
		default: {
			break;
		}
	}

	return point;
}

float3_t ParticleGenerator::emitOnPath(const float3_t& size,
	const Curve<float3_t>& path,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSize, std::uint32_t& gridIndex) {
	float_t x = 0.0;

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary: {
			x = random::uniform(rng, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::center: {
			x = random::normal(rng, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::hole: {
			x = random::normalReverse(rng, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			x = random::uniformGrid(rng, gridSize);
			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			x = sampleGrid1D(gridIndex, gridSize, 0.0, 1.0);
			gridIndex = (gridIndex + 1) % gridSize;
			break;
		}
		default: {
			break;
		}
	}

	return path.at(x) * size;
}

float3_t ParticleGenerator::emitInEllipsoid(const float3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex) {
	float_t r = 0.0;
	float_t phi = 0.0;
	float_t theta = 0.0;
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			r = std::sqrt(random::uniform(rng, 0.0, 1.0));
			phi = random::uniform(rng, 0.0, 2.0 * pi);
			theta = std::acos(random::uniform(rng, -1.0, +1.0));
			point = float3_t(
				std::sin(theta) * std::cos(phi),
				std::sin(theta) * std::sin(phi),
				std::cos(theta)) * r;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				theta = std::acos(random::uniform(rng, -1.0, +1.0));
				point = float3_t(
					std::sin(theta) * std::cos(phi),
					std::sin(theta) * std::sin(phi),
					std::cos(theta)) * r;
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 3) < point.x * point.x + point.y * point.y + point.z * point.z);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				theta = std::acos(random::uniform(rng, -1.0, +1.0));
				point = float3_t(
					std::sin(theta) * std::cos(phi),
					std::sin(theta) * std::sin(phi),
					std::cos(theta)) * r;
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 3) > point.x * point.x + point.y * point.y + point.z * point.z);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			phi = random::uniform(rng, 0.0, 2.0 * pi);
			theta = std::acos(random::uniform(rng, -1.0, +1.0));
			point = float3_t(
				std::sin(theta) * std::cos(phi),
				std::sin(theta) * std::sin(phi),
				std::cos(theta));

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			r = random::uniformGrid(rng, gridSizeX);
			phi = random::uniformGrid(rng, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			theta = random::uniformGrid(rng, gridSizeZ, 0.0, pi);
			point = float3_t(
				std::sin(theta) * std::cos(phi),
				std::sin(theta) * std::sin(phi),
				std::cos(theta)) * r;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					r = sampleGrid1D(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2D(gridIndex, gridSizeX, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid3D(gridIndex, gridSizeX, gridSizeY, gridSizeZ, 0.0, pi);
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					r = sampleGrid1D(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3D(gridIndex, gridSizeX, gridSizeZ, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid2D(gridIndex, gridSizeX, gridSizeZ, 0.0, pi);
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					r = sampleGrid2D(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1D(gridIndex, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid3D(gridIndex, gridSizeY, gridSizeX, gridSizeZ, 0.0, pi);
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					r = sampleGrid3D(gridIndex, gridSizeY, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1D(gridIndex, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid2D(gridIndex, gridSizeY, gridSizeZ, 0.0, pi);
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					r = sampleGrid2D(gridIndex, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3D(gridIndex, gridSizeZ, gridSizeX, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid1D(gridIndex, gridSizeZ, 0.0, pi);
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid3D(gridIndex, gridSizeZ, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2D(gridIndex, gridSizeZ, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid1D(gridIndex, gridSizeZ, 0.0, pi);
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1) % (gridSizeX * gridSizeY * gridSizeZ);
			point = float3_t(
				std::sin(theta) * std::cos(phi),
				std::sin(theta) * std::sin(phi),
				std::cos(theta)) * r;

			break;
		}
		default: {
			break;
		}
	}

	point *= size * 0.5;

	return point;
}

float3_t ParticleGenerator::emitInCuboid(const float3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = float3_t(
				random::uniform(rng, -size.x, +size.x),
				random::uniform(rng, -size.y, +size.y),
				random::uniform(rng, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = float3_t(
				random::normal(rng, -size.x, +size.x),
				random::normal(rng, -size.y, +size.y),
				random::normal(rng, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				point = float3_t(
					random::uniform(rng, -1.0, +1.0),
					random::uniform(rng, -1.0, +1.0),
					random::uniform(rng, -1.0, +1.0));
			}
			while(random::uniform(rng, 0.0, 1.0) > (point.x * point.x + point.y * point.y + point.z * point.z) * 0.5);

			point *= size * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			std::int32_t side = random::uniformInt(rng, 0, 5);
			switch(side) {
				case 0:
					point = float3_t(-size.x, random::uniform(rng, -size.y, +size.y), random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 1:
					point = float3_t(+size.x, random::uniform(rng, -size.y, +size.y), random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 2:
					point = float3_t(random::uniform(rng, -size.x, +size.x), -size.y, random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 3:
					point = float3_t(random::uniform(rng, -size.x, +size.x), +size.y, random::uniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 4:
					point = float3_t(random::uniform(rng, -size.x, +size.x), random::uniform(rng, -size.y, +size.y), -size.z) * 0.5;
					break;
				case 5:
					point = float3_t(random::uniform(rng, -size.x, +size.x), random::uniform(rng, -size.y, +size.y), +size.z) * 0.5;
					break;
				default:
					point = float3_t(0.0);
					break;
			}

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			point = float3_t(
				random::uniformGrid(rng, gridSizeX, -size.x, +size.x),
				random::uniformGrid(rng, gridSizeY, -size.y, +size.y),
				random::uniformGrid(rng, gridSizeZ, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					point = float3_t(
						sampleGrid1D(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid2D(gridIndex, gridSizeX, gridSizeY, -size.y, +size.y),
						sampleGrid3D(gridIndex, gridSizeX, gridSizeY, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					point = float3_t(
						sampleGrid1D(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid3D(gridIndex, gridSizeX, gridSizeZ, gridSizeY, -size.y, +size.y),
						sampleGrid2D(gridIndex, gridSizeX, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					point = float3_t(
						sampleGrid2D(gridIndex, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid1D(gridIndex, gridSizeY, -size.y, +size.y),
						sampleGrid3D(gridIndex, gridSizeY, gridSizeX, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					point = float3_t(
						sampleGrid3D(gridIndex, gridSizeY, gridSizeZ, gridSizeX, -size.x, +size.x),
						sampleGrid1D(gridIndex, gridSizeY, -size.y, +size.y),
						sampleGrid2D(gridIndex, gridSizeY, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					point = float3_t(
						sampleGrid2D(gridIndex, gridSizeZ, gridSizeX, -size.x, +size.y),
						sampleGrid3D(gridIndex, gridSizeZ, gridSizeX, gridSizeY, -size.y, +size.y),
						sampleGrid1D(gridIndex, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					point = float3_t(
						sampleGrid3D(gridIndex, gridSizeZ, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid2D(gridIndex, gridSizeZ, gridSizeY, -size.y, +size.y),
						sampleGrid1D(gridIndex, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1) % (gridSizeX * gridSizeY * gridSizeZ);

			break;
		}
		default: {
			break;
		}
	}

	return point;
}

float3_t ParticleGenerator::emitInCylinder(const float3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex) {
	float_t h = 0.0;
	float_t r = 0.0;
	float_t phi = 0.0;
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			h = random::uniform(rng, -1.0, +1.0);
			phi = random::uniform(rng, 0.0, 2.0 * pi);
			r = std::sqrt(random::uniform(rng, 0.0, 1.0));

			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::center: {
			h = random::uniform(rng, -1.0, +1.0);
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				point = float3_t(
					std::cos(phi) * r,
					std::sin(phi) * r,
					h);
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 2) < point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			h = random::uniform(rng, -1.0, +1.0);
			do {
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
				phi = random::uniform(rng, 0.0, 2.0 * pi);
				point = float3_t(
					std::cos(phi) * r,
					std::sin(phi) * r,
					h);
			}
			while(std::pow(random::uniform(rng, 0.0, 1.0), 2) > point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t baseArea = size.x * size.y * pi;
			float_t lateralArea = pi * (size.x + size.y) * size.z;
			float_t side = random::uniform(rng, 0.0, baseArea * 2.0 + lateralArea);
			if(side < baseArea) {
				h = -1.0;
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
			}
			else if(side < baseArea * 2.0) {
				h = +1.0;
				r = std::sqrt(random::uniform(rng, 0.0, 1.0));
			}
			else {
				h = random::uniform(rng, -1.0, +1.0);
				r = 1.0;
			}

			phi = random::uniform(rng, 0.0, 2.0 * pi);
			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			r = random::uniformGrid(rng, gridSizeX);
			phi = random::uniformGrid(rng, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			h = random::uniformGrid(rng, gridSizeZ, -1.0, +1.0);
			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					r = sampleGrid1D(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2D(gridIndex, gridSizeX, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid3D(gridIndex, gridSizeX, gridSizeY, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					r = sampleGrid1D(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3D(gridIndex, gridSizeX, gridSizeZ, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid2D(gridIndex, gridSizeX, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					r = sampleGrid2D(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1D(gridIndex, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid3D(gridIndex, gridSizeY, gridSizeX, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					r = sampleGrid3D(gridIndex, gridSizeY, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1D(gridIndex, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid2D(gridIndex, gridSizeY, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					r = sampleGrid2D(gridIndex, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3D(gridIndex, gridSizeZ, gridSizeX, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid1D(gridIndex, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid3D(gridIndex, gridSizeZ, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2D(gridIndex, gridSizeZ, gridSizeY, 0.0, 2.0 * pi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid1D(gridIndex, gridSizeZ, -1.0, +1.0);
					break;
				default:
					break;
			}

			gridIndex = (gridIndex + 1) % (gridSizeX * gridSizeY * gridSizeZ);
			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		default: {
			break;
		}
	}

	point *= size * 0.5;

	return point;
}
}