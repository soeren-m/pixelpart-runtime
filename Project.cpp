#include "Project.h"
#include "ShaderGraphBuilder.h"
#include "JSONUtil.h"

namespace pixelpart {
const uint32_t Project::version = 6;

void to_json(nlohmann::ordered_json& j, const Project& project) {
	j = nlohmann::ordered_json{
		{ "version", Project::version },
		{ "effect", project.effect },
		{ "postprocessing_pipeline", project.postProcessingPipeline },
		{ "background_color", project.backgroundColor },
		{ "camera_position", project.cameraPosition },
		{ "camera_zoom", project.cameraZoom },
		{ "render_settings", project.renderSettings },
		{ "preview_settings", project.previewSettings },
	};
}
void from_json(const nlohmann::ordered_json& j, Project& project) {
	uint32_t version = j.at("version");
	if(version == 0 || version > Project::version) {
		throw std::runtime_error("Unsupported project version " + std::to_string(version));
	}

	fromJson(project.effect, j, "effect", "particle_system");

	if(version <= 4) {
		if(j.contains("particle_system_metadata")) {
			const nlohmann::ordered_json& jEmitterMetadata = j.at("particle_system_metadata");

			for(uint32_t i = 0; i < jEmitterMetadata.size(); i++) {
				const nlohmann::ordered_json& jMetadata = jEmitterMetadata.at(i).at(1);
				const uint32_t emitterId = jEmitterMetadata.at(i).at(0);

				if(project.effect.particleEmitters.contains(emitterId) && jMetadata.contains("name")) {
					project.effect.particleEmitters.get(emitterId).name = jMetadata.at("name");
				}
			}
		}
	}

	for(ParticleEmitter& emitter : project.effect.particleEmitters) {
		if(emitter.name.empty()) {
			uint32_t counter = 1;
			std::string name = "Emitter";
			while(isNameUsed(project.effect, name)) {
				name = "Emitter" + std::to_string(counter++);
			}

			emitter.name = name;
		}
	}

	for(Sprite& sprite : project.effect.sprites) {
		if(sprite.name.empty()) {
			uint32_t counter = 1;
			std::string name = "Sprite";
			while(isNameUsed(project.effect, name)) {
				name = "Sprite" + std::to_string(counter++);
			}

			sprite.name = name;
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

	if(j.contains("view_settings")) {
		const nlohmann::ordered_json& jViewSettings = j.at("view_settings");
		std::string backgroundImageId;
		uint32_t backgroundImageLayer;
		vec2d backgroundImagePosition;
		vec2d backgroundImageSize;

		fromJson(project.backgroundColor, jViewSettings, "background_color", "");
		fromJson(project.cameraPosition, jViewSettings, "position", "");
		fromJson(project.cameraZoom, jViewSettings, "zoom", "");
		fromJson(backgroundImageId, jViewSettings, "background_image_id", "");
		fromJson(backgroundImageLayer, jViewSettings, "background_image_layer", "");
		fromJson(backgroundImagePosition, jViewSettings, "background_image_position", "");
		fromJson(backgroundImageSize, jViewSettings, "background_image_size", "");

		if(!backgroundImageId.empty()) {
			Sprite sprite;
			sprite.name = "__background__";
			sprite.motionPath = Curve<vec2d>(backgroundImagePosition);
			sprite.width = Curve<floatd>(backgroundImageSize.x);
			sprite.height = Curve<floatd>(backgroundImageSize.y);
			sprite.shader = ShaderGraphBuilder::buildTextureShaderGraph(backgroundImageId, "");
			sprite.layer = backgroundImageLayer;
			project.effect.sprites.add(sprite);
		}
	}
	else {
		fromJson(project.postProcessingPipeline, j, "postprocessing_pipeline", "");
		fromJson(project.backgroundColor, j, "background_color", "");
		fromJson(project.cameraPosition, j, "camera_position", "");
		fromJson(project.cameraZoom, j, "camera_zoom", "");
	}

	fromJson(project.renderSettings, j, "render_settings", "");
	fromJson(project.previewSettings, j, "preview_settings", "");
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