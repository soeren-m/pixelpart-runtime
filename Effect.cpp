#include "Effect.h"
#include "JSONUtil.h"

namespace pixelpart {
bool isNameUsed(const Effect& effect, const std::string& name) {
	for(const ParticleEmitter& emitter : effect.particleEmitters) {
		if(emitter.name == name) {
			return true;
		}
	}
	for(const Sprite& sprite : effect.sprites) {
		if(sprite.name == name) {
			return true;
		}
	}
	for(const ForceField& forceField : effect.forceFields) {
		if(forceField.name == name) {
			return true;
		}
	}
	for(const Collider& collider : effect.colliders) {
		if(collider.name == name) {
			return true;
		}
	}

	return false;
}
bool isResourceUsed(const Effect& effect, const std::string& resourceId) {
	for(const ParticleEmitter& emitter : effect.particleEmitters) {
		const auto& shaderNodes = emitter.particleShader.getNodes();

		for(const auto& nodeEntry : shaderNodes) {
			for(const auto& nodeParameter : nodeEntry.second.parameters) {
				if(nodeParameter.type == pixelpart::ShaderParameter::Value::type_resource_image) {
					if(resourceId == nodeParameter.getResourceId()) {
						return true;
					}
				}
			}
		}
	}

	for(const Sprite& sprite : effect.sprites) {
		const auto& shaderNodes = sprite.shader.getNodes();

		for(const auto& nodeEntry : shaderNodes) {
			for(const auto& nodeParameter : nodeEntry.second.parameters) {
				if(nodeParameter.type == pixelpart::ShaderParameter::Value::type_resource_image) {
					if(resourceId == nodeParameter.getResourceId()) {
						return true;
					}
				}
			}
		}
	}

	return false;
}

void to_json(nlohmann::ordered_json& j, const Effect& effect) {
	j = nlohmann::ordered_json{
		{ "emitters", effect.particleEmitters.get() },
		{ "sprites", effect.sprites.get() },
		{ "force_fields", effect.forceFields.get() },
		{ "colliders", effect.colliders.get() }
	};
}
void from_json(const nlohmann::ordered_json& j, Effect& effect) {
	std::vector<ParticleEmitter> particleEmitters;
	std::vector<Sprite> sprites;
	std::vector<ForceField> forceFields;
	std::vector<Collider> colliders;

	fromJson(particleEmitters, j, "emitters", "");
	fromJson(sprites, j, "sprites", "");
	fromJson(forceFields, j, "force_fields", "");
	fromJson(colliders, j, "colliders", "");

	const nlohmann::ordered_json& jEmitterArray = j.at("emitters");

	for(uint32_t i = 0; i < particleEmitters.size(); i++) {
		const ParticleEmitter& emitter = particleEmitters[i];

		if(jEmitterArray[i].contains("particle_spawn_on_step_emitter")) {
			uint32_t subEmitterId = NullId;
			floatd subEmitterSpawnProb = 1.0;
			fromJson(subEmitterId, jEmitterArray[i], "particle_spawn_on_step_emitter", "");
			fromJson(subEmitterSpawnProb, jEmitterArray[i], "particle_spawn_on_step_prob", "");

			for(ParticleEmitter& subEmitter : particleEmitters) {
				if(subEmitter.id == subEmitterId) {
					subEmitter.parentId = emitter.id;
					subEmitter.instantiationMode = ParticleEmitter::InstantiationMode::continuous;
					subEmitter.numParticles.moveByFactor(subEmitterSpawnProb);
				}
			}
		}

		if(jEmitterArray[i].contains("particle_spawn_on_death_emitter")) {
			uint32_t subEmitterId = NullId;
			floatd subEmitterSpawnProb = 1.0;
			fromJson(subEmitterId, jEmitterArray[i], "particle_spawn_on_death_emitter", "");
			fromJson(subEmitterSpawnProb, jEmitterArray[i], "particle_spawn_on_death_prob", "");

			for(ParticleEmitter& subEmitter : particleEmitters) {
				if(subEmitter.id == subEmitterId) {
					subEmitter.parentId = emitter.id;
					subEmitter.instantiationMode = ParticleEmitter::InstantiationMode::burst_death;
					if(jEmitterArray[i].contains("particle_spawn_on_death_number")) {
						subEmitter.numParticles = Curve<floatd>(static_cast<floatd>(jEmitterArray[i].at("particle_spawn_on_death_number").get<uint32_t>()));
						subEmitter.numParticles.moveByFactor(subEmitterSpawnProb);
					}
				}
			}
		}

		if(jEmitterArray[i].contains("force_fields")) {
			std::vector<ForceField> emitterForceFields = jEmitterArray[i].at("force_fields").get<std::vector<ForceField>>();
			for(ForceField& forceField : emitterForceFields) {
				forceField.emitterMask.reset();
				forceField.emitterMask[emitter.id] = true;
				forceField.lifetimeStart = 0.0;
				forceField.lifetimeDuration = 1.0;
				forceField.repeat = true;
				forceFields.push_back(forceField);
			}
		}

		if(jEmitterArray[i].contains("colliders")) {
			std::vector<Collider> emitterColliders = jEmitterArray[i].at("colliders").get<std::vector<Collider>>();
			for(Collider& collider : emitterColliders) {
				collider.emitterMask.reset();
				collider.emitterMask[emitter.id] = true;
				collider.lifetimeStart = 0.0;
				collider.lifetimeDuration = 1.0;
				collider.repeat = true;
				colliders.push_back(collider);
			}
		}
	}

	for(ParticleEmitter& emitter : particleEmitters) {
		if(emitter.id == NullId) {
			emitter.id = findUnusedNodeId(particleEmitters);
		}
	}
	for(Sprite& sprite : sprites) {
		if(sprite.id == NullId) {
			sprite.id = findUnusedNodeId(sprites);
		}
	}
	for(ForceField& forceField : forceFields) {
		if(forceField.id == NullId) {
			forceField.id = findUnusedNodeId(forceFields);
		}
	}
	for(Collider& collider : colliders) {
		if(collider.id == NullId) {
			collider.id = findUnusedNodeId(colliders);
		}
	}

	effect.particleEmitters.set(particleEmitters);
	effect.sprites.set(sprites);
	effect.forceFields.set(forceFields);
	effect.colliders.set(colliders);
}
}