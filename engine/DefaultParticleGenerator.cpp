#include "DefaultParticleGenerator.h"
#include "../effect/Coordinates.h"
#include "../effect/Transform.h"
#include "../math/Common.h"
#include "../math/Constants.h"
#include "../math/Geometry.h"
#include "../math/Trigonometry.h"
#include "../math/Transformation.h"
#include "../math/Interpolation.h"
#include <cmath>
#include <optional>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>

namespace pixelpart {
void DefaultParticleGenerator::generate(EffectRuntimeState& state,
	const Effect* effect, EffectRuntimeContext runtimeContext) const {
	float_t t = runtimeContext.time();
	float_t dt = runtimeContext.deltaTime();

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

		ParticleLodStrategy lodStrategy = particleType.lodStrategy(runtimeContext.lod());
		ParticleEmissionState& emissionState = state.particleEmissionStates().at(emissionPair);

		float_t startTime = particleEmitter.startTrigger()
			? particleEmitter.start() + runtimeContext.triggerActivationTime(particleEmitter.startTrigger())
			: particleEmitter.start();

		float_t emissionTime = particleEmitter.repeat()
			? std::fmod(t - startTime, particleEmitter.duration())
			: t - startTime;

		switch(particleEmitter.emissionMode()) {
			case ParticleEmitter::EmissionMode::continuous:
				emissionState.emissionCount +=
					particleType.count().at(emissionTime / particleEmitter.duration()) * dt * lodStrategy.emissionFactor();
				break;
			case ParticleEmitter::EmissionMode::burst_start:
				if(emissionTime < dt) {
					emissionState.emissionCount += particleType.count().at(0) * lodStrategy.emissionFactor();
				}
				break;
			case ParticleEmitter::EmissionMode::burst_end:
				if(emissionTime > particleEmitter.duration() - dt) {
					emissionState.emissionCount += particleType.count().at(1) * lodStrategy.emissionFactor();
				}
				break;
			default:
				break;
		}

		std::uint32_t emittedParticleCount = static_cast<std::uint32_t>(std::max(emissionState.emissionCount, 0.0));
		if(emittedParticleCount > 0) {
			emissionState.emissionCount -= static_cast<float_t>(initializeParticles(emittedParticleCount, lodStrategy.lifetimeFactor(),
				state, emissionState,
				particleCollection, nullptr, id_t::nullValue,
				effect, emissionPair, runtimeContext, true));
		}
	}

	std::unordered_map<ParticleEmissionPair, std::vector<ParticleEmissionPair>> childEmissionPairCollection;

	for(const auto& [emissionPair, particleCollection] : state.particleCollections()) {
		std::vector<ParticleEmissionPair>& childEmissionPairs = childEmissionPairCollection[emissionPair];
		const ParticleType& particleType = effect->particleTypes().at(emissionPair.typeId);

		for(const auto& [otherEmissionPair, otherParticleCollection] : state.particleCollections()) {
			if(emissionPair == otherEmissionPair) {
				continue;
			}

			const ParticleEmitter& otherParticleEmitter = effect->sceneGraph().at<ParticleEmitter>(otherEmissionPair.emitterId);
			const ParticleType& otherParticleType = effect->particleTypes().at(otherEmissionPair.typeId);
			if(otherParticleType.parentId() != particleType.id() ||
				otherParticleEmitter.primary() ||
				!otherParticleEmitter.activatedByTrigger(runtimeContext)) {
				continue;
			}

			childEmissionPairs.emplace_back(otherEmissionPair);
		}
	}

