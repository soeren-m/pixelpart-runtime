#include "DefaultParticleGenerator.h"
#include "../common/Coordinates.h"
#include "../math/Common.h"
#include "../math/Constants.h"
#include "../math/Geometry.h"
#include "../math/Trigonometry.h"
#include "../math/Transformation.h"
#include "../math/Interpolation.h"
#include "../math/Random.h"
#include <cmath>
#include <optional>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace pixelpart {
void DefaultParticleGenerator::generate(EffectRuntimeState& state,
	const Effect* effect, EffectRuntimeContext runtimeContext) const {
	for(auto& [emissionPair, particleCollection] : state.particleCollections()) {
		const ParticleType& particleType = effect->particleTypes().at(emissionPair.typeId);

		std::optional<std::uint32_t> particleEmitterIndex = effect->sceneGraph().indexOf(emissionPair.emitterId);
		if(!particleEmitterIndex) {
			continue;
		}

		const ParticleEmitter& particleEmitter = effect->sceneGraph().atIndex<ParticleEmitter>(particleEmitterIndex.value());
		if(!particleEmitter.primary() || !particleEmitter.active(runtimeContext)) {
			continue;
		}

		ParticleEmissionState& emissionState = state.particleEmissionStates().at(emissionPair);

		float_t startTime = particleEmitter.startTrigger()
			? particleEmitter.start() + runtimeContext.triggerActivationTime(particleEmitter.startTrigger())
			: particleEmitter.start();

		float_t emissionTime = particleEmitter.repeat()
			? std::fmod(runtimeContext.time() - startTime, particleEmitter.duration())
			: runtimeContext.time() - startTime;

		switch(particleEmitter.emissionMode()) {
			case ParticleEmitter::EmissionMode::continuous:
				emissionState.emissionCount +=
					particleType.count().at(emissionTime / particleEmitter.duration()) * runtimeContext.deltaTime();
				break;
			case ParticleEmitter::EmissionMode::burst_start:
				emissionState.emissionCount +=
					emissionTime < runtimeContext.deltaTime() ? particleType.count().at(0) : 0.0;
				break;
			case ParticleEmitter::EmissionMode::burst_end:
				emissionState.emissionCount +=
					emissionTime > particleEmitter.duration() - runtimeContext.deltaTime() ? particleType.count().at(1) : 0.0;
				break;
			default:
				break;
		}

		std::uint32_t emittedParticleCount = static_cast<std::uint32_t>(std::max(emissionState.emissionCount, 0.0));
		if(emittedParticleCount > 0) {
			emissionState.emissionCount -= static_cast<float_t>(initializeParticles(emittedParticleCount,
				state, emissionState,
				particleCollection, nullptr, id_t::nullValue,
				effect, emissionPair, runtimeContext, true));
		}
	}

	std::unordered_map<ParticleEmissionPair, std::vector<ParticleEmissionPair>> subEmissionPairCollection;

	for(const auto& [emissionPair, particleCollection] : state.particleCollections()) {
		std::vector<ParticleEmissionPair>& subEmissionPairs = subEmissionPairCollection[emissionPair];
		const ParticleType& particleType = effect->particleTypes().at(emissionPair.typeId);

		for(const auto& [otherEmissionPair, otherParticleCollection] : state.particleCollections()) {
			if(emissionPair == otherEmissionPair) {
				continue;
			}

			const ParticleEmitter& otherParticleEmitter = effect->sceneGraph().at<ParticleEmitter>(otherEmissionPair.emitterId);
			if(otherParticleEmitter.primary() || !otherParticleEmitter.activatedByTrigger(runtimeContext)) {
				continue;
			}

			const ParticleType& otherParticleType = effect->particleTypes().at(otherEmissionPair.typeId);
			if(otherParticleType.parentId() == particleType.id()) {
				subEmissionPairs.emplace_back(otherEmissionPair);
			}
		}
	}

	for(const auto& [emissionPair, particleCollection] : state.particleCollections()) {
		const std::vector<ParticleEmissionPair>& subEmissionPairs = subEmissionPairCollection.at(emissionPair);
		if(subEmissionPairs.empty()) {
			continue;
		}

		const ParticleEmitter& particleEmitter = effect->sceneGraph().at<ParticleEmitter>(emissionPair.emitterId);
		ParticleCollection::ReadPtr particles = particleCollection.readPtr();

		for(ParticleEmissionPair subEmissionPair : subEmissionPairs) {
			ParticleCollection& subParticleCollection = state.particleCollections().at(subEmissionPair);
			ParticleEmissionState& subEmissionState = state.particleEmissionStates().at(subEmissionPair);
			const ParticleType& subParticleType = effect->particleTypes().at(subEmissionPair.typeId);
			const ParticleEmitter& subParticleEmitter = effect->sceneGraph().at<ParticleEmitter>(subEmissionPair.emitterId);

			for(std::uint32_t p = 0; p < particleCollection.count(); p++) {
				float_t particleTime = particles.life[p] * particles.lifespan[p];
				if(particleTime > subParticleEmitter.duration() && !subParticleEmitter.repeat()) {
					continue;
				}

				float_t emissionTime = subParticleEmitter.repeat()
					? std::fmod(particleTime, subParticleEmitter.duration())
					: particleTime;

				switch(subParticleEmitter.emissionMode()) {
					case ParticleEmitter::EmissionMode::continuous:
						subEmissionState.emissionCount +=
							subParticleType.count().at(emissionTime / particleEmitter.duration()) * runtimeContext.deltaTime();
						break;
					case ParticleEmitter::EmissionMode::burst_start:
						subEmissionState.emissionCount +=
							particles.life[p] < runtimeContext.deltaTime() / particles.lifespan[p] ? subParticleType.count().at(0) : 0.0;
						break;
					case ParticleEmitter::EmissionMode::burst_end:
						subEmissionState.emissionCount +=
							particles.life[p] > 1.0 ? subParticleType.count().at(1) : 0.0;
						break;
					default:
						break;
				}

				std::uint32_t emittedParticleCount = static_cast<std::uint32_t>(std::max(subEmissionState.emissionCount, 0.0));
				if(emittedParticleCount > 0) {
					EffectRuntimeContext particleRuntimeContext(particleTime, 0.0);

					subEmissionState.emissionCount -= static_cast<float_t>(initializeParticles(emittedParticleCount,
						state, subEmissionState,
						subParticleCollection, &particleCollection, p,
						effect, subEmissionPair, particleRuntimeContext, false));
				}
			}
		}
	}

	for(auto& [emissionPair, particleCollection] : state.particleCollections()) {
		particleCollection.removeDead();
	}
}
void DefaultParticleGenerator::clear(EffectRuntimeState& state) const {
	for(auto& [emissionPair, particleCollection] : state.particleCollections()) {
		particleCollection.clear();
	}

	for(auto& [emissionPair, particleEmissionState] : state.particleEmissionStates()) {
		particleEmissionState.emissionCount = 0;
		particleEmissionState.emitterGridIndex = 0;
	}

	state.particleIdCounter() = 0;
}

