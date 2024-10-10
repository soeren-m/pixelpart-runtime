#pragma once

#include "NodeCollection.h"
#include "ParticleEmitter.h"
#include "ParticleType.h"
#include "ForceField.h"
#include "Collider.h"
#include "LightSource.h"
#include "EffectInputCollection.h"
#include "ResourceCollection.h"
#include "../json/json.hpp"

namespace pixelpart {
class Effect {
public:
	Effect() = default;
	Effect(bool is3d);

	void enable3d(bool mode);
	bool is3d() const;

	NodeCollection<ParticleEmitter>& particleEmitters();
	const NodeCollection<ParticleEmitter>& particleEmitters() const;

	NodeCollection<ParticleType>& particleTypes();
	const NodeCollection<ParticleType>& particleTypes() const;

	NodeCollection<ForceField>& forceFields();
	const NodeCollection<ForceField>& forceFields() const;

	NodeCollection<Collider>& colliders();
	const NodeCollection<Collider>& colliders() const;

	NodeCollection<LightSource>& lightSources();
	const NodeCollection<LightSource>& lightSources() const;

	EffectInputCollection& inputs();
	const EffectInputCollection& inputs() const;

	ResourceCollection& resources();
	const ResourceCollection& resources() const;

	void refreshProperties();

private:
	bool effect3d = false;

	NodeCollection<ParticleEmitter> effectParticleEmitters;
	NodeCollection<ParticleType> effectParticleTypes;
	NodeCollection<ForceField> effectForceFields;
	NodeCollection<Collider> effectColliders;
	NodeCollection<LightSource> effectLightSources;

	EffectInputCollection effectInputs;

	ResourceCollection effectResources;
};

void to_json(nlohmann::ordered_json& j, const Effect& effect);
void from_json(const nlohmann::ordered_json& j, Effect& effect);
}