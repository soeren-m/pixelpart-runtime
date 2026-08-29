#include "DefaultParticleGenerator.h"
#include "../effect/Coordinates.h"
#include "../effect/Curve.h"
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

		if(!effect->sceneGraph().contains<ParticleEmitter>(emissionPair.emitterId)) {
			continue;
		}

		const ParticleEmitter& particleEmitter = effect->sceneGraph().at<ParticleEmitter>(emissionPair.emitterId);
		if(!particleEmitter.primary() || !particleEmitter.active(runtimeContext)) {
			continue;
		}

		ParticleLodStrategy lodStrategy = particleType.lodStrategy(runtimeContext.lod());
		ParticleEmissionState& emissionState = state.particleEmissionStates().at(emissionPair);

		float_t startTime = particleEmitter.startTrigger()
			? particleEmitter.lifetimeStart() + runtimeContext.triggerActivationTime(particleEmitter.startTrigger())
			: particleEmitter.lifetimeStart();

		float_t emissionTime = particleEmitter.repeat()
			? std::fmod(t - startTime, particleEmitter.lifetimeDuration())
			: t - startTime;

		switch(particleEmitter.emissionMode()) {
			case ParticleEmitter::EmissionMode::continuous:
				emissionState.emissionCount +=
					particleType.count().at(emissionTime / particleEmitter.lifetimeDuration()) * dt * lodStrategy.emissionFactor();
				break;
			case ParticleEmitter::EmissionMode::burst_start:
				if(emissionTime < dt) {
					emissionState.emissionCount += particleType.count().at(0) * lodStrategy.emissionFactor();
				}
				break;
			case ParticleEmitter::EmissionMode::burst_end:
				if(emissionTime > particleEmitter.lifetimeDuration() - dt) {
					emissionState.emissionCount += particleType.count().at(1) * lodStrategy.emissionFactor();
				}
				break;
			default:
				break;
		}

		std::uint32_t emittedParticleCount = static_cast<std::uint32_t>(std::max(emissionState.emissionCount, 0.0));
		if(emittedParticleCount > 0) {
			ParticleEmitterEmissionData emitterEmissionData(effect, emissionPair.emitterId, runtimeContext, true);
			ParticleTypeEmissionData ptypeEmissionData(effect, emissionPair, runtimeContext, true);

			emissionState.emissionCount -= static_cast<float_t>(initializeParticles(emittedParticleCount, lodStrategy.lifetimeFactor(),
				state, emissionState,
				particleCollection, nullptr, id_t::nullValue,
				effect, emissionPair, runtimeContext,
				emitterEmissionData, ptypeEmissionData));
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

		float_t parentEmitterDuration = particleEmitter.lifetimeDuration();

		for(ParticleEmissionPair childEmissionPair : childEmissionPairs) {
			ParticleCollection& childParticleCollection = state.particleCollections().at(childEmissionPair);
			ParticleEmissionState& childEmissionState = state.particleEmissionStates().at(childEmissionPair);
			const ParticleType& childParticleType = effect->particleTypes().at(childEmissionPair.typeId);
			const ParticleEmitter& childParticleEmitter = effect->sceneGraph().at<ParticleEmitter>(childEmissionPair.emitterId);

			float_t childEmitterDuration = childParticleEmitter.lifetimeDuration();
			bool childEmitterRepeat = childParticleEmitter.repeat();
			ParticleEmitter::EmissionMode childEmissionMode = childParticleEmitter.emissionMode();

			const Curve<float_t>& childParticleTypeCountCurve = childParticleType.count().resultCurve();

			float_t lodEmissionFactor = childParticleType.lodStrategy(runtimeContext.lod()).emissionFactor();
			float_t lodLifetimeFactor = childParticleType.lodStrategy(runtimeContext.lod()).lifetimeFactor();

			ParticleEmitterEmissionData childEmitterEmissionData(effect, childEmissionPair.emitterId, runtimeContext, false);
			ParticleTypeEmissionData childPTypeEmissionData(effect, childEmissionPair, runtimeContext, false);

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
						effect, childEmissionPair, particleRuntimeContext,
						childEmitterEmissionData, childPTypeEmissionData));
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

	ParticleEmitterEmissionData emitterEmissionData(effect, emissionPair.emitterId, runtimeContext, false);
	ParticleTypeEmissionData ptypeEmissionData(effect, emissionPair, runtimeContext, false);

	initializeParticles(count, 1.0,
		state, *particleEmissionState,
		*particleCollection, nullptr, id_t::nullValue,
		effect, emissionPair, runtimeContext,
		emitterEmissionData, ptypeEmissionData);
}