void DefaultParticleGenerator::generate(EffectRuntimeState& state, std::uint32_t count,
	const Effect* effect, id_t particleEmitterId, id_t particleTypeId, EffectRuntimeContext runtimeContext) const {
	ParticleEmissionPair emissionPair(particleEmitterId, particleTypeId);
	ParticleCollection* particleCollection = state.particleCollection(particleEmitterId, particleTypeId);
	ParticleEmissionState* particleEmissionState = state.particleEmissionState(particleEmitterId, particleTypeId);
	if(!particleCollection || !particleEmissionState) {
		return;
	}

	initializeParticles(count,
		state, *particleEmissionState,
		*particleCollection, nullptr, id_t::nullValue,
		effect, emissionPair, runtimeContext, false);
}

std::uint32_t DefaultParticleGenerator::initializeParticles(std::uint32_t count,
	EffectRuntimeState& state, ParticleEmissionState& emissionState,
	ParticleCollection& particleCollection, const ParticleCollection* parentParticleCollection, std::uint32_t parentParticle,
	const Effect* effect, ParticleEmissionPair emissionPair, EffectRuntimeContext runtimeContext,
	bool useTriggers) {
	const ParticleType& particleType = effect->particleTypes().at(emissionPair.typeId);
	const ParticleEmitter& particleEmitter = effect->sceneGraph().at<ParticleEmitter>(emissionPair.emitterId);

	float_t emitterLife = particleEmitter.life(runtimeContext, useTriggers && parentParticle == id_t::nullValue);
	Transform emitterTransform = effect->sceneGraph().globalTransform(particleEmitter.id(), runtimeContext, useTriggers && parentParticle == id_t::nullValue);
	float3_t emitterPosition = emitterTransform.position();
	float3_t emitterRotation = math::radians(emitterTransform.rotation());
	float3_t emitterScale = emitterTransform.scale();
	matrix3_t emitterRotationMatrix = matrix3_t(math::yawPitchRollRotationMatrix(emitterRotation.y, emitterRotation.z, emitterRotation.x));
	float3_t emitterDirection = particleEmitter.direction().at(emitterLife);
	float_t emitterSpread = particleEmitter.spread().at(emitterLife);

	std::uint32_t parentId = id_t::nullValue;

	float3_t emissionPosition;
	float3_t parentVelocity;

	if(parentParticle == id_t::nullValue) {
		EffectRuntimeContext prevRuntimeContext(runtimeContext.time() - 0.1, 0.0, runtimeContext.triggerActivationTimes());
		Transform emitterPrevTransform = effect->sceneGraph().globalTransform(particleEmitter.id(), prevRuntimeContext, useTriggers);

		emissionPosition = !particleType.positionRelative() ? emitterPosition : float3_t(0.0);
		parentVelocity = (emitterPosition - emitterPrevTransform.position()) / (particleEmitter.duration() * 0.1);
	}
	else {
		ParticleCollection::ReadPtr parentParticles = parentParticleCollection->readPtr();

		Transform localEmitterTransform = effect->sceneGraph().localTransform(particleEmitter.id(), runtimeContext, false);
		float3_t localEmitterPosition = localEmitterTransform.position();

		emissionPosition = parentParticles.globalPosition[parentParticle] + localEmitterPosition +
			(particleType.positionRelative() ? -emitterPosition : float3_t(0.0));
		parentId = parentParticles.id[parentParticle];
		parentVelocity = parentParticles.velocity[parentParticle];
	}

	float_t parentSpeed = math::length(parentVelocity);
	float3_t parentDirection = parentSpeed != 0.0 ? parentVelocity / parentSpeed : worldUpVector3;

	count = particleCollection.add(count);

	ParticleCollection::WritePtr particles = particleCollection.writePtr();
	std::uint32_t particleCollectionCount = particleCollection.count();

	bool effect3d = effect->is3d();
	bool localCoords = particleType.positionRelative();
	float_t lifespan = particleType.lifespan().at(emitterLife);
	float_t lifespanVariance = particleType.lifespanVariance().value();
	float_t velocity = math::linearInterpolation(particleType.initialVelocity().at(emitterLife), parentSpeed, particleType.inheritedVelocity().at(emitterLife));
	float_t velocityVariance = particleType.velocityVariance().value();
	float3_t initialRotation = particleType.initialRotation().at(emitterLife);
	float3_t rotationVariance = particleType.rotationVariance().value();
	float3_t angularVelocityVariance = particleType.angularVelocityVariance().value();
	float_t initialSize = particleType.initialSize().at(emitterLife);
	float_t sizeVariance = particleType.sizeVariance().value();
	float3_t size = particleType.size().at();
	float_t initialOpacity = particleType.initialOpacity().at(emitterLife);
	float4_t colorVariance = particleType.colorVariance().value();
	float_t opacityVariance = particleType.opacityVariance().value();
	float4_t color = float4_t(float3_t(particleType.color().at()), particleType.opacity().at());

	std::mt19937& rng = state.rng();

	for(std::uint32_t addIndex = 0; addIndex < count; addIndex++) {
		std::uint32_t p = particleCollectionCount - count + addIndex;

		particles.id[p] = state.particleIdCounter()++;
		particles.parentId[p] = parentId;
		particles.life[p] = 0.0;
		particles.lifespan[p] = std::max(lifespan + math::randomUniform(rng, -lifespanVariance, +lifespanVariance), 0.000001);

		float3_t particleSpawnPosition = float3_t(0.0);
		switch(particleEmitter.shape()) {
			case ParticleEmitter::Shape::line:
				particleSpawnPosition = emitOnSegment(
					emitterScale.x,
					particleEmitter.distribution(),
					particleEmitter.gridOrder(),
					particleEmitter.gridSizeX(),
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::ellipse:
				particleSpawnPosition = emitInEllipse(
					float2_t(emitterScale),
					particleEmitter.distribution(),
					particleEmitter.gridOrder(),
					particleEmitter.gridSizeX(), particleEmitter.gridSizeY(),
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::rectangle:
				particleSpawnPosition = emitInRectangle(
					float2_t(emitterScale),
					particleEmitter.distribution(),
					particleEmitter.gridOrder(),
					particleEmitter.gridSizeX(), particleEmitter.gridSizeY(),
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::path:
				particleSpawnPosition = emitOnPath(
					emitterScale,
					particleEmitter.path(),
					particleEmitter.distribution(),
					particleEmitter.gridOrder(),
					particleEmitter.gridSizeX(),
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::ellipsoid:
				particleSpawnPosition = emitInEllipsoid(
					emitterScale,
					particleEmitter.distribution(),
					particleEmitter.gridOrder(),
					particleEmitter.gridSizeX(), particleEmitter.gridSizeY(), particleEmitter.gridSizeZ(),
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::cuboid:
				particleSpawnPosition = emitInCuboid(
					emitterScale,
					particleEmitter.distribution(),
					particleEmitter.gridOrder(),
					particleEmitter.gridSizeX(), particleEmitter.gridSizeY(), particleEmitter.gridSizeZ(),
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::cylinder:
				particleSpawnPosition = emitInCylinder(
					emitterScale,
					particleEmitter.distribution(),
					particleEmitter.gridOrder(),
					particleEmitter.gridSizeX(), particleEmitter.gridSizeY(), particleEmitter.gridSizeZ(),
					emissionState.emitterGridIndex,
					rng);
				break;
			default:
				break;
		}

		if(effect3d) {
			matrix3_t directionMatrix = matrix3_t(math::yawPitchRollRotationMatrix(
				math::radians(emitterDirection.y + emitterSpread * math::randomUniform(rng, -0.5, +0.5)),
				math::radians(emitterDirection.z + emitterSpread * math::randomUniform(rng, -0.5, +0.5)),
				math::radians(emitterDirection.x + emitterSpread * math::randomUniform(rng, -0.5, +0.5))));

			particleSpawnPosition = emitterRotationMatrix * particleSpawnPosition;

			switch(particleEmitter.directionMode()) {
				case ParticleEmitter::DirectionMode::outwards:
					particles.velocity[p] = directionMatrix *
						((particleSpawnPosition != float3_t(0.0)) ? math::normalize(particleSpawnPosition) : worldUpVector3);
					break;
				case ParticleEmitter::DirectionMode::inwards:
					particles.velocity[p] = directionMatrix *
						((particleSpawnPosition != float3_t(0.0)) ? math::normalize(-particleSpawnPosition) : worldUpVector3);
					break;
				case ParticleEmitter::DirectionMode::inherit:
					particles.velocity[p] = directionMatrix * parentDirection;
					break;
				case ParticleEmitter::DirectionMode::inherit_inverse:
					particles.velocity[p] = directionMatrix * -parentDirection;
					break;
				default:
					particles.velocity[p] = emitterRotationMatrix * directionMatrix * worldUpVector3;
					break;
			}
		}
		else {
			float_t direction = math::radians(emitterDirection.x + emitterSpread * math::randomUniform(rng, -0.5, +0.5));

			particleSpawnPosition = float3_t(math::rotateVector(float2_t(particleSpawnPosition), emitterRotation.x), 0.0);

			switch(particleEmitter.directionMode()) {
				case ParticleEmitter::DirectionMode::outwards:
					particles.velocity[p] = float3_t(math::rotateVector(
						(particleSpawnPosition != float3_t(0.0)) ? float2_t(math::normalize(particleSpawnPosition)) : worldUpVector2,
						direction), 0.0);
					break;
				case ParticleEmitter::DirectionMode::inwards:
					particles.velocity[p] = float3_t(math::rotateVector(
						(particleSpawnPosition != float3_t(0.0)) ? float2_t(math::normalize(-particleSpawnPosition)) : worldUpVector2,
						direction), 0.0);
					break;
				case ParticleEmitter::DirectionMode::inherit:
					particles.velocity[p] = float3_t(math::rotateVector(float2_t(parentDirection), direction), 0.0);
					break;
				case ParticleEmitter::DirectionMode::inherit_inverse:
					particles.velocity[p] = float3_t(math::rotateVector(float2_t(-parentDirection), direction), 0.0);
					break;
				default:
					particles.velocity[p] = float3_t(math::rotateVector(worldUpVector2, emitterRotation.x + direction), 0.0);
					break;
			}
		}

		particles.velocity[p] *= velocity + math::randomUniform(rng, -velocityVariance, +velocityVariance);
		particles.force[p] = float3_t(0.0);
		particles.position[p] = emissionPosition + particleSpawnPosition;
		particles.globalPosition[p] = localCoords
			? particles.position[p] + emitterPosition
			: particles.position[p];

		particles.initialRotation[p] = initialRotation + float3_t(
			math::randomUniform(rng, -rotationVariance.x, +rotationVariance.x),
			math::randomUniform(rng, -rotationVariance.y, +rotationVariance.y),
			math::randomUniform(rng, -rotationVariance.z, +rotationVariance.z));
		particles.initialAngularVelocity[p] = float3_t(
			math::randomUniform(rng, -angularVelocityVariance.x, +angularVelocityVariance.x),
			math::randomUniform(rng, -angularVelocityVariance.y, +angularVelocityVariance.y),
			math::randomUniform(rng, -angularVelocityVariance.z, +angularVelocityVariance.z));
		particles.rotation[p] = particles.initialRotation[p];

		particles.initialSize[p] = initialSize + math::randomUniform(rng, -sizeVariance, +sizeVariance);
		particles.size[p] = size * particles.initialSize[p];

		particles.initialColor[p] = float4_t(
			math::randomUniform(rng, -colorVariance.x, +colorVariance.x),
			math::randomUniform(rng, -colorVariance.y, +colorVariance.y),
			math::randomUniform(rng, -colorVariance.z, +colorVariance.z),
			initialOpacity + math::randomUniform(rng, -opacityVariance, +opacityVariance));
		particles.color[p] = color;
	}

	return count;
}

float3_t DefaultParticleGenerator::emitOnSegment(float_t length,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSize, std::uint32_t& gridIndex,
	std::mt19937& rng) {
	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary: {
			return float3_t(0.0, math::randomUniform(rng, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::center: {
			return float3_t(0.0, math::randomTruncatedNormal(rng, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::hole: {
			return float3_t(0.0, math::randomTruncatedInverseNormal(rng, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::grid_random: {
			return float3_t(0.0, math::randomUniformGrid(rng, gridSize, -length, +length) * 0.5, 0.0);
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			float_t y = sampleGrid1d(gridIndex, gridSize, -length, +length) * 0.5;
			gridIndex = (gridIndex + 1) % gridSize;
			return float3_t(0.0, y, 0.0);
		}
		default: {
			return float3_t(0.0, 0.0, 0.0);
		}
	}
}

float3_t DefaultParticleGenerator::emitInEllipse(const float2_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t& gridIndex,
	std::mt19937& rng) {
	float_t r = 0.0;
	float_t phi = 0.0;
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			r = std::sqrt(math::randomUniform(rng, 0.0, 1.0));
			phi = math::randomUniform(rng, 0.0, math::twoPi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			do {
				r = std::sqrt(math::randomUniform(rng, 0.0, 1.0));
				phi = math::randomUniform(rng, 0.0, math::twoPi);
				point = float3_t(
					std::cos(phi),
					std::sin(phi),
					0.0) * r;
			}
			while(std::pow(math::randomUniform(rng, 0.0, 1.0), 2) < point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				r = std::sqrt(math::randomUniform(rng, 0.0, 1.0));
				phi = math::randomUniform(rng, 0.0, math::twoPi);
				point = float3_t(
					std::cos(phi),
					std::sin(phi),
					0.0) * r;
			}
			while(std::pow(math::randomUniform(rng, 0.0, 1.0), 2) > point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			phi = math::randomUniform(rng, 0.0, math::twoPi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			r = math::randomUniformGrid(rng, gridSizeX, 0.0, 1.0);
			phi = math::randomUniformGrid(rng, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
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
					r = sampleGrid1d(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2d(gridIndex, gridSizeX, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					break;
				case ParticleEmitter::GridOrder::y_x_z:
				case ParticleEmitter::GridOrder::y_z_x:
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid2d(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1d(gridIndex, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
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

float3_t DefaultParticleGenerator::emitInRectangle(const float2_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t& gridIndex,
	std::mt19937& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = float3_t(
				math::randomUniform(rng, -size.x, +size.x),
				math::randomUniform(rng, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = float3_t(
				math::randomTruncatedNormal(rng, -size.x, +size.x),
				math::randomTruncatedNormal(rng, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				point = float3_t(
					math::randomUniform(rng, -1.0, +1.0),
					math::randomUniform(rng, -1.0, +1.0),
					0.0);
			}
			while(math::randomUniform(rng, 0.0, 1.0) > (point.x * point.x + point.y * point.y) * 0.5);

			point.x *= size.x * 0.5;
			point.y *= size.y * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t r = math::randomUniform(rng, 0.0, (size.x + size.y) * 2.0);
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
				math::randomUniformGrid(rng, gridSizeX, -size.x, +size.x),
				math::randomUniformGrid(rng, gridSizeY, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
				case ParticleEmitter::GridOrder::x_z_y:
				case ParticleEmitter::GridOrder::z_x_y:
					point = float3_t(
						sampleGrid1d(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid2d(gridIndex, gridSizeX, gridSizeY, -size.y, +size.y),
						0.0) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_x_z:
				case ParticleEmitter::GridOrder::y_z_x:
				case ParticleEmitter::GridOrder::z_y_x:
					point = float3_t(
						sampleGrid2d(gridIndex, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid1d(gridIndex, gridSizeY, -size.y, +size.y),
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

float3_t DefaultParticleGenerator::emitOnPath(const float3_t& size,
	const Curve<float3_t>& path,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSize, std::uint32_t& gridIndex,
	std::mt19937& rng) {
	float_t x = 0.0;

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary: {
			x = math::randomUniform(rng, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::center: {
			x = math::randomTruncatedNormal(rng, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::hole: {
			x = math::randomTruncatedInverseNormal(rng, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			x = math::randomUniformGrid(rng, gridSize, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			x = sampleGrid1d(gridIndex, gridSize, 0.0, 1.0);
			gridIndex = (gridIndex + 1) % gridSize;
			break;
		}
		default: {
			break;
		}
	}

	return path.at(x) * size;
}

float3_t DefaultParticleGenerator::emitInEllipsoid(const float3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex,
	std::mt19937& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			float_t r = std::sqrt(math::randomUniform(rng, 0.0, 1.0));
			float_t phi = math::randomUniform(rng, 0.0, math::twoPi);
			float_t ctheta = math::randomUniform(rng, -1.0, +1.0);
			float_t theta = std::acos(ctheta);
			float_t stheta = std::sin(theta);

			point = float3_t(
				stheta * std::cos(phi),
				stheta * std::sin(phi),
				ctheta) * r;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			do {
				float_t r = std::sqrt(math::randomUniform(rng, 0.0, 1.0));
				float_t phi = math::randomUniform(rng, 0.0, math::twoPi);
				float_t ctheta = math::randomUniform(rng, -1.0, +1.0);
				float_t theta = std::acos(ctheta);
				float_t stheta = std::sin(theta);

				point = float3_t(
					stheta * std::cos(phi),
					stheta * std::sin(phi),
					ctheta) * r;
			}
			while(std::pow(math::randomUniform(rng, 0.0, 1.0), 3) < point.x * point.x + point.y * point.y + point.z * point.z);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				float_t r = std::sqrt(math::randomUniform(rng, 0.0, 1.0));
				float_t phi = math::randomUniform(rng, 0.0, math::twoPi);
				float_t ctheta = math::randomUniform(rng, -1.0, +1.0);
				float_t theta = std::acos(ctheta);
				float_t stheta = std::sin(theta);

				point = float3_t(
					stheta * std::cos(phi),
					stheta * std::sin(phi),
					ctheta) * r;
			}
			while(std::pow(math::randomUniform(rng, 0.0, 1.0), 3) > point.x * point.x + point.y * point.y + point.z * point.z);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t phi = math::randomUniform(rng, 0.0, math::twoPi);
			float_t ctheta = math::randomUniform(rng, -1.0, +1.0);
			float_t theta = std::acos(ctheta);
			float_t stheta = std::sin(theta);

			point = float3_t(
				stheta * std::cos(phi),
				stheta * std::sin(phi),
				ctheta);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			float_t r = math::randomUniformGrid(rng, gridSizeX, 0.0, 1.0);
			float_t phi = math::randomUniformGrid(rng, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			float_t theta = math::randomUniformGrid(rng, gridSizeZ, 0.0, math::pi);
			float_t stheta = std::sin(theta);
			float_t ctheta = std::cos(theta);

			point = float3_t(
				stheta * std::cos(phi),
				stheta * std::sin(phi),
				ctheta) * r;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			float_t r = 0.0;
			float_t phi = 0.0;
			float_t theta = 0.0;
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					r = sampleGrid1d(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2d(gridIndex, gridSizeX, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid3d(gridIndex, gridSizeX, gridSizeY, gridSizeZ, 0.0, math::pi);
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					r = sampleGrid1d(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3d(gridIndex, gridSizeX, gridSizeZ, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid2d(gridIndex, gridSizeX, gridSizeZ, 0.0, math::pi);
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					r = sampleGrid2d(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1d(gridIndex, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid3d(gridIndex, gridSizeY, gridSizeX, gridSizeZ, 0.0, math::pi);
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					r = sampleGrid3d(gridIndex, gridSizeY, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1d(gridIndex, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid2d(gridIndex, gridSizeY, gridSizeZ, 0.0, math::pi);
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					r = sampleGrid2d(gridIndex, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3d(gridIndex, gridSizeZ, gridSizeX, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid1d(gridIndex, gridSizeZ, 0.0, math::pi);
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid3d(gridIndex, gridSizeZ, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2d(gridIndex, gridSizeZ, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					theta = sampleGrid1d(gridIndex, gridSizeZ, 0.0, math::pi);
					break;
				default:
					break;
			}

			float_t stheta = std::sin(theta);
			float_t ctheta = std::cos(theta);

			gridIndex = (gridIndex + 1) % (gridSizeX * gridSizeY * gridSizeZ);
			point = float3_t(
				stheta * std::cos(phi),
				stheta * std::sin(phi),
				ctheta) * r;

			break;
		}
		default: {
			break;
		}
	}

	point *= size * 0.5;

	return point;
}

float3_t DefaultParticleGenerator::emitInCuboid(const float3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex,
	std::mt19937& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = float3_t(
				math::randomUniform(rng, -size.x, +size.x),
				math::randomUniform(rng, -size.y, +size.y),
				math::randomUniform(rng, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = float3_t(
				math::randomTruncatedNormal(rng, -size.x, +size.x),
				math::randomTruncatedNormal(rng, -size.y, +size.y),
				math::randomTruncatedNormal(rng, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				point = float3_t(
					math::randomUniform(rng, -1.0, +1.0),
					math::randomUniform(rng, -1.0, +1.0),
					math::randomUniform(rng, -1.0, +1.0));
			}
			while(math::randomUniform(rng, 0.0, 1.0) > (point.x * point.x + point.y * point.y + point.z * point.z) * 0.5);

			point *= size * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			std::int32_t side = math::randomDiscreteUniform(rng, 0, 5);
			switch(side) {
				case 0:
					point = float3_t(-size.x, math::randomUniform(rng, -size.y, +size.y), math::randomUniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 1:
					point = float3_t(+size.x, math::randomUniform(rng, -size.y, +size.y), math::randomUniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 2:
					point = float3_t(math::randomUniform(rng, -size.x, +size.x), -size.y, math::randomUniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 3:
					point = float3_t(math::randomUniform(rng, -size.x, +size.x), +size.y, math::randomUniform(rng, -size.z, +size.z)) * 0.5;
					break;
				case 4:
					point = float3_t(math::randomUniform(rng, -size.x, +size.x), math::randomUniform(rng, -size.y, +size.y), -size.z) * 0.5;
					break;
				case 5:
					point = float3_t(math::randomUniform(rng, -size.x, +size.x), math::randomUniform(rng, -size.y, +size.y), +size.z) * 0.5;
					break;
				default:
					point = float3_t(0.0);
					break;
			}

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			point = float3_t(
				math::randomUniformGrid(rng, gridSizeX, -size.x, +size.x),
				math::randomUniformGrid(rng, gridSizeY, -size.y, +size.y),
				math::randomUniformGrid(rng, gridSizeZ, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					point = float3_t(
						sampleGrid1d(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid2d(gridIndex, gridSizeX, gridSizeY, -size.y, +size.y),
						sampleGrid3d(gridIndex, gridSizeX, gridSizeY, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					point = float3_t(
						sampleGrid1d(gridIndex, gridSizeX, -size.x, +size.x),
						sampleGrid3d(gridIndex, gridSizeX, gridSizeZ, gridSizeY, -size.y, +size.y),
						sampleGrid2d(gridIndex, gridSizeX, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					point = float3_t(
						sampleGrid2d(gridIndex, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid1d(gridIndex, gridSizeY, -size.y, +size.y),
						sampleGrid3d(gridIndex, gridSizeY, gridSizeX, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					point = float3_t(
						sampleGrid3d(gridIndex, gridSizeY, gridSizeZ, gridSizeX, -size.x, +size.x),
						sampleGrid1d(gridIndex, gridSizeY, -size.y, +size.y),
						sampleGrid2d(gridIndex, gridSizeY, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					point = float3_t(
						sampleGrid2d(gridIndex, gridSizeZ, gridSizeX, -size.x, +size.y),
						sampleGrid3d(gridIndex, gridSizeZ, gridSizeX, gridSizeY, -size.y, +size.y),
						sampleGrid1d(gridIndex, gridSizeZ, -size.z, +size.z)) * 0.5;
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					point = float3_t(
						sampleGrid3d(gridIndex, gridSizeZ, gridSizeY, gridSizeX, -size.x, +size.x),
						sampleGrid2d(gridIndex, gridSizeZ, gridSizeY, -size.y, +size.y),
						sampleGrid1d(gridIndex, gridSizeZ, -size.z, +size.z)) * 0.5;
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

float3_t DefaultParticleGenerator::emitInCylinder(const float3_t& size,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSizeX, std::uint32_t gridSizeY, std::uint32_t gridSizeZ, std::uint32_t& gridIndex,
	std::mt19937& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			float_t h = math::randomUniform(rng, -1.0, +1.0);
			float_t phi = math::randomUniform(rng, 0.0, math::twoPi);
			float_t r = std::sqrt(math::randomUniform(rng, 0.0, 1.0));

			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::center: {
			float_t h = math::randomUniform(rng, -1.0, +1.0);
			do {
				float_t r = std::sqrt(math::randomUniform(rng, 0.0, 1.0));
				float_t phi = math::randomUniform(rng, 0.0, math::twoPi);
				point = float3_t(
					std::cos(phi) * r,
					std::sin(phi) * r,
					h);
			}
			while(std::pow(math::randomUniform(rng, 0.0, 1.0), 2) < point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			float_t h = math::randomUniform(rng, -1.0, +1.0);
			do {
				float_t r = std::sqrt(math::randomUniform(rng, 0.0, 1.0));
				float_t phi = math::randomUniform(rng, 0.0, math::twoPi);
				point = float3_t(
					std::cos(phi) * r,
					std::sin(phi) * r,
					h);
			}
			while(std::pow(math::randomUniform(rng, 0.0, 1.0), 2) > point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t h = 0.0;
			float_t r = 0.0;
			float_t baseArea = size.x * size.y * math::pi;
			float_t lateralArea = math::pi * (size.x + size.y) * size.z;
			float_t side = math::randomUniform(rng, 0.0, baseArea * 2.0 + lateralArea);
			if(side < baseArea) {
				h = -1.0;
				r = std::sqrt(math::randomUniform(rng, 0.0, 1.0));
			}
			else if(side < baseArea * 2.0) {
				h = +1.0;
				r = std::sqrt(math::randomUniform(rng, 0.0, 1.0));
			}
			else {
				h = math::randomUniform(rng, -1.0, +1.0);
				r = 1.0;
			}

			float_t phi = math::randomUniform(rng, 0.0, math::twoPi);
			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			float_t r = math::randomUniformGrid(rng, gridSizeX, 0.0, 1.0);
			float_t phi = math::randomUniformGrid(rng, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			float_t h = math::randomUniformGrid(rng, gridSizeZ, -1.0, +1.0);
			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			float_t r = 0.0;
			float_t phi = 0.0;
			float_t h = 0.0;
			switch(gridOrder) {
				case ParticleEmitter::GridOrder::x_y_z:
					r = sampleGrid1d(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2d(gridIndex, gridSizeX, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid3d(gridIndex, gridSizeX, gridSizeY, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::x_z_y:
					r = sampleGrid1d(gridIndex, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3d(gridIndex, gridSizeX, gridSizeZ, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid2d(gridIndex, gridSizeX, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::y_x_z:
					r = sampleGrid2d(gridIndex, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1d(gridIndex, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid3d(gridIndex, gridSizeY, gridSizeX, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::y_z_x:
					r = sampleGrid3d(gridIndex, gridSizeY, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid1d(gridIndex, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid2d(gridIndex, gridSizeY, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::z_x_y:
					r = sampleGrid2d(gridIndex, gridSizeZ, gridSizeX, 0.0, 1.0);
					phi = sampleGrid3d(gridIndex, gridSizeZ, gridSizeX, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid1d(gridIndex, gridSizeZ, -1.0, +1.0);
					break;
				case ParticleEmitter::GridOrder::z_y_x:
					r = sampleGrid3d(gridIndex, gridSizeZ, gridSizeY, gridSizeX, 0.0, 1.0);
					phi = sampleGrid2d(gridIndex, gridSizeZ, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
					h = sampleGrid1d(gridIndex, gridSizeZ, -1.0, +1.0);
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

float_t DefaultParticleGenerator::sampleGrid1d(std::uint32_t gridIndex, std::uint32_t gridSize, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex % gridSize) / static_cast<float_t>(gridSize - 1) * (max - min) + min;
}
float_t DefaultParticleGenerator::sampleGrid2d(std::uint32_t gridIndex, std::uint32_t gridSize1, std::uint32_t gridSize2, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex / gridSize1 % gridSize2) / static_cast<float_t>(gridSize2 - 1) * (max - min) + min;
}
float_t DefaultParticleGenerator::sampleGrid3d(std::uint32_t gridIndex, std::uint32_t gridSize1, std::uint32_t gridSize2, std::uint32_t gridSize3, float_t min, float_t max) {
	return static_cast<float_t>(gridIndex / gridSize1 / gridSize2 % gridSize3) / static_cast<float_t>(gridSize3 - 1) * (max - min) + min;
}
}
