#include "DefaultParticleGenerator.h"
#include "SpawnPositionGeneration.h"
#include "SpawnDirectionGeneration.h"
#include "../effect/ParticleSimulationSpace.h"
#include "../effect/Coordinates.h"
#include "../effect/Curve.h"
#include "../math/Common.h"
#include "../math/MatrixCommon.h"
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
			ParticleEmitterEmissionData emitterEmissionData(effect, emissionPair.emitterId, runtimeContext, true, emissionState.emitterPrevTransform);
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

			ParticleEmitterEmissionData childEmitterEmissionData(effect, childEmissionPair.emitterId, runtimeContext, false, childEmissionState.emitterPrevTransform);
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

	for(auto& [emissionPair, emissionState] : state.particleEmissionStates()) {
		emissionState.emitterPrevTransform = effect->sceneGraph().globalTransform(emissionPair.emitterId, runtimeContext);
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

	ParticleEmitterEmissionData emitterEmissionData(effect, emissionPair.emitterId, runtimeContext, false, particleEmissionState->emitterPrevTransform);
	ParticleTypeEmissionData ptypeEmissionData(effect, emissionPair, runtimeContext, false);

	initializeParticles(count, 1.0,
		state, *particleEmissionState,
		*particleCollection, nullptr, id_t::nullValue,
		effect, emissionPair, runtimeContext,
		emitterEmissionData, ptypeEmissionData);
}

DefaultParticleGenerator::ParticleEmitterEmissionData::ParticleEmitterEmissionData(const Effect* effect, id_t particleEmitterId, EffectRuntimeContext runtimeContext, bool useTriggers, const Transform& prevTransform) {
	const ParticleEmitter& particleEmitter = effect->sceneGraph().at<ParticleEmitter>(particleEmitterId);

	float_t life = particleEmitter.life(runtimeContext, useTriggers);

	Transform transform = effect->sceneGraph().globalTransform(particleEmitterId, runtimeContext, useTriggers);
	float3_t globalRotation = math::radians(transform.rotation());

	globalTransform = math::normalizeTransformationMatrix(transform.matrix());
	invGlobalTransform = math::inverse(globalTransform);
	localTransform = math::normalizeTransformationMatrix(particleEmitter.transform(runtimeContext, false).matrix());
	globalPosition = transform.position();
	globalScale = transform.scale();
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
	velocity = float3_t(invGlobalTransform * float4_t((globalPosition - prevTransform.position()) / runtimeContext.deltaTime(), 0.0));
}

DefaultParticleGenerator::ParticleTypeEmissionData::ParticleTypeEmissionData(const Effect* effect, ParticleEmissionPair emissionPair, EffectRuntimeContext runtimeContext, bool useTriggers) {
	const ParticleEmitter& particleEmitter = effect->sceneGraph().at<ParticleEmitter>(emissionPair.emitterId);
	const ParticleType& particleType = effect->particleTypes().at(emissionPair.typeId);

	float_t emitterLife = particleEmitter.life(runtimeContext, useTriggers);

	localCoords = particleType.simulationSpace() == ParticleSimulationSpace::local;
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

std::uint32_t DefaultParticleGenerator::initializeParticles(std::uint32_t numParticlesToAdd, float_t lifetimeFactor,
	EffectRuntimeState& state, ParticleEmissionState& emissionState,
	ParticleCollection& particleCollection, const ParticleCollection* parentParticleCollection, std::uint32_t parentParticle,
	const Effect* effect, ParticleEmissionPair emissionPair, EffectRuntimeContext runtimeContext,
	const ParticleEmitterEmissionData& emitterEmissionData,
	const ParticleTypeEmissionData& ptypeEmissionData) {
	bool effect3d = effect->is3d();

	matrix4_t simulationSpaceToGlobalTransform = ptypeEmissionData.localCoords ? emitterEmissionData.globalTransform : matrix4_t(1.0);
	matrix4_t localToSimulationSpaceTransform;
	std::uint32_t parentId = id_t::nullValue;
	float3_t parentVelocity;

	if(parentParticle == id_t::nullValue) {
		localToSimulationSpaceTransform = !ptypeEmissionData.localCoords ? emitterEmissionData.globalTransform : matrix4_t(1.0);
		parentVelocity = emitterEmissionData.velocity;
	}
	else {
		ParticleCollection::ReadPtr parentParticles = parentParticleCollection->readPtr();
		parentId = parentParticles.id[parentParticle];

		localToSimulationSpaceTransform = math::translationMatrix(parentParticles.globalPosition[parentParticle]) * emitterEmissionData.localTransform;
		if(ptypeEmissionData.localCoords) {
			localToSimulationSpaceTransform = emitterEmissionData.invGlobalTransform * localToSimulationSpaceTransform;
		}

		parentVelocity = parentParticles.velocity[parentParticle];
	}

	float_t parentSpeed = math::length(parentVelocity);
	float3_t parentDirection = parentSpeed != 0.0 ? parentVelocity / parentSpeed : worldUpVector3;

	float_t initialSpeed = math::linearInterpolation(ptypeEmissionData.initialVelocity, parentSpeed, ptypeEmissionData.inheritedVelocity);

	numParticlesToAdd = particleCollection.add(numParticlesToAdd);

	ParticleCollection::WritePtr particles = particleCollection.writePtr();
	std::uint32_t particleCount = particleCollection.count();

	pcg32& rng = state.rng();
	std::uint32_t& idCounter = state.particleIdCounter();

	for(std::uint32_t addIndex = 0; addIndex < numParticlesToAdd; addIndex++) {
		std::uint32_t p = particleCount - numParticlesToAdd + addIndex;

		particles.id[p] = idCounter++;
		particles.parentId[p] = parentId;
		particles.life[p] = 0.0;
		particles.lifespan[p] = std::max((ptypeEmissionData.lifespan + rng.next(-ptypeEmissionData.lifespanVariance, ptypeEmissionData.lifespanVariance)) * lifetimeFactor, 0.000001);

		float3_t particleSpawnPosition = generateParticleSpawnPosition(
			emitterEmissionData.globalScale,
			emitterEmissionData.shape,
			emitterEmissionData.distribution,
			emitterEmissionData.gridOrder,
			emitterEmissionData.gridSize,
			emitterEmissionData.path,
			emissionState.emitterGridIndex,
			rng);

		float3_t particleSpawnDirection = generateParticleSpawnDirection(
			emitterEmissionData.directionMode,
			emitterEmissionData.direction,
			emitterEmissionData.spread,
			parentDirection,
			particleSpawnPosition,
			effect3d,
			rng);

		particles.velocity[p] = float3_t(localToSimulationSpaceTransform * float4_t(particleSpawnDirection, 0.0));
		particles.velocity[p] *= initialSpeed + rng.next(-ptypeEmissionData.velocityVariance, ptypeEmissionData.velocityVariance);
		particles.force[p] = float3_t(0.0);

		particles.position[p] = float3_t(localToSimulationSpaceTransform * float4_t(particleSpawnPosition, 1.0));
		particles.globalPosition[p] = float3_t(simulationSpaceToGlobalTransform * float4_t(particles.position[p], 1.0));

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

	return numParticlesToAdd;
}
}
