#include "Project.h"
#include "JSONUtil.h"

namespace pixelpart {
const uint32_t Project::version = 7;

void to_json(nlohmann::ordered_json& j, const Project& project) {
	j = nlohmann::ordered_json{
		{ "version", Project::version },

		{ "effect", project.effect },

		{ "postprocessing_pipeline", project.postProcessingPipeline },
		{ "background_color", project.backgroundColor },
		{ "camera_position", project.cameraPosition },
		{ "camera_zoom", project.cameraZoom },
		{ "camera_fov", project.cameraFieldOfView },
		{ "camera_pitch", project.cameraPitch },
		{ "camera_yaw", project.cameraYaw },
		{ "camera_roll", project.cameraRoll },

		{ "render_settings", project.renderSettings },
		{ "preview_settings", project.previewSettings },
	};
}
void from_json(const nlohmann::ordered_json& j, Project& project) {
	uint32_t version = j.at("version");
	if(version < 7 || version > Project::version) {
		throw std::runtime_error("Unsupported project version " + std::to_string(version));
	}

	fromJson(project.effect, j, "effect");

	for(ParticleEmitter& particleEmitter : project.effect.particleEmitters) {
		if(particleEmitter.name.empty()) {
			uint32_t counter = 1;
			std::string name = "Emitter";
			while(isNameUsed(project.effect, name)) {
				name = "Emitter" + std::to_string(counter++);
			}

			particleEmitter.name = name;
		}
	}

	for(ParticleType& particleType : project.effect.particleTypes) {
		if(particleType.name.empty()) {
			uint32_t counter = 1;
			std::string name = "Particle";
			while(isNameUsed(project.effect, name)) {
				name = "Particle" + std::to_string(counter++);
			}

			particleType.name = name;
		}
	}

	for(ForceField& forceField : project.effect.forceFields) {
		if(forceField.name.empty()) {
			uint32_t counter = 1;
			std::string name = "Force";
			while(isNameUsed(project.effect, name)) {
				name = "Force" + std::to_string(counter++);
			}

			forceField.name = name;
		}
	}

	for(Collider& collider : project.effect.colliders) {
		if(collider.name.empty()) {
			uint32_t counter = 1;
			std::string name = "Collider";
			while(isNameUsed(project.effect, name)) {
				name = "Collider" + std::to_string(counter++);
			}

			collider.name = name;
		}
	}

	fromJson(project.postProcessingPipeline, j, "postprocessing_pipeline");
	fromJson(project.backgroundColor, j, "background_color");
	fromJson(project.cameraPosition, j, "camera_position");
	fromJson(project.cameraZoom, j, "camera_zoom");
	fromJson(project.cameraFieldOfView, j, "camera_fov");
	fromJson(project.cameraPitch, j, "camera_pitch");
	fromJson(project.cameraYaw, j, "camera_yaw");
	fromJson(project.cameraRoll, j, "camera_roll");

	fromJson(project.renderSettings, j, "render_settings");
	fromJson(project.previewSettings, j, "preview_settings");
}

std::string serialize(const Project& project, const ResourceDatabase& resources, int32_t indent) {
	ResourceDatabase requiredResources;
	for(const auto& imageResource : resources.images) {
		if(isResourceUsed(project.effect, imageResource.first)) {
			requiredResources.images.insert(imageResource);
		}
	}

	nlohmann::ordered_json jsonData = nlohmann::ordered_json{
		{ "project", project },
		{ "resources", requiredResources }
	};

	return jsonData.dump(indent);
}

Project deserialize(std::istream& stream, ResourceDatabase& resources) {
	nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(stream);
	resources = jsonData.at("resources");

	return jsonData.at("project").get<Project>();
}
Project deserialize(const std::string& data, ResourceDatabase& resources) {
	nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data);
	resources = jsonData.at("resources");

	return jsonData.at("project").get<Project>();
}
Project deserialize(const char* data, std::size_t size, ResourceDatabase& resources) {
	nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(data, data + size);
	resources = jsonData.at("resources");

	return jsonData.at("project").get<Project>();
}
}