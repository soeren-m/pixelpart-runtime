#include "Project.h"

namespace pixelpart {
const uint32_t Project::version = 8u;

Effect& Project::effect() {
	return projectEffect;
}
const Effect& Project::effect() const {
	return projectEffect;
}

ImageEffectSettings& Project::imageEffectSettings() {
	return projectImageEffectSettings;
}
const ImageEffectSettings& Project::imageEffectSettings() const {
	return projectImageEffectSettings;
}

CameraSettings& Project::cameraSettings() {
	return projectCameraSettings;
}
const CameraSettings& Project::cameraSettings() const {
	return projectCameraSettings;
}

RenderSettings& Project::renderSettings() {
	return projectRenderSettings;
}
const RenderSettings& Project::renderSettings() const {
	return projectRenderSettings;
}

RenderSettings& Project::previewSettings() {
	return projectPreviewSettings;
}
const RenderSettings& Project::previewSettings() const {
	return projectPreviewSettings;
}

bool Project::isNameUsed(const std::string& name) const {
	for(const ParticleEmitter& particleEmitter : projectEffect.particleEmitters()) {
		if(particleEmitter.name() == name) {
			return true;
		}
	}
	for(const ParticleType& particleType : projectEffect.particleTypes()) {
		if(particleType.name() == name) {
			return true;
		}
	}
	for(const ForceField& forceField : projectEffect.forceFields()) {
		if(forceField.name() == name) {
			return true;
		}
	}
	for(const Collider& collider : projectEffect.colliders()) {
		if(collider.name() == name) {
			return true;
		}
	}
	for(const LightSource& lightSource : projectEffect.lightSources()) {
		if(lightSource.name() == name) {
			return true;
		}
	}

	return false;
}
bool Project::isResourceUsed(const std::string& resourceId) const {
	for(const ParticleType& particleType : projectEffect.particleTypes()) {
		if(particleType.materialInstance().materialId() == resourceId) {
			return true;
		}
		if(particleType.meshRendererSettings().meshResourceId == resourceId) {
			return true;
		}

		for(const auto& materialParameterEntry : particleType.materialInstance().materialParameters()) {
			if(materialParameterEntry.second.type() == pixelpart::VariantParameter::Value::type_resource_image &&
				resourceId == materialParameterEntry.second.valueResourceId()) {
				return true;
			}
		}
	}

	for(const ForceField& forceField : projectEffect.forceFields()) {
		if(resourceId == forceField.vectorResourceId()) {
			return true;
		}
	}

	for(const auto& resourceEntry : projectEffect.resources().materials()) {
		const MaterialResource& material = resourceEntry.second;

		for(const auto& nodeEntry : material.shaderGraph().nodes()) {
			for(const auto& nodeParameter : nodeEntry.second.parameters()) {
				if(nodeParameter.type() == pixelpart::VariantParameter::Value::type_resource_image &&
					resourceId == nodeParameter.valueResourceId()) {
					return true;
				}
			}
		}
	}

	return false;
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

void to_json(nlohmann::ordered_json& j, const Project& project) {
	j = nlohmann::ordered_json{
		{ "version", Project::version },
		{ "effect", project.effect() },
		{ "image_effects", project.imageEffectSettings() },
		{ "camera", project.cameraSettings() },
		{ "render", project.renderSettings() },
		{ "preview", project.previewSettings() },
	};
}
void from_json(const nlohmann::ordered_json& j, Project& project) {
	uint32_t version = j.at("version");
	if(version != Project::version) {
		throw std::runtime_error("Unsupported project version " + std::to_string(version));
	}

	project.effect() = j.value("effect", Effect());

	for(ParticleEmitter& particleEmitter : project.effect().particleEmitters()) {
		if(particleEmitter.name().empty()) {
			uint32_t counter = 1u;
			std::string name = "Emitter";
			while(project.isNameUsed( name)) {
				name = "Emitter" + std::to_string(counter++);
			}

			particleEmitter.name(name);
		}
	}

	for(ParticleType& particleType : project.effect().particleTypes()) {
		if(particleType.name().empty()) {
			uint32_t counter = 1u;
			std::string name = "Particle";
			while(project.isNameUsed(name)) {
				name = "Particle" + std::to_string(counter++);
			}

			particleType.name(name);
		}
	}

	for(ForceField& forceField : project.effect().forceFields()) {
		if(forceField.name().empty()) {
			uint32_t counter = 1u;
			std::string name = "Force";
			while(project.isNameUsed(name)) {
				name = "Force" + std::to_string(counter++);
			}

			forceField.name(name);
		}
	}

	for(Collider& collider : project.effect().colliders()) {
		if(collider.name().empty()) {
			uint32_t counter = 1u;
			std::string name = "Collider";
			while(project.isNameUsed(name)) {
				name = "Collider" + std::to_string(counter++);
			}

			collider.name(name);
		}
	}

	for(LightSource& lightSource : project.effect().lightSources()) {
		if(lightSource.name().empty()) {
			uint32_t counter = 1u;
			std::string name = "Light";
			while(project.isNameUsed(name)) {
				name = "Light" + std::to_string(counter++);
			}

			lightSource.name(name);
		}
	}

	project.imageEffectSettings() = j.value("image_effects", ImageEffectSettings());
	project.cameraSettings() = j.value("camera", CameraSettings());
	project.renderSettings() = j.value("render", RenderSettings());
	project.previewSettings() = j.value("preview", RenderSettings());
}
}