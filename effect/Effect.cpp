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

Effect::Effect(bool is3d) : effect3d(is3d) {

}

bool Effect::is3d() const {
	return effect3d;
}

NodeCollection<ParticleEmitter>& Effect::particleEmitters() {
	return effectParticleEmitters;
}
const NodeCollection<ParticleEmitter>& Effect::particleEmitters() const {
	return effectParticleEmitters;
}

NodeCollection<ParticleType>& Effect::particleTypes() {
	return effectParticleTypes;
}
const NodeCollection<ParticleType>& Effect::particleTypes() const {
	return effectParticleTypes;
}

NodeCollection<ForceField>& Effect::forceFields() {
	return effectForceFields;
}
const NodeCollection<ForceField>& Effect::forceFields() const {
	return effectForceFields;
}

NodeCollection<Collider>& Effect::colliders() {
	return effectColliders;
}
const NodeCollection<Collider>& Effect::colliders() const {
	return effectColliders;
}

NodeCollection<LightSource>& Effect::lightSources() {
	return effectLightSources;
}
const NodeCollection<LightSource>& Effect::lightSources() const {
	return effectLightSources;
}

EffectInputCollection& Effect::inputs() {
	return effectInputs;
}
const EffectInputCollection& Effect::inputs() const {
	return effectInputs;
}

ResourceCollection& Effect::resources() {
	return effectResources;
}
const ResourceCollection& Effect::resources() const {
	return effectResources;
}

void Effect::refreshProperties() {
	ComputeGraph::InputSet inputValues;
	for(const auto& entry : effectInputs) {
		inputValues[entry.first] = entry.second.value;
	}

	for(ParticleEmitter& particleEmitter : effectParticleEmitters) {
		particleEmitter.position().refresh(inputValues);
		particleEmitter.size().refresh(inputValues);
		particleEmitter.orientation().refresh(inputValues);
		particleEmitter.direction().refresh(inputValues);
		particleEmitter.spread().refresh(inputValues);
	}

	for(ParticleType& particleType : effectParticleTypes) {
		particleType.position().refresh(inputValues);
		particleType.numParticles().refresh(inputValues);
		particleType.lifespan().refresh(inputValues);
		particleType.lifespanVariance().refresh(inputValues);
		particleType.motionPathForce().refresh(inputValues);
		particleType.initialVelocity().refresh(inputValues);
		particleType.inheritedVelocity().refresh(inputValues);
		particleType.velocityVariance().refresh(inputValues);
		particleType.acceleration().refresh(inputValues);
		particleType.radialAcceleration().refresh(inputValues);
		particleType.initialRotation().refresh(inputValues);
		particleType.rotation().refresh(inputValues);
		particleType.rotationBySpeed().refresh(inputValues);
		particleType.rotationVariance().refresh(inputValues);
		particleType.angularVelocityVariance().refresh(inputValues);
		particleType.pivot().refresh(inputValues);
		particleType.physicalSize().refresh(inputValues);
		particleType.weight().refresh(inputValues);
		particleType.bounce().refresh(inputValues);
		particleType.friction().refresh(inputValues);
		particleType.initialSize().refresh(inputValues);
		particleType.size().refresh(inputValues);
		particleType.sizeVariance().refresh(inputValues);
		particleType.stretch().refresh(inputValues);
		particleType.color().refresh(inputValues);
		particleType.colorVariance().refresh(inputValues);
		particleType.initialOpacity().refresh(inputValues);
		particleType.opacity().refresh(inputValues);
		particleType.opacityVariance().refresh(inputValues);
	}

	for(ForceField& forceField : effectForceFields) {
		forceField.position().refresh(inputValues);
		forceField.size().refresh(inputValues);
		forceField.orientation().refresh(inputValues);
		forceField.strength().refresh(inputValues);
		forceField.accelerationField.direction().refresh(inputValues);
		forceField.accelerationField.directionVariance().refresh(inputValues);
		forceField.accelerationField.strengthVariance().refresh(inputValues);
		forceField.vectorField.tightness().refresh(inputValues);
		forceField.noiseField.octaves().refresh(inputValues);
		forceField.noiseField.frequency().refresh(inputValues);
		forceField.noiseField.persistence().refresh(inputValues);
		forceField.noiseField.lacunarity().refresh(inputValues);
		forceField.noiseAnimationTimeScale().refresh(inputValues);
		forceField.noiseAnimationTimeBase().refresh(inputValues);
		forceField.dragVelocityInfluence().refresh(inputValues);
		forceField.dragSizeInfluence().refresh(inputValues);
	}

	for(Collider& collider : effectColliders) {
		collider.position().refresh(inputValues);
		collider.width().refresh(inputValues);
		collider.orientation().refresh(inputValues);
		collider.killOnContact().refresh(inputValues);
		collider.bounce().refresh(inputValues);
		collider.friction().refresh(inputValues);
	}

	for(LightSource& lightSource : effectLightSources) {
		lightSource.position().refresh(inputValues);
		lightSource.direction().refresh(inputValues);
		lightSource.range().refresh(inputValues);
		lightSource.attenuation().refresh(inputValues);
		lightSource.spotAngle().refresh(inputValues);
		lightSource.spotAngleAttenuation().refresh(inputValues);
		lightSource.color().refresh(inputValues);
		lightSource.intensity().refresh(inputValues);
	}
}

void to_json(nlohmann::ordered_json& j, const Effect& effect) {
	j = nlohmann::ordered_json{
		{ "3d", effect.is3d() },
		{ "particle_emitters", effect.particleEmitters().get() },
		{ "particle_types", effect.particleTypes().get() },
		{ "force_fields", effect.forceFields().get() },
		{ "colliders", effect.colliders().get() },
		{ "light_sources", effect.lightSources().get() },
		{ "inputs", effect.inputs() },
		{ "resources", effect.resources() }
	};
}
void from_json(const nlohmann::ordered_json& j, Effect& effect) {
	effect = Effect(j.value("3d", false));
	effect.inputs() = j.value("inputs", EffectInputCollection());
	effect.resources() = j.value("resources", ResourceCollection());

	std::vector<ParticleEmitter> particleEmitters = j.value("particle_emitters", std::vector<ParticleEmitter>());
	std::vector<ParticleType> particleTypes = j.value("particle_types", std::vector<ParticleType>());
	std::vector<ForceField> forceFields = j.value("force_fields", std::vector<ForceField>());
	std::vector<Collider> colliders = j.value("colliders", std::vector<Collider>());
	std::vector<LightSource> lightSources = j.value("light_sources", std::vector<LightSource>());

	for(ParticleEmitter& particleEmitter : particleEmitters) {
		if(particleEmitter.id() == nullId) {
			particleEmitter.id(findUnusedNodeId(particleEmitters));
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

	effect.particleEmitters().set(particleEmitters);
	effect.particleTypes().set(particleTypes);
	effect.forceFields().set(forceFields);
	effect.colliders().set(colliders);
	effect.lightSources().set(lightSources);
}
}