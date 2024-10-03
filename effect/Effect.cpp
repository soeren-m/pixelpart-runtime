#include "Effect.h"
#include "../common/Id.h"
#include <vector>

namespace pixelpart {
Effect::Effect(bool is3d) : effect3d(is3d) {

}

void Effect::enable3d(bool mode) {
	effect3d = mode;
	// TODO
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
		particleType.count().refresh(inputValues);
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
		forceField.accelerationDirection().refresh(inputValues);
		forceField.accelerationDirectionVariance().refresh(inputValues);
		forceField.accelerationStrengthVariance().refresh(inputValues);
		forceField.vectorTightness().refresh(inputValues);
		forceField.noiseOctaves().refresh(inputValues);
		forceField.noiseFrequency().refresh(inputValues);
		forceField.noisePersistence().refresh(inputValues);
		forceField.noiseLacunarity().refresh(inputValues);
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
		{ "particle_emitters", effect.particleEmitters() },
		{ "particle_types", effect.particleTypes() },
		{ "force_fields", effect.forceFields() },
		{ "colliders", effect.colliders() },
		{ "light_sources", effect.lightSources() },
		{ "inputs", effect.inputs() },
		{ "resources", effect.resources() }
	};
}
void from_json(const nlohmann::ordered_json& j, Effect& effect) {
	effect = Effect(j.value("3d", false));
	effect.inputs() = j.value("inputs", EffectInputCollection());
	effect.resources() = j.value("resources", ResourceCollection());
	effect.particleEmitters() = j.value("particle_emitters", NodeCollection<ParticleEmitter>());
	effect.particleTypes() = j.value("particle_types", NodeCollection<ParticleType>());
	effect.forceFields() = j.value("force_fields", NodeCollection<ForceField>());
	effect.colliders() = j.value("colliders", NodeCollection<Collider>());
	effect.lightSources() = j.value("light_sources", NodeCollection<LightSource>());
}
}