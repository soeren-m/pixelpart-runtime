#include "Project.h"
#include "../common/Json.h"

namespace pixelpart {
const uint32_t Project::version = 8u;

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
			while(isNameUsedInEffect(project.effect, name)) {
				name = "Emitter" + std::to_string(counter++);
			}

			particleEmitter.name = name;
		}
	}

	for(ParticleType& particleType : project.effect.particleTypes) {
		if(particleType.name.empty()) {
			uint32_t counter = 1u;
			std::string name = "Particle";
			while(isNameUsedInEffect(project.effect, name)) {
				name = "Particle" + std::to_string(counter++);
			}

			particleType.name = name;
		}
	}

	for(ForceField& forceField : project.effect.forceFields) {
		if(forceField.name.empty()) {
			uint32_t counter = 1u;
			std::string name = "Force";
			while(isNameUsedInEffect(project.effect, name)) {
				name = "Force" + std::to_string(counter++);
			}

			forceField.name = name;
		}
	}

	for(Collider& collider : project.effect.colliders) {
		if(collider.name.empty()) {
			uint32_t counter = 1u;
			std::string name = "Collider";
			while(isNameUsedInEffect(project.effect, name)) {
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