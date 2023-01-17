#include "Effect.h"
#include "JSONUtil.h"

namespace pixelpart {
template <typename T>
uint32_t findUnusedNodeId(const std::vector<T>& nodes) {
	uint32_t id = 0;
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

bool isNameUsed(const Effect& effect, const std::string& name) {
	for(const ParticleEmitter& particleEmitter : effect.particleEmitters) {
		if(particleEmitter.name == name) {
			return true;
		}
	}
	for(const ParticleType& particleType : effect.particleTypes) {
		if(particleType.name == name) {
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
	for(const ParticleType& particleType : effect.particleTypes) {
		const auto& shaderNodes = particleType.shader.getNodes();

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
		{ "particle_emitters", effect.particleEmitters.get() },
		{ "particle_types", effect.particleTypes.get() },
		{ "force_fields", effect.forceFields.get() },
		{ "colliders", effect.colliders.get() },
		{ "3d", effect.is3d }
	};
}
void from_json(const nlohmann::ordered_json& j, Effect& effect) {
	std::vector<ParticleEmitter> particleEmitters;
	std::vector<ParticleType> particleTypes;
	std::vector<ForceField> forceFields;
	std::vector<Collider> colliders;

	effect = Effect();

	fromJson(particleEmitters, j, "particle_emitters");
	fromJson(particleTypes, j, "particle_types");
	fromJson(forceFields, j, "force_fields");
	fromJson(colliders, j, "colliders");
	fromJson(effect.is3d, j, "3d");

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

	effect.particleEmitters.set(particleEmitters);
	effect.particleTypes.set(particleTypes);
	effect.forceFields.set(forceFields);
	effect.colliders.set(colliders);
}
}