DefaultParticleGenerator::ParticleEmitterEmissionData::ParticleEmitterEmissionData(const Effect* effect, id_t particleEmitterId, EffectRuntimeContext runtimeContext, bool useTriggers) {
	const ParticleEmitter& particleEmitter = effect->sceneGraph().at<ParticleEmitter>(particleEmitterId);

	EffectRuntimeContext prevRuntimeContext(runtimeContext.time() - 0.1);
	prevRuntimeContext.triggerActivationTimes() = runtimeContext.triggerActivationTimes();

	float_t life = particleEmitter.life(runtimeContext, useTriggers);

	globalTransform = effect->sceneGraph().globalTransform(particleEmitterId, runtimeContext, useTriggers);
	globalPrevTransform = effect->sceneGraph().globalTransform(particleEmitterId, prevRuntimeContext, useTriggers);
	globalPosition = globalTransform.position();
	globalRotation = math::radians(globalTransform.rotation());
	globalScale = globalTransform.scale();
	globalRotationMatrix = matrix3_t(math::yawPitchRollRotationMatrix(globalRotation.y, globalRotation.z, globalRotation.x));
	shape = particleEmitter.shape();
	path = particleEmitter.path();
	distribution = particleEmitter.distribution();
	gridOrder = particleEmitter.gridOrder();
	gridSize[0] = particleEmitter.gridSizeX();
	gridSize[1] = particleEmitter.gridSizeY();
	gridSize[2] = particleEmitter.gridSizeZ();
	emissionMode = particleEmitter.emissionMode();
	directionMode = particleEmitter.directionMode();
	direction = particleEmitter.direction().at(life);
	spread = particleEmitter.spread().at(life);
	velocity = (globalPosition - globalPrevTransform.position()) / (particleEmitter.lifetimeDuration() * 0.1);
}

DefaultParticleGenerator::ParticleTypeEmissionData::ParticleTypeEmissionData(const Effect* effect, ParticleEmissionPair emissionPair, EffectRuntimeContext runtimeContext, bool useTriggers) {
	const ParticleEmitter& particleEmitter = effect->sceneGraph().at<ParticleEmitter>(emissionPair.emitterId);
	const ParticleType& particleType = effect->particleTypes().at(emissionPair.typeId);

	float_t emitterLife = particleEmitter.life(runtimeContext, useTriggers);

	localCoords = particleType.positionRelative();
	lifespan = particleType.lifespan().at(emitterLife);
	lifespanVariance = particleType.lifespanVariance().value();
	initialVelocity = particleType.initialVelocity().at(emitterLife);
	inheritedVelocity = particleType.inheritedVelocity().at(emitterLife);
	velocityVariance = particleType.velocityVariance().value();
	initialRotation = particleType.initialRotation().at(emitterLife);
	rotationVariance = particleType.rotationVariance().value();
	angularVelocityVariance = particleType.angularVelocityVariance().value();
	initialSize = particleType.initialSize().at(emitterLife);
	sizeVariance = particleType.sizeVariance().value();
	size = particleType.size().at();
	initialOpacity = particleType.initialOpacity().at(emitterLife);
	colorVariance = particleType.colorVariance().value();
	opacityVariance = particleType.opacityVariance().value();
	color = float4_t(float3_t(particleType.color().at()), particleType.opacity().at());
}

