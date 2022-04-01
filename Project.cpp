#include "Project.h"
#include "JSONUtil.h"

namespace pixelpart {
const uint32_t Project::version = 5;

void to_json(nlohmann::ordered_json& j, const Project& project) {
	j = nlohmann::ordered_json{
		{ "version", Project::version },
		{ "effect", project.effect },
		{ "postprocessing_effects", project.postProcessingEffects },
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

	if(version <= 2) {
		for(uint32_t i = 0; i < project.effect.getNumParticleEmitters(); i++) {
			ParticleEmitter& emitter = project.effect.getParticleEmitterByIndex(i);
			if(emitter.parentId != NullId) {
				emitter.motionPath = Curve<vec2d>(vec2d(0.0));
			}
		}
	}

	if(version <= 4) {
		if(j.contains("particle_system_metadata")) {
			const nlohmann::ordered_json& jEmitterMetadata = j.at("particle_system_metadata");
			
			for(uint32_t i = 0; i < jEmitterMetadata.size(); i++) {
				const nlohmann::ordered_json& jMetadata = jEmitterMetadata.at(i).at(1);
				const uint32_t emitterId = jEmitterMetadata.at(i).at(0);
			
				if(project.effect.hasParticleEmitter(emitterId) && jMetadata.contains("name")) {
					project.effect.getParticleEmitter(emitterId).name = jMetadata.at("name");
				}
			}
		}
	}

	for(uint32_t i = 0; i < project.effect.getNumParticleEmitters(); i++) {
		ParticleEmitter& emitter = project.effect.getParticleEmitterByIndex(i);

		if(emitter.name.empty()) {
			uint32_t counter = 1;
			std::string name = "Emitter";
			while(project.effect.isNameUsed(name)) {
				name = "Emitter" + std::to_string(counter++);
			}

			emitter.name = name;
		}
	}

	for(uint32_t i = 0; i < project.effect.getNumForceFields(); i++) {
		ForceField& forceField = project.effect.getForceField(i);

		if(forceField.name.empty()) {
			uint32_t counter = 1;
			std::string name = "Force";
			while(project.effect.isNameUsed(name)) {
				name = "Force" + std::to_string(counter++);
			}

			forceField.name = name;
		}
	}

	for(uint32_t i = 0; i < project.effect.getNumColliders(); i++) {
		Collider& collider = project.effect.getCollider(i);

		if(collider.name.empty()) {
			uint32_t counter = 1;
			std::string name = "Collider";
			while(project.effect.isNameUsed(name)) {
				name = "Collider" + std::to_string(counter++);
			}

			collider.name = name;
		}
	}

	for(uint32_t i = 0; i < project.effect.getNumSprites(); i++) {
		Sprite& sprite = project.effect.getSprite(i);

		if(sprite.name.empty()) {
			uint32_t counter = 1;
			std::string name = "Sprite";
			while(project.effect.isNameUsed(name)) {
				name = "Sprite" + std::to_string(counter++);
			}

			sprite.name = name;
		}
	}

	if(j.contains("view_settings")) {
		const nlohmann::ordered_json& jViewSettings = j.at("view_settings");
		std::string backgroundImageId;
		uint32_t backgroundImageLayer;
		vec2d backgroundImagePosition;
		vec2d backgroundImageSize;

		fromJson(project.postProcessingEffects, jViewSettings, "postprocessing_effects", "");
		fromJson(project.backgroundColor, jViewSettings, "background_color", "");
		fromJson(project.cameraPosition, jViewSettings, "position", "");
		fromJson(project.cameraZoom, jViewSettings, "zoom", "");
		fromJson(backgroundImageId, jViewSettings, "background_image_id", "");
		fromJson(backgroundImageLayer, jViewSettings, "background_image_layer", "");
		fromJson(backgroundImagePosition, jViewSettings, "background_image_position", "");
		fromJson(backgroundImageSize, jViewSettings, "background_image_size", "");

		if(!backgroundImageId.empty()) {
			Sprite sprite;
			sprite.name = "BACKGROUND";
			sprite.image.id = backgroundImageId;
			sprite.layer = backgroundImageLayer;
			sprite.motionPath = Curve<vec2d>(backgroundImagePosition);
			sprite.width = Curve<floatd>(backgroundImageSize.x);
			sprite.height = Curve<floatd>(backgroundImageSize.y);
			project.effect.addSprite(sprite);
		}
	}
	else {
		fromJson(project.postProcessingEffects, j, "postprocessing_effects", "");
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
		if(project.effect.isResourceUsed(imageResource.first)) {
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