	for(const auto& [emissionPair, particleCollection] : state.particleCollections()) {
		const std::vector<ParticleEmissionPair>& childEmissionPairs = childEmissionPairCollection.at(emissionPair);
		if(childEmissionPairs.empty()) {
			continue;
		}

		const ParticleEmitter& particleEmitter = effect->sceneGraph().at<ParticleEmitter>(emissionPair.emitterId);
		ParticleCollection::ReadPtr particles = particleCollection.readPtr();

		float_t parentEmitterDuration = particleEmitter.duration();

		for(ParticleEmissionPair childEmissionPair : childEmissionPairs) {
			ParticleCollection& childParticleCollection = state.particleCollections().at(childEmissionPair);
			ParticleEmissionState& childEmissionState = state.particleEmissionStates().at(childEmissionPair);
			const ParticleType& childParticleType = effect->particleTypes().at(childEmissionPair.typeId);
			const ParticleEmitter& childParticleEmitter = effect->sceneGraph().at<ParticleEmitter>(childEmissionPair.emitterId);

			float_t childEmitterDuration = childParticleEmitter.duration();
			bool childEmitterRepeat = childParticleEmitter.repeat();
			ParticleEmitter::EmissionMode childEmissionMode = childParticleEmitter.emissionMode();

			const Curve<float_t>& childParticleTypeCountCurve = childParticleType.count().resultCurve();

			float_t lodEmissionFactor = childParticleType.lodStrategy(runtimeContext.lod()).emissionFactor();
			float_t lodLifetimeFactor = childParticleType.lodStrategy(runtimeContext.lod()).lifetimeFactor();

			for(std::uint32_t p = 0; p < particleCollection.count(); p++) {
				float_t particleTime = particles.life[p] * particles.lifespan[p];
				if(particleTime > childEmitterDuration && !childEmitterRepeat) {
					continue;
				}

				switch(childEmissionMode) {
					case ParticleEmitter::EmissionMode::continuous: {
						float_t emissionTime = childEmitterRepeat
							? std::fmod(particleTime, childEmitterDuration)
							: particleTime;

						childEmissionState.emissionCount +=
							childParticleTypeCountCurve.at(emissionTime / parentEmitterDuration) * dt * lodEmissionFactor;

						break;
					}
					case ParticleEmitter::EmissionMode::burst_start: {
						if(particleTime < dt) {
							childEmissionState.emissionCount += childParticleTypeCountCurve.at(0) * lodEmissionFactor;
						}

						break;
					}
					case ParticleEmitter::EmissionMode::burst_end: {
						if(particles.life[p] > 1.0) {
							childEmissionState.emissionCount += childParticleTypeCountCurve.at(1) * lodEmissionFactor;
						}

						break;
					}
					default: {
						break;
					}
				}

				std::uint32_t emittedParticleCount = static_cast<std::uint32_t>(std::max(childEmissionState.emissionCount, 0.0));
				if(emittedParticleCount > 0) {
					EffectRuntimeContext particleRuntimeContext(particleTime);

					childEmissionState.emissionCount -= static_cast<float_t>(initializeParticles(emittedParticleCount, lodLifetimeFactor,
						state, childEmissionState,
						childParticleCollection, &particleCollection, p,
						effect, childEmissionPair, particleRuntimeContext, false));
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

	initializeParticles(count, 1.0,
		state, *particleEmissionState,
		*particleCollection, nullptr, id_t::nullValue,
		effect, emissionPair, runtimeContext, false);
}

std::uint32_t DefaultParticleGenerator::initializeParticles(std::uint32_t count, float_t lifetimeFactor,
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
		EffectRuntimeContext prevRuntimeContext(runtimeContext.time() - 0.1);
		prevRuntimeContext.triggerActivationTimes() = runtimeContext.triggerActivationTimes();

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

	pcg32& rng = state.rng();

	for(std::uint32_t addIndex = 0; addIndex < count; addIndex++) {
		std::uint32_t p = particleCollectionCount - count + addIndex;

		particles.id[p] = state.particleIdCounter()++;
		particles.parentId[p] = parentId;
		particles.life[p] = 0.0;
		particles.lifespan[p] = std::max((lifespan + rng.next(-lifespanVariance, +lifespanVariance)) * lifetimeFactor, 0.000001);

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
				math::radians(emitterDirection.y + emitterSpread * rng.next(-0.5, +0.5)),
				math::radians(emitterDirection.z + emitterSpread * rng.next(-0.5, +0.5)),
				math::radians(emitterDirection.x + emitterSpread * rng.next(-0.5, +0.5))));

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
			float_t direction = math::radians(emitterDirection.x + emitterSpread * rng.next(-0.5, +0.5));

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

		particles.velocity[p] *= velocity + rng.next(-velocityVariance, +velocityVariance);
		particles.force[p] = float3_t(0.0);
		particles.position[p] = emissionPosition + particleSpawnPosition;
		particles.globalPosition[p] = localCoords
			? particles.position[p] + emitterPosition
			: particles.position[p];

		particles.initialRotation[p] = initialRotation + float3_t(
			rng.next(-rotationVariance.x, +rotationVariance.x),
			rng.next(-rotationVariance.y, +rotationVariance.y),
			rng.next(-rotationVariance.z, +rotationVariance.z));
		particles.initialAngularVelocity[p] = float3_t(
			rng.next(-angularVelocityVariance.x, +angularVelocityVariance.x),
			rng.next(-angularVelocityVariance.y, +angularVelocityVariance.y),
			rng.next(-angularVelocityVariance.z, +angularVelocityVariance.z));
		particles.rotation[p] = particles.initialRotation[p];

		particles.initialSize[p] = initialSize + rng.next(-sizeVariance, +sizeVariance);
		particles.size[p] = size * particles.initialSize[p];

		particles.initialColor[p] = float4_t(
			rng.next(-colorVariance.x, +colorVariance.x),
			rng.next(-colorVariance.y, +colorVariance.y),
			rng.next(-colorVariance.z, +colorVariance.z),
			initialOpacity + rng.next(-opacityVariance, +opacityVariance));
		particles.color[p] = color;
	}

	return count;
}

float3_t DefaultParticleGenerator::emitOnSegment(float_t length,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSize, std::uint32_t& gridIndex,
	pcg32& rng) {
	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary: {
			return float3_t(rng.next(-length, +length) * 0.5, 0.0, 0.0);
		}
		case ParticleEmitter::Distribution::center: {
			return float3_t(randomCentered(rng, -length, +length) * 0.5, 0.0, 0.0);
		}
		case ParticleEmitter::Distribution::hole: {
			return float3_t(randomInverseCentered(rng, -length, +length) * 0.5, 0.0, 0.0);
		}
		case ParticleEmitter::Distribution::grid_random: {
			return float3_t(randomUniformGrid(rng, gridSize, -length, +length) * 0.5, 0.0, 0.0);
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			float_t x = sampleGrid1d(gridIndex, gridSize, -length, +length) * 0.5;
			gridIndex = (gridIndex + 1) % gridSize;
			return float3_t(x, 0.0, 0.0);
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
	pcg32& rng) {
	float_t r = 0.0;
	float_t phi = 0.0;
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			r = std::sqrt(rng.next());
			phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			do {
				r = std::sqrt(rng.next());
				phi = rng.next(0.0, math::twoPi);
				point = float3_t(
					std::cos(phi),
					std::sin(phi),
					0.0) * r;
			}
			while(std::pow(rng.next(), 2) < point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				r = std::sqrt(rng.next());
				phi = rng.next(0.0, math::twoPi);
				point = float3_t(
					std::cos(phi),
					std::sin(phi),
					0.0) * r;
			}
			while(std::pow(rng.next(), 2) > point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			r = randomUniformGrid(rng, gridSizeX, 0.0, 1.0);
			phi = randomUniformGrid(rng, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
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
	pcg32& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = float3_t(
				rng.next(-size.x, +size.x),
				rng.next(-size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = float3_t(
				randomCentered(rng, -size.x, +size.x),
				randomCentered(rng, -size.y, +size.y),
				0.0) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				point = float3_t(
					rng.next(-1.0, +1.0),
					rng.next(-1.0, +1.0),
					0.0);
			}
			while(rng.next() > (point.x * point.x + point.y * point.y) * 0.5);

			point.x *= size.x * 0.5;
			point.y *= size.y * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t r = rng.next(0.0, (size.x + size.y) * 2.0);
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
				randomUniformGrid(rng, gridSizeX, -size.x, +size.x),
				randomUniformGrid(rng, gridSizeY, -size.y, +size.y),
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
	pcg32& rng) {
	float_t x = 0.0;

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary: {
			x = rng.next();
			break;
		}
		case ParticleEmitter::Distribution::center: {
			x = randomCentered(rng, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::hole: {
			x = randomInverseCentered(rng, 0.0, 1.0);
			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			x = randomUniformGrid(rng, gridSize, 0.0, 1.0);
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
	pcg32& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			float_t r = std::sqrt(rng.next());
			float_t phi = rng.next(0.0, math::twoPi);
			float_t ctheta = rng.next(-1.0, +1.0);
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
				float_t r = std::sqrt(rng.next());
				float_t phi = rng.next(0.0, math::twoPi);
				float_t ctheta = rng.next(-1.0, +1.0);
				float_t theta = std::acos(ctheta);
				float_t stheta = std::sin(theta);

				point = float3_t(
					stheta * std::cos(phi),
					stheta * std::sin(phi),
					ctheta) * r;
			}
			while(std::pow(rng.next(), 3) < point.x * point.x + point.y * point.y + point.z * point.z);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				float_t r = std::sqrt(rng.next());
				float_t phi = rng.next(0.0, math::twoPi);
				float_t ctheta = rng.next(-1.0, +1.0);
				float_t theta = std::acos(ctheta);
				float_t stheta = std::sin(theta);

				point = float3_t(
					stheta * std::cos(phi),
					stheta * std::sin(phi),
					ctheta) * r;
			}
			while(std::pow(rng.next(), 3) > point.x * point.x + point.y * point.y + point.z * point.z);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t phi = rng.next(0.0, math::twoPi);
			float_t ctheta = rng.next(-1.0, +1.0);
			float_t theta = std::acos(ctheta);
			float_t stheta = std::sin(theta);

			point = float3_t(
				stheta * std::cos(phi),
				stheta * std::sin(phi),
				ctheta);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			float_t r = randomUniformGrid(rng, gridSizeX, 0.0, 1.0);
			float_t phi = randomUniformGrid(rng, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			float_t theta = randomUniformGrid(rng, gridSizeZ, 0.0, math::pi);
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
	pcg32& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			point = float3_t(
				rng.next(-size.x, +size.x),
				rng.next(-size.y, +size.y),
				rng.next(-size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::center: {
			point = float3_t(
				randomCentered(rng, -size.x, +size.x),
				randomCentered(rng, -size.y, +size.y),
				randomCentered(rng, -size.z, +size.z)) * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			do {
				point = float3_t(
					rng.next(-1.0, +1.0),
					rng.next(-1.0, +1.0),
					rng.next(-1.0, +1.0));
			}
			while(rng.next() > (point.x * point.x + point.y * point.y + point.z * point.z) * 0.5);

			point *= size * 0.5;

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			std::int32_t side = std::uniform_int_distribution<std::int32_t>(0, 5)(rng);
			switch(side) {
				case 0:
					point = float3_t(-size.x, rng.next(-size.y, +size.y), rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 1:
					point = float3_t(+size.x, rng.next(-size.y, +size.y), rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 2:
					point = float3_t(rng.next(-size.x, +size.x), -size.y, rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 3:
					point = float3_t(rng.next(-size.x, +size.x), +size.y, rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 4:
					point = float3_t(rng.next(-size.x, +size.x), rng.next(-size.y, +size.y), -size.z) * 0.5;
					break;
				case 5:
					point = float3_t(rng.next(-size.x, +size.x), rng.next(-size.y, +size.y), +size.z) * 0.5;
					break;
				default:
					point = float3_t(0.0);
					break;
			}

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			point = float3_t(
				randomUniformGrid(rng, gridSizeX, -size.x, +size.x),
				randomUniformGrid(rng, gridSizeY, -size.y, +size.y),
				randomUniformGrid(rng, gridSizeZ, -size.z, +size.z)) * 0.5;

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
	pcg32& rng) {
	float3_t point = float3_t(0.0);

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform: {
			float_t h = rng.next(-1.0, +1.0);
			float_t phi = rng.next(0.0, math::twoPi);
			float_t r = std::sqrt(rng.next());

			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::center: {
			float_t h = rng.next(-1.0, +1.0);
			do {
				float_t r = std::sqrt(rng.next());
				float_t phi = rng.next(0.0, math::twoPi);
				point = float3_t(
					std::cos(phi) * r,
					std::sin(phi) * r,
					h);
			}
			while(std::pow(rng.next(), 2) < point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			float_t h = rng.next(-1.0, +1.0);
			do {
				float_t r = std::sqrt(rng.next());
				float_t phi = rng.next(0.0, math::twoPi);
				point = float3_t(
					std::cos(phi) * r,
					std::sin(phi) * r,
					h);
			}
			while(std::pow(rng.next(), 2) > point.x * point.x + point.y * point.y);

			break;
		}
		case ParticleEmitter::Distribution::boundary: {
			float_t h = 0.0;
			float_t r = 0.0;
			float_t baseArea = size.x * size.y * math::pi;
			float_t lateralArea = math::pi * (size.x + size.y) * size.z;
			float_t side = rng.next(0.0, baseArea * 2.0 + lateralArea);
			if(side < baseArea) {
				h = -1.0;
				r = std::sqrt(rng.next());
			}
			else if(side < baseArea * 2.0) {
				h = +1.0;
				r = std::sqrt(rng.next());
			}
			else {
				h = rng.next(-1.0, +1.0);
				r = 1.0;
			}

			float_t phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			float_t r = randomUniformGrid(rng, gridSizeX, 0.0, 1.0);
			float_t phi = randomUniformGrid(rng, gridSizeY, 0.0, math::twoPi * (1.0 - 1.0 / static_cast<float_t>(gridSizeY)));
			float_t h = randomUniformGrid(rng, gridSizeZ, -1.0, +1.0);
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

float_t DefaultParticleGenerator::randomCentered(pcg32& rng, float_t min, float_t max) {
	float_t u1 = rng.next();
	float_t u2 = rng.next();
	float_t u3 = rng.next();

	return (u1 + u2 + u3) / 3.0 * (max - min) + min;
}
float_t DefaultParticleGenerator::randomInverseCentered(pcg32& rng, float_t min, float_t max) {
	float_t u1 = rng.next();
	float_t u2 = rng.next();
	float_t u3 = rng.next();

	float_t x = (u1 + u2 + u3) / 3.0;

	return (0.5 + std::copysign(0.5 - std::abs(x - 0.5), x - 0.5)) * (max - min) + min;
}
float_t DefaultParticleGenerator::randomUniformGrid(pcg32& rng, std::uint32_t size, float_t min, float_t max) {
	std::uniform_int_distribution<std::uint32_t> distrib(0, size - 1);

	return static_cast<float_t>(distrib(rng)) / static_cast<float_t>(size - 1) * (max - min) + min;
}
}