std::uint32_t DefaultParticleGenerator::initializeParticles(std::uint32_t count, float_t lifetimeFactor,
	EffectRuntimeState& state, ParticleEmissionState& emissionState,
	ParticleCollection& particleCollection, const ParticleCollection* parentParticleCollection, std::uint32_t parentParticle,
	const Effect* effect, ParticleEmissionPair emissionPair, EffectRuntimeContext runtimeContext,
	const ParticleEmitterEmissionData& emitterEmissionData,
	const ParticleTypeEmissionData& ptypeEmissionData) {
	bool effect3d = effect->is3d();
	std::uint32_t parentId = id_t::nullValue;

	float3_t emissionPosition;
	float3_t parentVelocity;

	if(parentParticle == id_t::nullValue) {
		emissionPosition = !ptypeEmissionData.localCoords ? emitterEmissionData.globalPosition : float3_t(0.0);
		parentVelocity = emitterEmissionData.velocity;
	}
	else {
		ParticleCollection::ReadPtr parentParticles = parentParticleCollection->readPtr();
		Transform localEmitterTransform = effect->sceneGraph().localTransform(emissionPair.emitterId, runtimeContext, false);

		emissionPosition = parentParticles.globalPosition[parentParticle] + localEmitterTransform.position() +
			(ptypeEmissionData.localCoords ? -emitterEmissionData.globalPosition : float3_t(0.0));
		parentId = parentParticles.id[parentParticle];
		parentVelocity = parentParticles.velocity[parentParticle];
	}

	float_t parentSpeed = math::length(parentVelocity);
	float3_t parentDirection = parentSpeed != 0.0 ? parentVelocity / parentSpeed : worldUpVector3;

	count = particleCollection.add(count);

	ParticleCollection::WritePtr particles = particleCollection.writePtr();
	std::uint32_t particleCollectionCount = particleCollection.count();

	pcg32& rng = state.rng();

	for(std::uint32_t addIndex = 0; addIndex < count; addIndex++) {
		std::uint32_t p = particleCollectionCount - count + addIndex;

		particles.id[p] = state.particleIdCounter()++;
		particles.parentId[p] = parentId;
		particles.life[p] = 0.0;
		particles.lifespan[p] = std::max((ptypeEmissionData.lifespan + rng.next(-ptypeEmissionData.lifespanVariance, ptypeEmissionData.lifespanVariance)) * lifetimeFactor, 0.000001);

		float3_t particleSpawnPosition = float3_t(0.0);
		switch(emitterEmissionData.shape) {
			case ParticleEmitter::Shape::line:
				particleSpawnPosition = emitOnSegment(
					emitterEmissionData.globalScale.x,
					emitterEmissionData.distribution,
					emitterEmissionData.gridOrder,
					emitterEmissionData.gridSize[0],
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::ellipse:
				particleSpawnPosition = emitInEllipse(
					float2_t(emitterEmissionData.globalScale),
					emitterEmissionData.distribution,
					emitterEmissionData.gridOrder,
					emitterEmissionData.gridSize[0], emitterEmissionData.gridSize[1],
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::rectangle:
				particleSpawnPosition = emitInRectangle(
					float2_t(emitterEmissionData.globalScale),
					emitterEmissionData.distribution,
					emitterEmissionData.gridOrder,
					emitterEmissionData.gridSize[0], emitterEmissionData.gridSize[1],
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::path:
				particleSpawnPosition = emitOnPath(
					emitterEmissionData.globalScale,
					emitterEmissionData.path,
					emitterEmissionData.distribution,
					emitterEmissionData.gridOrder,
					emitterEmissionData.gridSize[0],
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::ellipsoid:
				particleSpawnPosition = emitInEllipsoid(
					emitterEmissionData.globalScale,
					emitterEmissionData.distribution,
					emitterEmissionData.gridOrder,
					emitterEmissionData.gridSize[0], emitterEmissionData.gridSize[1], emitterEmissionData.gridSize[2],
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::cuboid:
				particleSpawnPosition = emitInCuboid(
					emitterEmissionData.globalScale,
					emitterEmissionData.distribution,
					emitterEmissionData.gridOrder,
					emitterEmissionData.gridSize[0], emitterEmissionData.gridSize[1], emitterEmissionData.gridSize[2],
					emissionState.emitterGridIndex,
					rng);
				break;
			case ParticleEmitter::Shape::cylinder:
				particleSpawnPosition = emitInCylinder(
					emitterEmissionData.globalScale,
					emitterEmissionData.distribution,
					emitterEmissionData.gridOrder,
					emitterEmissionData.gridSize[0], emitterEmissionData.gridSize[1], emitterEmissionData.gridSize[2],
					emissionState.emitterGridIndex,
					rng);
				break;
			default:
				break;
		}

		particleSpawnPosition = emitterEmissionData.globalRotationMatrix * particleSpawnPosition;

		matrix3_t directionMatrix = matrix3_t(math::yawPitchRollRotationMatrix(
			effect3d ? math::radians(emitterEmissionData.direction.y + emitterEmissionData.spread * rng.next(-0.5, 0.5)) : 0.0,
			effect3d ? math::radians(emitterEmissionData.direction.z + emitterEmissionData.spread * rng.next(-0.5, 0.5)) : 0.0,
			math::radians(emitterEmissionData.direction.x + emitterEmissionData.spread * rng.next(-0.5, 0.5))));

		switch(emitterEmissionData.directionMode) {
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
				particles.velocity[p] = emitterEmissionData.globalRotationMatrix * directionMatrix * worldUpVector3;
				break;
		}

		particles.velocity[p] *= math::linearInterpolation(ptypeEmissionData.initialVelocity, parentSpeed, ptypeEmissionData.inheritedVelocity) +
			rng.next(-ptypeEmissionData.velocityVariance, ptypeEmissionData.velocityVariance);
		particles.force[p] = float3_t(0.0);
		particles.position[p] = emissionPosition + particleSpawnPosition;
		particles.globalPosition[p] = ptypeEmissionData.localCoords
			? particles.position[p] + emitterEmissionData.globalPosition
			: particles.position[p];

		particles.initialRotation[p] = ptypeEmissionData.initialRotation + float3_t(
			rng.next(-ptypeEmissionData.rotationVariance.x, ptypeEmissionData.rotationVariance.x),
			rng.next(-ptypeEmissionData.rotationVariance.y, ptypeEmissionData.rotationVariance.y),
			rng.next(-ptypeEmissionData.rotationVariance.z, ptypeEmissionData.rotationVariance.z));
		particles.initialAngularVelocity[p] = float3_t(
			rng.next(-ptypeEmissionData.angularVelocityVariance.x, ptypeEmissionData.angularVelocityVariance.x),
			rng.next(-ptypeEmissionData.angularVelocityVariance.y, ptypeEmissionData.angularVelocityVariance.y),
			rng.next(-ptypeEmissionData.angularVelocityVariance.z, ptypeEmissionData.angularVelocityVariance.z));
		particles.rotation[p] = particles.initialRotation[p];

		particles.initialSize[p] = ptypeEmissionData.initialSize + rng.next(-ptypeEmissionData.sizeVariance, ptypeEmissionData.sizeVariance);
		particles.size[p] = ptypeEmissionData.size * particles.initialSize[p];

		particles.initialColor[p] = float4_t(
			rng.next(-ptypeEmissionData.colorVariance.x, ptypeEmissionData.colorVariance.x),
			rng.next(-ptypeEmissionData.colorVariance.y, ptypeEmissionData.colorVariance.y),
			rng.next(-ptypeEmissionData.colorVariance.z, ptypeEmissionData.colorVariance.z),
			ptypeEmissionData.initialOpacity + rng.next(-ptypeEmissionData.opacityVariance, ptypeEmissionData.opacityVariance));
		particles.color[p] = ptypeEmissionData.color;
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
			r = rng.next();
			r *= r;
			phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			r = std::sqrt(std::sqrt(std::sqrt(rng.next())));
			phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi),
				std::sin(phi),
				0.0) * r;

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
			float_t r = rng.next(0.0, (size.x + size.y) * 2.0);
			float_t d = rng.next();
			d = d * d * d * d * 0.5;

			point = float3_t(-size * 0.5, 0.0);

			if(r < size.y) {
				point += float3_t(d * size.x, r, 0.0);
			}
			else if(r < size.y + size.x) {
				point += float3_t(r - size.y, size.y - d * size.y, 0.0);
			}
			else if(r < (size.y + size.x) + size.y) {
				point += float3_t(size.x - d * size.x, r - (size.y + size.x), 0.0);
			}
			else {
				point += float3_t(r - (size.y + size.x + size.y), d * size.y, 0.0);
			}

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
	const Polyline& path,
	ParticleEmitter::Distribution distribution,
	ParticleEmitter::GridOrder gridOrder,
	std::uint32_t gridSize, std::uint32_t& gridIndex,
	pcg32& rng) {
	float_t d = 0.0;

	switch(distribution) {
		case ParticleEmitter::Distribution::uniform:
		case ParticleEmitter::Distribution::boundary: {
			d = rng.next(0.0, path.length());
			break;
		}
		case ParticleEmitter::Distribution::center: {
			d = randomCentered(rng, 0.0, path.length());
			break;
		}
		case ParticleEmitter::Distribution::hole: {
			d = randomInverseCentered(rng, 0.0, path.length());
			break;
		}
		case ParticleEmitter::Distribution::grid_random: {
			d = randomUniformGrid(rng, gridSize, 0.0, path.length());
			break;
		}
		case ParticleEmitter::Distribution::grid_ordered: {
			d = sampleGrid1d(gridIndex, gridSize, 0.0, path.length());
			gridIndex = (gridIndex + 1) % gridSize;
			break;
		}
		default: {
			break;
		}
	}

	return path.sample(d) * size;
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
			float_t r = rng.next();
			r *= r;
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
		case ParticleEmitter::Distribution::hole: {
			float_t r = std::sqrt(std::sqrt(std::sqrt(rng.next())));
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
			std::int32_t side = std::uniform_int_distribution<std::int32_t>(0, 5)(rng);
			float_t d = rng.next();
			d = d * d * d * d;

			switch(side) {
				case 0:
					point = float3_t(-size.x + d * size.x, rng.next(-size.y, +size.y), rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 1:
					point = float3_t(+size.x - d * size.x, rng.next(-size.y, +size.y), rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 2:
					point = float3_t(rng.next(-size.x, +size.x), -size.y + d * size.y, rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 3:
					point = float3_t(rng.next(-size.x, +size.x), +size.y - d * size.y, rng.next(-size.z, +size.z)) * 0.5;
					break;
				case 4:
					point = float3_t(rng.next(-size.x, +size.x), rng.next(-size.y, +size.y), -size.z + d * size.z) * 0.5;
					break;
				case 5:
					point = float3_t(rng.next(-size.x, +size.x), rng.next(-size.y, +size.y), +size.z - d * size.z) * 0.5;
					break;
				default:
					break;
			}

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
			float_t h = randomCentered(rng, -1.0, +1.0);
			float_t r = rng.next();
			r *= r;
			float_t phi = rng.next(0.0, math::twoPi);

			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

			break;
		}
		case ParticleEmitter::Distribution::hole: {
			float_t d = rng.next();
			d = d * d * d * d;

			float_t h = 0.0;
			float_t r = 0.0;
			float_t baseArea = size.x * size.y * math::pi;
			float_t lateralArea = math::pi * (size.x + size.y) * size.z;
			float_t side = rng.next(0.0, baseArea * 2.0 + lateralArea);
			if(side < baseArea) {
				h = -1.0 + d;
				r = std::sqrt(rng.next());
			}
			else if(side < baseArea * 2.0) {
				h = 1.0 - d;
				r = std::sqrt(rng.next());
			}
			else {
				h = rng.next(-1.0, +1.0);
				r = std::sqrt(std::sqrt(std::sqrt(rng.next())));
			}

			float_t phi = rng.next(0.0, math::twoPi);
			point = float3_t(
				std::cos(phi) * r,
				std::sin(phi) * r,
				h);

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
