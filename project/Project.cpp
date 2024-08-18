#include "Project.h"
#include "../common/Json.h"

namespace pixelpart {
const uint32_t Project::version = 8u;

bool Project::isNameUsed(const std::string& name) const {
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
	for(const LightSource& lightSource : effect.lightSources) {
		if(lightSource.name == name) {
			return true;
		}
	}

	return false;
}
bool Project::isResourceUsed(const std::string& resourceId) const {
	for(const ParticleType& particleType : effect.particleTypes) {
		if(particleType.materialInstance.materialId == resourceId) {
			return true;
		}
		if(particleType.meshRendererSettings.meshResourceId == resourceId) {
			return true;
		}

		for(const auto& materialParameterEntry : particleType.materialInstance.materialParameters) {
			if(materialParameterEntry.second.type == pixelpart::VariantParameter::Value::type_resource_image &&
				resourceId == materialParameterEntry.second.getResourceId()) {
				return true;
			}
		}
	}

	for(const ForceField& forceField : effect.forceFields) {
		if(resourceId == forceField.vectorField.resourceId) {
			return true;
		}
	}

	for(const auto& resourceEntry : effect.resources.materials) {
		const MaterialResource& material = resourceEntry.second;

		for(const auto& nodeEntry : material.shaderGraph.getNodes()) {
			for(const auto& nodeParameter : nodeEntry.second.parameters) {
				if(nodeParameter.type == pixelpart::VariantParameter::Value::type_resource_image &&
					resourceId == nodeParameter.getResourceId()) {
					return true;
				}
			}
		}
	}

	return false;
}

void to_json(nlohmann::ordered_json& j, const Project& project) {
	j = nlohmann::ordered_json{
		{ "version", Project::version },

		{ "effect", project.effect },

		{ "image_effects", project.imageEffectSettings },
		{ "camera", project.cameraSettings },
		{ "render", project.renderSettings },
		{ "preview", project.previewSettings },
	};
}
void from_json(const nlohmann::ordered_json& j, Project& project) {
	uint32_t version = j.at("version");
	if(version != Project::version) {
		throw std::runtime_error("Unsupported project version " + std::to_string(version));
	}

	fromJson(project.effect, j, "effect");

	for(ParticleEmitter& particleEmitter : project.effect.particleEmitters) {
		if(particleEmitter.name.empty()) {
			uint32_t counter = 1u;
			std::string name = "Emitter";
			while(project.isNameUsed( name)) {
				name = "Emitter" + std::to_string(counter++);
			}

			particleEmitter.name = name;
		}
	}

	for(ParticleType& particleType : project.effect.particleTypes) {
		if(particleType.name.empty()) {
			uint32_t counter = 1u;
			std::string name = "Particle";
			while(project.isNameUsed(name)) {
				name = "Particle" + std::to_string(counter++);
			}

			particleType.name = name;
		}
	}

	for(ForceField& forceField : project.effect.forceFields) {
		if(forceField.name.empty()) {
			uint32_t counter = 1u;
			std::string name = "Force";
			while(project.isNameUsed(name)) {
				name = "Force" + std::to_string(counter++);
			}

			forceField.name = name;
		}
	}

	for(Collider& collider : project.effect.colliders) {
		if(collider.name.empty()) {
			uint32_t counter = 1u;
			std::string name = "Collider";
			while(project.isNameUsed(name)) {
				name = "Collider" + std::to_string(counter++);
			}

			collider.name = name;
		}
	}

	fromJson(project.imageEffectSettings, j, "image_effects");
	fromJson(project.cameraSettings, j, "camera");
	fromJson(project.renderSettings, j, "render");
	fromJson(project.previewSettings, j, "preview");
}

std::string serialize(const Project& project, int32_t indent) {
	nlohmann::ordered_json jsonData = project;

	return jsonData.dump(indent);
}

Project deserialize(std::istream& stream) {
	nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(stream);

	return jsonData.get<Project>();
}
Project deserialize(const std::string& data) {
	nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data);

	return jsonData.get<Project>();
}
Project deserialize(const char* data, std::size_t size) {
	nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data, data + size);

	return jsonData.get<Project>();
}
}