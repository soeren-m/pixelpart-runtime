#include "Effect.h"
#include "../common/Constants.h"
#include "../common/Json.h"
#include <vector>

namespace pixelpart {
template <typename T>
id_t findUnusedNodeId(const std::vector<T>& nodes) {
	id_t id = 0u;
	bool isUsed = true;

	while(isUsed) {
		id++;
		isUsed = false;

		for(const T& node : nodes) {
			if(node.id == id) {
				isUsed = true;
				break;
			}
		}
	}

	return id;
}

void Effect::refreshProperties() {
	ComputeGraph::InputSet inputValues;
	for(const auto& entry : inputs) {
		inputValues[entry.first] = entry.second.value;
	}

	for(ParticleEmitter& particleEmitter : particleEmitters) {
		particleEmitter.position.refresh(inputValues);
		particleEmitter.size.refresh(inputValues);
		particleEmitter.orientation.refresh(inputValues);
		particleEmitter.direction.refresh(inputValues);
		particleEmitter.spread.refresh(inputValues);
	}

	for(ParticleType& particleType : particleTypes) {
		particleType.position.refresh(inputValues);
		particleType.numParticles.refresh(inputValues);
		particleType.lifespan.refresh(inputValues);
		particleType.lifespanVariance.refresh(inputValues);
		particleType.motionPathForce.refresh(inputValues);
		particleType.initialVelocity.refresh(inputValues);
		particleType.inheritedVelocity.refresh(inputValues);
		particleType.velocityVariance.refresh(inputValues);
		particleType.acceleration.refresh(inputValues);
		particleType.radialAcceleration.refresh(inputValues);
		particleType.initialRotation.refresh(inputValues);
		particleType.rotation.refresh(inputValues);
		particleType.rotationBySpeed.refresh(inputValues);
		particleType.rotationVariance.refresh(inputValues);
		particleType.angularVelocityVariance.refresh(inputValues);
		particleType.pivot.refresh(inputValues);
		particleType.physicalSize.refresh(inputValues);
		particleType.weight.refresh(inputValues);
		particleType.bounce.refresh(inputValues);
		particleType.friction.refresh(inputValues);
		particleType.initialSize.refresh(inputValues);
		particleType.size.refresh(inputValues);
		particleType.sizeVariance.refresh(inputValues);
		particleType.stretch.refresh(inputValues);
		particleType.color.refresh(inputValues);
		particleType.colorVariance.refresh(inputValues);
		particleType.initialOpacity.refresh(inputValues);
		particleType.opacity.refresh(inputValues);
		particleType.opacityVariance.refresh(inputValues);
	}

	for(ForceField& forceField : forceFields) {
		forceField.position.refresh(inputValues);
		forceField.size.refresh(inputValues);
		forceField.orientation.refresh(inputValues);
		forceField.strength.refresh(inputValues);
		forceField.accelerationField.direction.refresh(inputValues);
		forceField.accelerationField.directionVariance.refresh(inputValues);
		forceField.accelerationField.strengthVariance.refresh(inputValues);
		forceField.vectorField.tightness.refresh(inputValues);
		forceField.noiseField.octaves.refresh(inputValues);
		forceField.noiseField.frequency.refresh(inputValues);
		forceField.noiseField.persistence.refresh(inputValues);
		forceField.noiseField.lacunarity.refresh(inputValues);
		forceField.noiseField.animationTimeScale.refresh(inputValues);
		forceField.noiseField.animationTimeBase.refresh(inputValues);
		forceField.dragField.velocityInfluence.refresh(inputValues);
		forceField.dragField.sizeInfluence.refresh(inputValues);
	}

	for(Collider& collider : colliders) {
		collider.position.refresh(inputValues);
		collider.width.refresh(inputValues);
		collider.orientation.refresh(inputValues);
		collider.killOnContact.refresh(inputValues);
		collider.bounce.refresh(inputValues);
		collider.friction.refresh(inputValues);
	}

	for(LightSource& lightSource : lightSources) {
		lightSource.position.refresh(inputValues);
		lightSource.direction.refresh(inputValues);
		lightSource.range.refresh(inputValues);
		lightSource.attenuation.refresh(inputValues);
		lightSource.spotAngle.refresh(inputValues);
		lightSource.spotAngleAttenuation.refresh(inputValues);
		lightSource.color.refresh(inputValues);
		lightSource.intensity.refresh(inputValues);
	}
}

void to_json(nlohmann::ordered_json& j, const Effect& effect) {
	j = nlohmann::ordered_json{
		{ "3d", effect.is3d },

		{ "particle_emitters", effect.particleEmitters.get() },
		{ "particle_types", effect.particleTypes.get() },
		{ "force_fields", effect.forceFields.get() },
		{ "colliders", effect.colliders.get() },
		{ "light_sources", effect.lightSources.get() },

		{ "inputs", effect.inputs },

		{ "resources", effect.resources }
	};
}
void from_json(const nlohmann::ordered_json& j, Effect& effect) {
	std::vector<ParticleEmitter> particleEmitters;
	std::vector<ParticleType> particleTypes;
	std::vector<ForceField> forceFields;
	std::vector<Collider> colliders;
	std::vector<LightSource> lightSources;

	effect = Effect();

	fromJson(effect.is3d, j, "3d");

	fromJson(particleEmitters, j, "particle_emitters");
	fromJson(particleTypes, j, "particle_types");
	fromJson(forceFields, j, "force_fields");
	fromJson(colliders, j, "colliders");
	fromJson(lightSources, j, "light_sources");

	fromJson(effect.inputs, j, "inputs");

	fromJson(effect.resources, j, "resources");

	for(ParticleEmitter& particleEmitter : particleEmitters) {
		if(particleEmitter.id == nullId) {
			particleEmitter.id = findUnusedNodeId(particleEmitters);
		}
	}
	for(ParticleType& particleType : particleTypes) {
		if(particleType.id == nullId) {
			particleType.id = findUnusedNodeId(particleTypes);
		}
	}
	for(ForceField& forceField : forceFields) {
		if(forceField.id == nullId) {
			forceField.id = findUnusedNodeId(forceFields);
		}
	}
	for(Collider& collider : colliders) {
		if(collider.id == nullId) {
			collider.id = findUnusedNodeId(colliders);
		}
	}
	for(LightSource& lightSource : lightSources) {
		if(lightSource.id == nullId) {
			lightSource.id = findUnusedNodeId(lightSources);
		}
	}

	effect.particleEmitters.set(particleEmitters);
	effect.particleTypes.set(particleTypes);
	effect.forceFields.set(forceFields);
	effect.colliders.set(colliders);
	effect.lightSources.set(lightSources);
}
}