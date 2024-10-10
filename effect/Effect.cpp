#include "Effect.h"
#include "../common/Id.h"
#include <vector>

namespace pixelpart {
Effect::Effect(bool is3d) : effect3d(is3d) {

}

void Effect::enable3d(bool mode) {
	effect3d = mode;
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
		particleEmitter.position().input(inputValues);
		particleEmitter.size().input(inputValues);
		particleEmitter.orientation().input(inputValues);
		particleEmitter.direction().input(inputValues);
		particleEmitter.spread().input(inputValues);
	}

	for(ParticleType& particleType : effectParticleTypes) {
		particleType.position().input(inputValues);
		particleType.count().input(inputValues);
		particleType.lifespan().input(inputValues);
		particleType.lifespanVariance().input(inputValues);
		particleType.motionPathForce().input(inputValues);
		particleType.initialVelocity().input(inputValues);
		particleType.inheritedVelocity().input(inputValues);
		particleType.velocityVariance().input(inputValues);
		particleType.acceleration().input(inputValues);
		particleType.radialAcceleration().input(inputValues);
		particleType.initialRotation().input(inputValues);
		particleType.rotation().input(inputValues);
		particleType.rotationBySpeed().input(inputValues);
		particleType.rotationVariance().input(inputValues);
		particleType.angularVelocityVariance().input(inputValues);
		particleType.pivot().input(inputValues);
		particleType.physicalSize().input(inputValues);
		particleType.weight().input(inputValues);
		particleType.bounce().input(inputValues);
		particleType.friction().input(inputValues);
		particleType.initialSize().input(inputValues);
		particleType.size().input(inputValues);
		particleType.sizeVariance().input(inputValues);
		particleType.stretch().input(inputValues);
		particleType.color().input(inputValues);
		particleType.colorVariance().input(inputValues);
		particleType.initialOpacity().input(inputValues);
		particleType.opacity().input(inputValues);
		particleType.opacityVariance().input(inputValues);
	}

	for(ForceField& forceField : effectForceFields) {
		forceField.position().input(inputValues);
		forceField.size().input(inputValues);
		forceField.orientation().input(inputValues);
		forceField.strength().input(inputValues);
		forceField.accelerationDirection().input(inputValues);
		forceField.accelerationDirectionVariance().input(inputValues);
		forceField.accelerationStrengthVariance().input(inputValues);
		forceField.vectorTightness().input(inputValues);
		forceField.noiseOctaves().input(inputValues);
		forceField.noiseFrequency().input(inputValues);
		forceField.noisePersistence().input(inputValues);
		forceField.noiseLacunarity().input(inputValues);
		forceField.noiseAnimationTimeScale().input(inputValues);
		forceField.noiseAnimationTimeBase().input(inputValues);
		forceField.dragVelocityInfluence().input(inputValues);
		forceField.dragSizeInfluence().input(inputValues);
	}

	for(Collider& collider : effectColliders) {
		collider.position().input(inputValues);
		collider.width().input(inputValues);
		collider.orientation().input(inputValues);
		collider.killOnContact().input(inputValues);
		collider.bounce().input(inputValues);
		collider.friction().input(inputValues);
	}

	for(LightSource& lightSource : effectLightSources) {
		lightSource.position().input(inputValues);
		lightSource.direction().input(inputValues);
		lightSource.range().input(inputValues);
		lightSource.attenuation().input(inputValues);
		lightSource.spotAngle().input(inputValues);
		lightSource.spotAngleAttenuation().input(inputValues);
		lightSource.color().input(inputValues);
		lightSource.intensity().input(inputValues);
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